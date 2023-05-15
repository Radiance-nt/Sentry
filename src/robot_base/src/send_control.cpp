#include <ros/ros.h>
#include <serial/serial.h>
#include <iostream>
#include "control/crc_m.h"
#include <std_msgs/Float32.h>
#include <geometry_msgs/TwistStamped.h>
#include <std_msgs/String.h>
#include "HandlerUtils.hpp"
#include "msgExample.h"

using namespace std;

float speedX = 0;
float speedY = 0;
float angle = 0;
bool msg_flag = false;
float chassis_speed = -1;
float chassis_orientation = -1;

ros::Publisher chassis_status_pub;
std::string port_name;
ros::Publisher serial_pub; // add for publishing serial data
bool vec_break_flag = 0;
bool BREAK_ALLOWED = 1;
dataFromPC_t code;
geometry_msgs::TwistStamped chassis;

// 接收控制指令
void control_callback(const geometry_msgs::Twist::ConstPtr &msg) {
    msg_flag = true;
    speedX = (msg->linear.x);
    speedY = (msg->linear.y);
    angle = (msg->angular.z);
    // std::cout<<"speed "<< speed <<std::endl;
}


// 编码发送给底盘的控制指令
void control_coding(const float SpeedX, const float SpeedY, const float Angle) {
    // /* code */
    bool allow_aim;
    ros::param::get("/RobotControl/allow_nav", allow_aim);

    code.ctrlFromPC.markHead = 0x5e;
    code.ctrlFromPC.markTail = 0x71;
    if (allow_aim) {
        code.ctrlFromPC.setSpeedX = -SpeedY;
        code.ctrlFromPC.setSpeedY = SpeedX;
        code.ctrlFromPC.setAngle = Angle;
    } else {
        code.ctrlFromPC.setSpeedX = 0;
        code.ctrlFromPC.setSpeedY = 0;
        code.ctrlFromPC.setAngle = 0;
    }

}


int main(int argc, char **argv) {
    ros::init(argc, argv, "control_serial_node");
    ros::NodeHandle nh;
    ros::NodeHandle nh_private("~");

    std::string control_topic;
    std::string chassis_status_topic;

    nh_private.param<std::string>("control_topic", control_topic, "/cmd_vel");
    nh_private.param<std::string>("chassis_status", chassis_status_topic, "/robot/chassis_status");

    ros::Subscriber motor_control_sub = nh.subscribe<geometry_msgs::Twist>(control_topic, 100,
                                                                           control_callback);//订阅控制指令
    chassis_status_pub = nh.advertise<geometry_msgs::TwistStamped>(chassis_status_topic, 100);// 发布车辆速度及角度

    serial_pub = nh.advertise<std_msgs::String>("serial_data/write", 100); // create publisher object for serial data

    ros::Rate loop_rate(50);
    int cnt = 0;
    float SPEEDX = 0, SPEEDY = 0, ANGLE = 0;

    while (ros::ok()) {
        //std::cout<< vec_break_flag <<std::endl;
        // 紧急制动 若紧急制动标志位不为1或不允许紧急制动，则数十位数后启动车辆
        if (!vec_break_flag | !BREAK_ALLOWED) {
            if (cnt > 10) {
                cnt = 10;
                SPEEDX = speedX;
                SPEEDY = speedY;
                ANGLE = angle;
            } else { ANGLE = angle; }
        } else {
            SPEEDX = (speedX < 0) ? 0 : speedX; // 前方有障碍物，禁止前进
            SPEEDY = (speedY < 0) ? 0 : speedY;
            ANGLE = angle;
            cnt = 0;
        }
        //std::cout<< SPEED<<" "<<std::endl;
        // 编码控制指令
        control_coding(SPEEDX, SPEEDY, ANGLE);
//
//        printf("data:\n");
//        for (int i = 0; i < 14; i++) {
//            printf("%x ", (unsigned char) code.usbDataBuffer[i]);
//        }
//        printf("\n");

        bool allow_rotate, allow_hitBack;
        ros::param::get("/RobotControl/allow_rotate", allow_rotate);
        ros::param::get("/RobotControl/allow_hitBack", allow_hitBack);
        aimData aim_data{};
        std::memcpy(aim_data.aim, reinterpret_cast<char *>(code.usbDataBuffer), 14);
        aim_data.allow_rotate = allow_rotate;
        aim_data.allow_hitBack = allow_hitBack;
        auto data = serialize<aimData>(aim_data);
        std_msgs::String msg;
        msg.data = std::string(data, sizeof(aimData));

        serial_pub.publish(msg);

        msg_flag = false;
        ros::spinOnce();
        loop_rate.sleep();

        cnt++;
    }
    return 0;
}
