#include "HardwareBridge.h"
#include <sys/mman.h>
#include <unistd.h>
#include <cstring>
#include <thread>
//#include "rt/rt_interface_lcm.h"
#include "rt/rt_rc_interface.h"
#include "rt/rt_sbus.h"
#include "rt/rt_spi.h"
// #include "rt/rt_imu.h"
// #include "rt/rt_vectornav.h"
#include <Configuration.h>


#define printf_purple(fmt, args...) \
    printf("\e[1;35m" fmt "\e[0m", ## args)

/*!
 * If an error occurs during initialization, before motors are enabled, print
 * error and exit.
 * @param reason Error message string
 * @param printErrno If true, also print C errno
 */
void HardwareBridge::initError(const char *reason, bool printErrno) {
    printf("FAILED TO INITIALIZE HARDWARE: %s\n", reason);

    if (printErrno) {
        printf("Error: %s\n", strerror(errno));
    }

    exit(-1);
}

/*!
 * All initialization code that is common between Cheetah 3 and Mini Cheetah
 */
void HardwareBridge::initCommon() {
    printf("[HardwareBridge] Init stack\n");
    prefaultStack();
    printf("[HardwareBridge] Init scheduler\n");
    setupScheduler();
    if (!_interfaceLCM.good()) {
        initError("_interfaceLCM failed to initialize\n", false);
    }

    printf("[HardwareBridge] Subscribe LCM\n");
    _interfaceLCM.subscribe("interface", &HardwareBridge::handleGamepadLCM, this);
    _interfaceLCM.subscribe("interface_request",
                            &HardwareBridge::handleControlParameter, this);

    printf("[HardwareBridge] Start interface LCM handler\n");
    _interfaceLcmThread = std::thread(&HardwareBridge::handleInterfaceLCM, this);
}

void HardwareBridge::handleInterfaceLCM() {
    while (!_interfaceLcmQuit) _interfaceLCM.handle();
}

/*!
 * Writes to a 16 KB buffer on the stack. If we are using 4K pages for our
 * stack, this will make sure that we won't have a page fault when the stack
 * grows.  Also mlock's all pages associated with the current process, which
 * prevents the cheetah software from being swapped out.  If we do run out of
 * memory, the robot program will be killed by the OOM process killer (and
 * leaves a log) instead of just becoming unresponsive.
 */
void HardwareBridge::prefaultStack() {
    printf("[Init] Prefault stack...\n");
    volatile char stack[MAX_STACK_SIZE];
    memset(const_cast<char *>(stack), 0, MAX_STACK_SIZE);
    if (mlockall(MCL_CURRENT | MCL_FUTURE) == -1) {
        initError(
                "mlockall failed.  This is likely because you didn't run robot as "
                "root.\n",
                true);
    }
}

/*!
 * Configures the
 */
void HardwareBridge::setupScheduler() {
    printf("[Init] Setup RT Scheduler...\n");
    struct sched_param params;
    params.sched_priority = TASK_PRIORITY;
    if (sched_setscheduler(0, SCHED_FIFO, &params) == -1) {
        initError("sched_setscheduler failed.\n", true);
    }
}

void HardwareBridge::handleGamepadLCM(const lcm::ReceiveBuffer *rbuf,
                                      const std::string &chan,
                                      const gamepad_lcmt *msg) {
    (void) rbuf;
    (void) chan;
    _gamepadCommand.set(msg);
}

void HardwareBridge::publishVisualizationLCM() {
   cheetah_visualization_lcmt visualization_data;

   for (int i = 0; i < 3; i++) {
       visualization_data.x[i] = _mainCheetahVisualization.p[i];
   }

   for (int i = 0; i < 4; i++) {
       visualization_data.quat[i] = _mainCheetahVisualization.quat[i];
       visualization_data.rgba[i] = _mainCheetahVisualization.color[i];
   }

   for (int i = 0; i < 12; i++) {
       visualization_data.q[i] = _mainCheetahVisualization.q[i];
   }

   _visualizationLCM.publish("main_cheetah_visualization", &visualization_data);

}

void HardwareBridge::run_sbus() {
//    std::cout << "run sbus1" << std::endl;
    if (_port > 0) {
        int x = receive_sbus(_port);
        if (x) {
            //sbus_packet_complete();
            sbus_packet_complete2();
        }
    }
}

void HardwareBridge::handleControlParameter(
        const lcm::ReceiveBuffer *rbuf, const std::string &chan,
        const control_parameter_request_lcmt *msg) {
    (void) rbuf;
    (void) chan;
    if (msg->requestNumber <= _parameter_response_lcmt.requestNumber) {
        // nothing to do!
        printf(
                "[HardwareBridge] Warning: the interface has run a ControlParameter "
                "iteration, but there is no new request!\n");
        // return;
    }

    // sanity check
    s64 nRequests = msg->requestNumber - _parameter_response_lcmt.requestNumber;
    if (nRequests != 1) {
        printf("[ERROR] Hardware bridge: we've missed %ld requests\n",
               nRequests - 1);
    }

    switch (msg->requestKind) {
        case (s8) ControlParameterRequestKind::SET_USER_PARAM_BY_NAME: {
            if (!_userControlParameters) {
                printf("[Warning] Got user param %s, but not using user parameters!\n",
                       (char *) msg->name);
            } else {
                std::string name((char *) msg->name);
                ControlParameter &param = _userControlParameters->collection.lookup(name);

                // type check
                if ((s8) param._kind != msg->parameterKind) {
                    throw std::runtime_error(
                            "type mismatch for parameter " + name + ", robot thinks it is " +
                            controlParameterValueKindToString(param._kind) +
                            " but received a command to set it to " +
                            controlParameterValueKindToString(
                                    (ControlParameterValueKind) msg->parameterKind));
                }

                // do the actual set
                ControlParameterValue v;
                memcpy(&v, msg->value, sizeof(v));
                param.set(v, (ControlParameterValueKind) msg->parameterKind);

                // respond:
                _parameter_response_lcmt.requestNumber =
                        msg->requestNumber;  // acknowledge that the set has happened
                _parameter_response_lcmt.parameterKind =
                        msg->parameterKind;  // just for debugging print statements
                memcpy(_parameter_response_lcmt.value, msg->value, 64);
                //_parameter_response_lcmt.value = _parameter_request_lcmt.value; // just
                //for debugging print statements
                strcpy((char *) _parameter_response_lcmt.name,
                       name.c_str());  // just for debugging print statements
                _parameter_response_lcmt.requestKind = msg->requestKind;

                printf("[User Control Parameter] set %s to %s\n", name.c_str(),
                       controlParameterValueToString(
                               v, (ControlParameterValueKind) msg->parameterKind)
                               .c_str());
            }
        }
            break;

        case (s8) ControlParameterRequestKind::SET_ROBOT_PARAM_BY_NAME: {
            std::string name((char *) msg->name);
            ControlParameter &param = _robotParams.collection.lookup(name);

            // type check
            if ((s8) param._kind != msg->parameterKind) {
                throw std::runtime_error(
                        "type mismatch for parameter " + name + ", robot thinks it is " +
                        controlParameterValueKindToString(param._kind) +
                        " but received a command to set it to " +
                        controlParameterValueKindToString(
                                (ControlParameterValueKind) msg->parameterKind));
            }

            // do the actual set
            ControlParameterValue v;
            memcpy(&v, msg->value, sizeof(v));
            param.set(v, (ControlParameterValueKind) msg->parameterKind);

            // respond:
            _parameter_response_lcmt.requestNumber =
                    msg->requestNumber;  // acknowledge that the set has happened
            _parameter_response_lcmt.parameterKind =
                    msg->parameterKind;  // just for debugging print statements
            memcpy(_parameter_response_lcmt.value, msg->value, 64);
            //_parameter_response_lcmt.value = _parameter_request_lcmt.value; // just
            //for debugging print statements
            strcpy((char *) _parameter_response_lcmt.name,
                   name.c_str());  // just for debugging print statements
            _parameter_response_lcmt.requestKind = msg->requestKind;

            printf("[Robot Control Parameter] set %s to %s\n", name.c_str(),
                   controlParameterValueToString(
                           v, (ControlParameterValueKind) msg->parameterKind)
                           .c_str());

        }
            break;

        default: {
            throw std::runtime_error("parameter type unsupported");
        }
            break;
    }
    _interfaceLCM.publish("interface_response", &_parameter_response_lcmt);
//    if(REMOTE_CONTROL){
//        ;
//    }
//    else{
//        _interfaceLCM.publish("interface_response", &_parameter_response_lcmt);
//    };

}

MiniCheetahHardwareBridge::MiniCheetahHardwareBridge(RobotController *robot_ctrl, bool load_parameters_from_file)
        : HardwareBridge(robot_ctrl), _spiLcm(getLcmUrl(255)), _vectornav_lcm(getLcmUrl(255))/*,
          _contactLcm(getLcmUrl(255))*/ {
    _load_parameters_from_file = load_parameters_from_file;
}

void MiniCheetahHardwareBridge::run() {
    initCommon();
    initHardware();

    if (_load_parameters_from_file) {
        printf("[Hardware Bridge] Loading parameters from file...\n");

        try {
            _robotParams.initializeFromYamlFile(THIS_COM "config/mini-cheetah-defaults.yaml");
        }
        catch (std::exception &e) {
            printf("Failed to initialize robot parameters from yaml file: %s\n", e.what());
            exit(1);
        }
        printf("Loaded robot parameters\n");

        if (_userControlParameters) {
            try {
                _userControlParameters->initializeFromYamlFile(THIS_COM "config/default-user.yaml");
            } catch (std::exception &e) {
                printf("Failed to initialize user parameters from yaml file: %s\n", e.what());
                exit(1);
            }

            if (!_userControlParameters->isFullyInitialized()) {
                printf("Failed to initialize all user parameters\n");
                exit(1);
            }

            printf("Loaded user parameters\n");
        } else {
            printf("Did not load user parameters because there aren't any\n");
        }

    } else {
        printf("[Hardware Bridge] Loading parameters over LCM...\n");
        while (!_robotParams.isFullyInitialized()) {
            printf("[Hardware Bridge] Waiting for robot parameters...\n");
            usleep(1000000);
        }

        if (_userControlParameters) {
            while (!_userControlParameters->isFullyInitialized()) {
                printf("[Hardware Bridge] Waiting for user parameters...\n");
                usleep(1000000);
            }
        }
    }


    printf("[Hardware Bridge] Got all parameters, starting up!\n");

    //_robotRunner = new RobotRunner(&taskManager, 0.001f, "robot-control");
    // 将硬件的参数传递给robotRunner，将minicheetah这一层的参数全部复制给robotrunner这一层的参数
    _robotRunner =
            new RobotRunner(_controller, &taskManager, _robotParams.controller_dt, "robot-control");

    _robotRunner->driverCommand = &_gamepadCommand;
    _robotRunner->spiData = &_spiData;
    _robotRunner->spiCommand = &_spiCommand;
    _robotRunner->robotType = RobotType::MINI_CHEETAH;
    _robotRunner->vectorNavData = &_vectorNavData;
    _robotRunner->controlParameters = &_robotParams;
    _robotRunner->visualizationData = &_visualizationData;
    _robotRunner->cheetahMainVisualization = &_mainCheetahVisualization;

    _robotRunner->contact_force = &_contact_force;
    _robotRunner->_laser_distance = &laser_distance;
    _robotRunner->_laser_yaw = &laser_yaw;

    _robotRunner->init();
    _firstRun = false;

    // init control thread

    statusTask.start();  //开始线程状态打印的线程
    printf_purple("# 1 #\n");

    // spi Task start
    PeriodicMemberFunction<MiniCheetahHardwareBridge> spiTask(
            &taskManager, .002, "spi", &MiniCheetahHardwareBridge::runSpi, this);
    spiTask.start();  // 开始spi通讯的线程， 发布控制指令并更新机器人关节构形
    printf_purple("# 3 #\n");

    // robot controller start
    _robotRunner->start();  // 开启机器人控制的线程
    printf_purple("# 5 #\n");

    // visualization start
    PeriodicMemberFunction<MiniCheetahHardwareBridge> visualizationLCMTask(
            &taskManager, .0167, "lcm-vis",
            &MiniCheetahHardwareBridge::publishVisualizationLCM, this);
    visualizationLCMTask.start();  // 开启机器人参数传递到lcm里面的进程，lcm里面参数的数据用于上位机的sim可视化
    printf_purple("# 6 #\n");


    for (;;) {
        usleep(1000000);  //整个程序进入死循环
        // printf("joy %f\n", _robotRunner->driverCommand->leftStickAnalog[0]);  // gamepad的进程应该是在上位机上运行的，不需要在hardwarebridge里面运行
    }
}


void MiniCheetahHardwareBridge::initHardware() {
    printf("[MiniCheetahHardware] Init vectornav\n");
    _vectorNavData.quat << 1, 0, 0, 0;

    init_spi();

}

void MiniCheetahHardwareBridge::runSpi() {
    spi_command_t *cmd = get_spi_command();
    spi_data_t *data = get_spi_data();
    memcpy(cmd, &_spiCommand, sizeof(spi_command_t));
    spi_driver_run();
    memcpy(&_spiData, data, sizeof(spi_data_t));
    // for remote mode, no data is sent back from lcm
    _spiLcm.publish("spi_data", data);
    _spiLcm.publish("spi_command", cmd);

}

