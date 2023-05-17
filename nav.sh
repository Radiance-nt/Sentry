#!/bin/bash

rosparam set /RobotControl/allow_nav "true"
source devel/setup.bash
roslaunch robot_nav robot_nav.launch
