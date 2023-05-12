//
// Created by radiance on 23-5-4.
//
#include <ros/ros.h>
#include <std_msgs/String.h>
#include "HandlerUtils.hpp"
#include "msgExample.h"

void serialDataCallback(const std_msgs::String::ConstPtr &msg) {
    // 处理串口数据，这里只是假设打印接收到的数据
    auto data = msg->data.c_str();

    auto sentryInfo = deserialize<sentryStrategy_t>(data);
    if ((sentryInfo.head != 0x5e) && (sentryInfo.tail != 0x71)){
        return;
    }
    ros::param::set("/RobotStatus/health", sentryInfo.sentryHPReal);
    ros::param::set("/RobotStatus/remainTime", sentryInfo.remainTime);
    ros::param::set("/RobotStatus/health", sentryInfo.sentryHPReal);
}

int main(int argc, char **argv) {
    ros::init(argc, argv, "serial_listener");
    ros::NodeHandle nh;

    // 创建 Subscriber 对象，订阅 serial_data 主题
    ros::Subscriber serial_sub = nh.subscribe<std_msgs::String>("serial_data/read", 100, serialDataCallback);

    ros::spin(); // 进入消息循环

    return 0;
}
