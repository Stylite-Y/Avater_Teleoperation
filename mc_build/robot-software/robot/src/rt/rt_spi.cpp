/*!
 * @file rt_spi.h
 * @brief SPI communication to spine board
 */
#ifdef linux

#include <byteswap.h>
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <linux/spi/spidev.h>
#include "rt/rt_spi.h"
#include <lcm/lcm-cpp.hpp>

// define gpio operation related string
#define SYSFS_GPIO_EXPORT "/sys/class/gpio/export"
#define SYSFS_GPIO_UNEXPORT "/sys/class/gpio/unexport"
#define SYSFS_GPIO_RST_PIN_VAL "25"
#define SYSFS_GPIO_RST_DIR "/sys/class/gpio/gpio25/direction"
#define SYSFS_GPIO_RST_DIR_VAL "out"
#define SYSFS_GPIO_RST_VAL "/sys/class/gpio/gpio25/value"
#define SYSFS_GPIO_RST_VAL_H "1"
#define SYSFS_GPIO_RST_VAL_L "0"

//#define K_ABAD_OFFSET_POS 0.7f
//#define K_HIP_OFFSET_POS 0.96f
//#define K_KNEE_OFFSET_POS 4.166f

#define K_ABAD_OFFSET_POS 0.56f
#define K_HIP_OFFSET_POS 1.04f
#define K_KNEE_OFFSET_POS 4.166f

unsigned char spi_mode = SPI_MODE_0;
unsigned char spi_bits_per_word = 32;    // 32 bit communication
//unsigned int spi_speed = 6000000;
unsigned int spi_speed = 12500000;       // 12M is still safe and high speed in simple case test
//unsigned int spi_speed = 25000000;
uint8_t lsb = 0x01;

int spi_1_fd = -1;
int spi_2_fd = -1;

int spi_open();

static spine_cmd_t g_spine_cmd;    // global value to store the data for spi communication
static spine_data_t g_spine_data;

spi_command_t spi_command_drv;
spi_data_t spi_data_drv;
spi_torque_t spi_torque;

pthread_mutex_t spi_mutex;

const float max_torque[3] = {18.f, 18.f, 28.f};  // TODO CHECK WITH BEN
const float wimp_torque[3] = {6.f, 6.f, 6.f};    // TODO CHECK WITH BEN
const float disabled_torque[3] = {0.f, 0.f, 0.f};

// only used for actual robot
const float abad_side_sign[4] = {-1.f, -1.f, 1.f, 1.f};
const float hip_side_sign[4] = {-1.f, 1.f, -1.f, 1.f};
const float knee_side_sign[4] = {-.6429f, .6429f, -.6429f, .6429f};

// only used for actual robot
const float abad_offset[4] = {-K_ABAD_OFFSET_POS, K_ABAD_OFFSET_POS,
                              K_ABAD_OFFSET_POS, -K_ABAD_OFFSET_POS};
const float hip_offset[4] = {-K_HIP_OFFSET_POS, K_HIP_OFFSET_POS,
                             -K_HIP_OFFSET_POS, K_HIP_OFFSET_POS};
const float knee_offset[4] = {K_KNEE_OFFSET_POS, -K_KNEE_OFFSET_POS,
                              K_KNEE_OFFSET_POS, -K_KNEE_OFFSET_POS};
// spi message struct
struct spi_ioc_transfer spi_message[1];
/*!
 * Compute SPI message checksum
 * @param data : input
 * @param len : length (in 32-bit words)
 * @return
 */
uint32_t xor_checksum(uint32_t *data, size_t len) {
    uint32_t t = 0;
    for (size_t i = 0; i < len; i++) t = t ^ data[i];
    return t;
}

/*!
 * Emulate the spi board to estimate the torque.
 */
void fake_spine_control(spi_command_t *cmd, spi_data_t *data,
                        spi_torque_t *torque_out, int board_num) {
    torque_out->tau_abad[board_num] =
            cmd->kp_abad[board_num] *
            (cmd->q_des_abad[board_num] - data->q_abad[board_num]) +
            cmd->kd_abad[board_num] *
            (cmd->qd_des_abad[board_num] - data->qd_abad[board_num]) +
            cmd->tau_abad_ff[board_num];

    torque_out->tau_hip[board_num] =
            cmd->kp_hip[board_num] *
            (cmd->q_des_hip[board_num] - data->q_hip[board_num]) +
            cmd->kd_hip[board_num] *
            (cmd->qd_des_hip[board_num] - data->qd_hip[board_num]) +
            cmd->tau_hip_ff[board_num];

    torque_out->tau_knee[board_num] =
            cmd->kp_knee[board_num] *
            (cmd->q_des_knee[board_num] - data->q_knee[board_num]) +
            cmd->kd_knee[board_num] *
            (cmd->qd_des_knee[board_num] - data->qd_knee[board_num]) +
            cmd->tau_knee_ff[board_num];

    const float *torque_limits = disabled_torque;

    if (cmd->flags[board_num] & 0b1) {
        if (cmd->flags[board_num] & 0b10)
            torque_limits = wimp_torque;
        else
            torque_limits = max_torque;
    }

    if (torque_out->tau_abad[board_num] > torque_limits[0])
        torque_out->tau_abad[board_num] = torque_limits[0];
    if (torque_out->tau_abad[board_num] < -torque_limits[0])
        torque_out->tau_abad[board_num] = -torque_limits[0];

    if (torque_out->tau_hip[board_num] > torque_limits[1])
        torque_out->tau_hip[board_num] = torque_limits[1];
    if (torque_out->tau_hip[board_num] < -torque_limits[1])
        torque_out->tau_hip[board_num] = -torque_limits[1];

    if (torque_out->tau_knee[board_num] > torque_limits[2])
        torque_out->tau_knee[board_num] = torque_limits[2];
    if (torque_out->tau_knee[board_num] < -torque_limits[2])
        torque_out->tau_knee[board_num] = -torque_limits[2];
}

/*
 * trigger reset signal for stm32
 * reset the bottom communication chip's inner state
 * */
int nrst_reset() {
    int reset_gpio_fd = -1;
    ssize_t res = 0;

    reset_gpio_fd = open(SYSFS_GPIO_UNEXPORT, O_WRONLY);
    if (reset_gpio_fd == -1) {
        printf("[ERROR: RT SPI] UP Board fail to open unexport\n");
        return EXIT_FAILURE;
    }

    res = write(reset_gpio_fd, SYSFS_GPIO_RST_PIN_VAL, sizeof(SYSFS_GPIO_RST_PIN_VAL));
    if (res == -1) perror("[ERROR: RT SPI] Reset pin is not opened in default");
    close(reset_gpio_fd);

    reset_gpio_fd = open(SYSFS_GPIO_EXPORT, O_WRONLY);
    if (reset_gpio_fd == -1) {
        printf("[ERROR: RT SPI] UP Board fail to open export\n");
        exit(0);
        return EXIT_FAILURE;
    }

    res = write(reset_gpio_fd, SYSFS_GPIO_RST_PIN_VAL, sizeof(SYSFS_GPIO_RST_PIN_VAL));
    if (res == -1) perror("[ERROR: RT SPI] fail to set reset pin");
    close(reset_gpio_fd);

    reset_gpio_fd = open(SYSFS_GPIO_RST_DIR, O_WRONLY);
    if (reset_gpio_fd == -1) {
        printf("[ERROR: RT SPI] Radio hard reset pin direction open error.\n");
        return EXIT_FAILURE;
    }
    res = write(reset_gpio_fd, SYSFS_GPIO_RST_DIR_VAL, sizeof(SYSFS_GPIO_RST_DIR_VAL));
    if (res == -1) perror("[ERROR: RT SPI] can't set direction of pin");
    close(reset_gpio_fd);

    reset_gpio_fd = open(SYSFS_GPIO_RST_VAL, O_RDWR);
    if (reset_gpio_fd == -1) {
        printf("[ERROR: RT SPI] Radio hard reset pin value open error.\n");
        return EXIT_FAILURE;
    }
    res = write(reset_gpio_fd, SYSFS_GPIO_RST_VAL_H, sizeof(SYSFS_GPIO_RST_VAL_H));
    if (res == -1) perror("[ERROR: RT SPI] fail to pull up reset pin");
    usleep(100);
    res = write(reset_gpio_fd, SYSFS_GPIO_RST_VAL_L, sizeof(SYSFS_GPIO_RST_VAL_L));
    if (res == -1) perror("[ERROR: RT SPI] fail to pull down reset pin");
    usleep(100);
    res = write(reset_gpio_fd, SYSFS_GPIO_RST_VAL_H, sizeof(SYSFS_GPIO_RST_VAL_H));
    if (res == -1) perror("[ERROR: RT SPI] fail to pull up reset pin");
    usleep(100);
    close(reset_gpio_fd);
    usleep(1000000);
    return 0;
}

/*!
 * Initialize SPI
 */
void init_spi() {
    // check sizes:
    size_t command_size = sizeof(spi_command_t);
    size_t data_size = sizeof(spi_data_t);

    memset(&spi_command_drv, 0, sizeof(spi_command_drv));
    memset(&spi_data_drv, 0, sizeof(spi_data_drv));

    if (pthread_mutex_init(&spi_mutex, NULL) != 0)
        printf("[ERROR: RT SPI] Failed to create spi data mutex\n");

    if (command_size != K_EXPECTED_COMMAND_SIZE) {
        printf("[RT SPI] Error command size is %ld, expected %d\n", command_size,
               K_EXPECTED_COMMAND_SIZE);
    } else
        printf("[RT SPI] command size good\n");

    if (data_size != K_EXPECTED_DATA_SIZE) {
        printf("[RT SPI] Error data size is %ld, expected %d\n", data_size,
               K_EXPECTED_DATA_SIZE);
    } else
        printf("[RT SPI] data size good\n");

    nrst_reset();
    printf("[RT SPI] Reset SPINE board\n");
    printf("[RT SPI] Open\n");
    spi_open();
}

/*!
 * Open SPI device
 */
int spi_open() {
    int rv = 0;

    spi_1_fd = open("/dev/spidev2.0", O_RDWR);
    if (spi_1_fd < 0) perror("[ERROR] Couldn't open spidev 2.0");
    spi_2_fd = open("/dev/spidev2.1", O_RDWR);
    if (spi_2_fd < 0) perror("[ERROR] Couldn't open spidev 2.1");

    rv = ioctl(spi_1_fd, SPI_IOC_WR_MODE, &spi_mode);
    if (rv < 0) perror("[ERROR] ioctl spi_ioc_wr_mode (1)");

    rv = ioctl(spi_2_fd, SPI_IOC_WR_MODE, &spi_mode);
    if (rv < 0) perror("[ERROR] ioctl spi_ioc_wr_mode (2)");

    rv = ioctl(spi_1_fd, SPI_IOC_RD_MODE, &spi_mode);
    if (rv < 0) perror("[ERROR] ioctl spi_ioc_rd_mode (1)");

    rv = ioctl(spi_2_fd, SPI_IOC_RD_MODE, &spi_mode);
    if (rv < 0) perror("[ERROR] ioctl spi_ioc_rd_mode (2)");

    rv = ioctl(spi_1_fd, SPI_IOC_WR_BITS_PER_WORD, &spi_bits_per_word);
    if (rv < 0) perror("[ERROR] ioctl spi_ioc_wr_bits_per_word (1)");

    rv = ioctl(spi_2_fd, SPI_IOC_WR_BITS_PER_WORD, &spi_bits_per_word);
    if (rv < 0) perror("[ERROR] ioctl spi_ioc_wr_bits_per_word (2)");

    rv = ioctl(spi_1_fd, SPI_IOC_RD_BITS_PER_WORD, &spi_bits_per_word);
    if (rv < 0) perror("[ERROR] ioctl spi_ioc_rd_bits_per_word (1)");

    rv = ioctl(spi_2_fd, SPI_IOC_RD_BITS_PER_WORD, &spi_bits_per_word);
    if (rv < 0) perror("[ERROR] ioctl spi_ioc_rd_bits_per_word (2)");

    rv = ioctl(spi_1_fd, SPI_IOC_WR_MAX_SPEED_HZ, &spi_speed);
    if (rv < 0) perror("[ERROR] ioctl spi_ioc_wr_max_speed_hz (1)");
    rv = ioctl(spi_2_fd, SPI_IOC_WR_MAX_SPEED_HZ, &spi_speed);
    if (rv < 0) perror("[ERROR] ioctl spi_ioc_wr_max_speed_hz (2)");

    rv = ioctl(spi_1_fd, SPI_IOC_RD_MAX_SPEED_HZ, &spi_speed);
    if (rv < 0) perror("[ERROR] ioctl spi_ioc_rd_max_speed_hz (1)");
    rv = ioctl(spi_2_fd, SPI_IOC_RD_MAX_SPEED_HZ, &spi_speed);
    if (rv < 0) perror("[ERROR] ioctl spi_ioc_rd_max_speed_hz (2)");

    rv = ioctl(spi_1_fd, SPI_IOC_RD_LSB_FIRST, &lsb);
    if (rv < 0) perror("[ERROR] ioctl spi_ioc_rd_lsb_first (1)");

    rv = ioctl(spi_2_fd, SPI_IOC_RD_LSB_FIRST, &lsb);
    if (rv < 0) perror("[ERROR] ioctl spi_ioc_rd_lsb_first (2)");
    return rv;
}

int spi_driver_iterations = 0;

/*!
 * convert spi command to spine_cmd_t
 */
void spi_to_spine(spi_command_t *cmd, spine_cmd_t *spine_cmd, int leg_0) {
    for (int i = 0; i < 2; i++) {
        // spine_cmd->q_des_abad[i] = (cmd->q_des_abad[i+leg_0] +
        // abad_offset[i+leg_0]) * abad_side_sign[i+leg_0]; spine_cmd->q_des_hip[i]
        // = (cmd->q_des_hip[i+leg_0] + hip_offset[i+leg_0]) *
        // hip_side_sign[i+leg_0]; spine_cmd->q_des_knee[i] =
        // (cmd->q_des_knee[i+leg_0] + knee_offset[i+leg_0]) /
        // knee_side_sign[i+leg_0];
        spine_cmd->q_des_abad[i] =
                (cmd->q_des_abad[i + leg_0] * abad_side_sign[i + leg_0]) +
                abad_offset[i + leg_0];
        spine_cmd->q_des_hip[i] =
                (cmd->q_des_hip[i + leg_0] * hip_side_sign[i + leg_0]) +
                hip_offset[i + leg_0];
        spine_cmd->q_des_knee[i] =
                (cmd->q_des_knee[i + leg_0] / knee_side_sign[i + leg_0]) +
                knee_offset[i + leg_0];

        spine_cmd->qd_des_abad[i] =
                cmd->qd_des_abad[i + leg_0] * abad_side_sign[i + leg_0];
        spine_cmd->qd_des_hip[i] =
                cmd->qd_des_hip[i + leg_0] * hip_side_sign[i + leg_0];
        spine_cmd->qd_des_knee[i] =
                cmd->qd_des_knee[i + leg_0] / knee_side_sign[i + leg_0];

        spine_cmd->kp_abad[i] = cmd->kp_abad[i + leg_0];
        spine_cmd->kp_hip[i] = cmd->kp_hip[i + leg_0];
        spine_cmd->kp_knee[i] = cmd->kp_knee[i + leg_0];

        spine_cmd->kd_abad[i] = cmd->kd_abad[i + leg_0];
        spine_cmd->kd_hip[i] = cmd->kd_hip[i + leg_0];
        spine_cmd->kd_knee[i] = cmd->kd_knee[i + leg_0];

        spine_cmd->tau_abad_ff[i] =
                cmd->tau_abad_ff[i + leg_0] * abad_side_sign[i + leg_0];
        spine_cmd->tau_hip_ff[i] =
                cmd->tau_hip_ff[i + leg_0] * hip_side_sign[i + leg_0];
        spine_cmd->tau_knee_ff[i] =
                cmd->tau_knee_ff[i + leg_0] * knee_side_sign[i + leg_0];

        spine_cmd->flags[i] = cmd->flags[i + leg_0];
    }
    spine_cmd->checksum = xor_checksum((uint32_t *) spine_cmd, 32);
}

/*!
 * convert spine_data_t to spi data
 */
void spine_to_spi(spi_data_t *data, spine_data_t *spine_data, int leg_0) {
    for (int i = 0; i < 2; i++) {
        data->q_abad[i + leg_0] = (spine_data->q_abad[i] - abad_offset[i + leg_0]) *
                                  abad_side_sign[i + leg_0];
        data->q_hip[i + leg_0] = (spine_data->q_hip[i] - hip_offset[i + leg_0]) *
                                 hip_side_sign[i + leg_0];
        data->q_knee[i + leg_0] = (spine_data->q_knee[i] - knee_offset[i + leg_0]) *
                                  knee_side_sign[i + leg_0];

        data->qd_abad[i + leg_0] =
                spine_data->qd_abad[i] * abad_side_sign[i + leg_0];
        data->qd_hip[i + leg_0] = spine_data->qd_hip[i] * hip_side_sign[i + leg_0];
        data->qd_knee[i + leg_0] =
                spine_data->qd_knee[i] * knee_side_sign[i + leg_0];

        data->tau_abad[i+leg_0] = spine_data->tau_abad[i] / abad_side_sign[i+leg_0];
        data->tau_hip[i+leg_0] = spine_data->tau_hip[i] / hip_side_sign[i+leg_0];
        data->tau_knee[i+leg_0] = spine_data->tau_knee[i] / knee_side_sign[i+leg_0];

        data->flags[i + leg_0] = spine_data->flags[i];
    }

    uint32_t calc_checksum = xor_checksum((uint32_t *) spine_data, 20);
    if (calc_checksum != (uint32_t) spine_data->checksum)
        printf("SPI ERROR BAD CHECKSUM GOT 0x%hx EXPECTED 0x%hx\n", calc_checksum,
               spine_data->checksum);
}

/*!
 * send receive data and command from spine
 */
void spi_send_receive(spi_command_t *command, spi_data_t *data) {
    // 这里面的操作不会导致指令重置为0
    // update driver status flag
    spi_driver_iterations++;
    data->spi_driver_status = spi_driver_iterations << 16;

    // transmit and receive buffers
    uint8_t tx_buf[K_BYTES_PER_MESSAGE];
    uint8_t rx_buf[K_BYTES_PER_MESSAGE];
    for (int spi_board = 0; spi_board < 2; spi_board++) {
        // copy command into spine type:
        spi_to_spine(command, &g_spine_cmd, spi_board * 2);

        // pointers to command/data spine array
        uint8_t *cmd_d = (uint8_t *) &g_spine_cmd;
        uint8_t *data_d = (uint8_t *) &g_spine_data;
        // zero rx buffer
        memset(rx_buf, 0, K_BYTES_PER_MESSAGE);
        // copy into tx buffer flipping bytes
        for (int i = 0; i < K_WORDS_PER_MESSAGE/2; i++){
            // swap data
            // tx_buf[2*i] = cmd_d[2*i+1];
            // tx_buf[2*i+1] = cmd_d[2*i];
            tx_buf[4*i+0] = cmd_d[4*i+2];
            tx_buf[4*i+1] = cmd_d[4*i+3];
            tx_buf[4*i+2] = cmd_d[4*i+0];
            tx_buf[4*i+3] = cmd_d[4*i+1];

            // in addition
            // a=a^b;b=b^a;a=a^b; can realize quick data swap, this is very interesting
        }
        // zero message struct.
        memset(spi_message, 0, 1 * sizeof(struct spi_ioc_transfer));
        // set up message struct
        spi_message[0].bits_per_word = spi_bits_per_word;
        spi_message[0].cs_change = 1;
        spi_message[0].delay_usecs = 0;
        spi_message[0].len = K_BYTES_PER_MESSAGE;
        spi_message[0].rx_buf = (uint64_t) rx_buf;
        spi_message[0].tx_buf = (uint64_t) tx_buf;

        // do spi communication
        (void) ioctl(spi_board == 0 ? spi_1_fd : spi_2_fd, SPI_IOC_MESSAGE(1), &spi_message);
        // flip bytes the other way
        for (int i = 0; i < 42/2; i++){
            // data_d[i] = (rx_buf[i] >> 8) + ((rx_buf[i] & 0xff) << 8);
            // data_d[i*2] = rx_buf[2*i+1];
            // data_d[i*2+1] = rx_buf[2*i]
            data_d[i*4+0] = rx_buf[i*4+2];  // the copy order is important, make sure
            data_d[i*4+1] = rx_buf[i*4+3];
            data_d[i*4+2] = rx_buf[i*4+0];
            data_d[i*4+3] = rx_buf[i*4+1];
        }
        // copy back to data
        spine_to_spi(data, &g_spine_data, spi_board * 2);
    }
}

/*!
 * Run SPI
 */
void spi_driver_run() {
    // do spi board calculations
    for (int i = 0; i < 4; i++) {
        fake_spine_control(&spi_command_drv, &spi_data_drv, &spi_torque, i);
    }

    // in here, the driver is good
    pthread_mutex_lock(&spi_mutex);  // 因为外面是直接把数据拷贝进来的，所以这边不需要加进程锁
    spi_send_receive(&spi_command_drv, &spi_data_drv);
    pthread_mutex_unlock(&spi_mutex);
}

/*!
 * Get the spi command
 */
spi_command_t *get_spi_command() { return &spi_command_drv; }

/*!
 * Get the spi data
 */
spi_data_t *get_spi_data() { return &spi_data_drv; }

#endif
