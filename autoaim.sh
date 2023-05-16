#!/bin/bash

source /opt/intel/openvino_2021/bin/setupvars.sh
source ~/aim_ros/devel/setup.bash

rosparam set /RobotControl/allow_aim "true"
roslaunch infantry_new infantry_new.launch