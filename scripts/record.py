import lcm
import pandas as pd
import time
from datetime import datetime
import signal
import sys
# from NN_IO_t import NN_IO_t
from BP4_NN_IO_t import BP4_NN_IO_t as NN_IO_t
from spi_command_t import spi_command_t
from leg_control_data_lcmt import leg_control_data_lcmt as leg_data

nn_data = []
spi_data = []
leg_raw_data = []
t0 = time.time()


def my_handler1(channel, data):
    msg = NN_IO_t.decode(data)
    temp = {}
    temp.update({'time': (time.time() - t0)})
    for i in range(35):
        temp.update({'input' + str(i): msg.input[i]})
        pass
    for i in range(12):
        temp.update({'output' + str(i): msg.output[i]})
        pass
    nn_data.append(temp)


# print("nn")

def my_handler2(channel, data):
    msg = spi_command_t.decode(data)
    temp = {}
    temp.update({'time': (time.time() - t0)})
    for i in range(4):
        temp.update({'q' + str(3 * i + 0): msg.q_des_abad[i]})
        temp.update({'q' + str(3 * i + 1): msg.q_des_hip[i]})
        temp.update({'q' + str(3 * i + 2): msg.q_des_knee[i]})
        temp.update({'qd' + str(3 * i + 0): msg.qd_des_abad[i]})
        temp.update({'qd' + str(3 * i + 1): msg.qd_des_hip[i]})
        temp.update({'qd' + str(3 * i + 2): msg.qd_des_knee[i]})
        pass
    spi_data.append(temp)


def my_handler3(channel, data):
    # print("leg")
    msg = leg_data.decode(data)
    temp = {}
    for i in range(12):
        temp.update({'raw_q' + str(i): msg.q[i]})
        temp.update({'raw_dq' + str(i): msg.qd[i]})
        pass
    leg_raw_data.append(temp)



lc = lcm.LCM()
subscription_state = lc.subscribe("NN_IO", my_handler1)
subscription_state = lc.subscribe("spi_command", my_handler2)
subscription_state = lc.subscribe("leg_control_data", my_handler3)
save_path = './../other/data/'


def sigint_handler(signum, frame):
    global is_sigint_up
    print("catch interrupt")
    df1 = pd.DataFrame(nn_data)
    df2 = pd.DataFrame(spi_data)
    df3 = pd.DataFrame(leg_raw_data)
    a = datetime.now()
    df1.to_excel(save_path + datetime.strftime(a, '%Y-%m-%d %H:%M:%S') + '_nn.xlsx')
    df2.to_excel(save_path + datetime.strftime(a, '%Y-%m-%d %H:%M:%S') + '_spi_command.xlsx')
    df3.to_excel(save_path + datetime.strftime(a, '%Y-%m-%d %H:%M:%S') + '_raw_data.xlsx')
    is_sigint_up = True


signal.signal(signal.SIGINT, sigint_handler)
is_sigint_up = False

while not is_sigint_up:
    try:
        lc.handle()
    except:
        pass
