#!/bin/bash

# enable multicast and add route for lcm out the top
# echo 1234

cd /home/bp5/robot-software/build
export SUDO_ASKPASS=./_PWD_TEMP_
# to auto start the controller, de comment the following command
sudo -A ./run_mc2.sh ./bp5_ctrl
#sudo -A ./run_mc2.sh ./sm_ctrl
