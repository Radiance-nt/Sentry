#define  WRITE_SIZE 64
#include <ros/ros.h>
#include <serial/serial.h>
#include <iostream>
#include "control/crc_m.h"
#include <std_msgs/Float32MultiArray.h>
#include <std_msgs/Float32.h>
#include <sensor_msgs/Range.h>
#include <geometry_msgs/TwistStamped.h>
#include <thread>
#include <std_msgs/String.h>

using namespace std;

serial::Serial sp;
std::string port_name;
ros::Publisher serial_pub; // add for publishing serial data
void toggleLastCharacter(std::string &str) {
    if (str.empty()) {
        return;
    }
    char &lastChar = str.back();
    if (lastChar == '0') {
        lastChar = '1';
    } else if (lastChar == '1') {
        lastChar = '0';
    }
}

// 打开串口
int open_port() {
    // TODO: Add latch for Concurrency
    serial::Timeout to = serial::Timeout::simpleTimeout(100);
    sp.setPort(port_name);
    sp.setBaudrate(115200);
    sp.setTimeout(to);
    int retry = 0;
    while (true) {
        retry++;
        try {
            sp.open();
            break;
        }
        catch (std::exception &e) {
            ROS_ERROR_STREAM("Unable to open port: " << port_name);
            toggleLastCharacter(port_name);
            ros::Duration(0.1).sleep();
            if (retry > 4) {
                exit(0);
            }
        }
    }
    if (sp.isOpen()) {
        ROS_INFO_STREAM(port_name << " is Opened.");
    } else {
        return -1;
    }
    return 1;
}

void writeSerialCallback(const std_msgs::String::ConstPtr &msg) {

    try {
        printf("\n[Motor] data write: %x", (unsigned char) msg->data.c_str()[0]);

        sp.write(reinterpret_cast<const uint8_t *>(msg->data.c_str()), WRITE_SIZE);
    } catch (serial::IOException &e) {
        sp.close();
        open_port();
    } catch (std::exception &e) {
        sp.close();
        open_port();
    }
}

void readSerial() {
    while (true) {
        // check if data is available to read
        try {
            if (sp.available()) {
                std::string data_read = sp.read(sp.available());
                // process the data as needed

                // publish the data to the topic
                std_msgs::String msg;
                msg.data = data_read;
                printf("\n[Motor] Read from motor:\n");
                for (int i = 0; i < 64 ; i++) {
                    printf("%x ", data_read[i]);
                }
                serial_pub.publish(msg);
            }
        } catch (serial::IOException &e) {
            sp.close();
            open_port();
        } catch (std::exception &e) {
            sp.close();
            open_port();
        }

        ros::Duration(0.001).sleep();
    }
}


int main(int argc, char **argv) {
    ros::init(argc, argv, "serial_node");
    ros::NodeHandle nh;
    ros::NodeHandle nh_private("~");


    nh_private.param<std::string>("PortName", port_name, "/dev/ttyACM0");

    // 打开串口
    open_port();

    serial_pub = nh.advertise<std_msgs::String>("serial_data/read", 100); // create publisher object for serial data
    ros::Subscriber serial_sub = nh.subscribe<std_msgs::String>("serial_data/write", 100,
                                                                writeSerialCallback);//订阅控制指令

    // start the thread for reading and publishing serial data
    thread t1(readSerial);

    ros::Rate loop_rate(50);
    while (ros::ok()) {
        ros::spinOnce();
        loop_rate.sleep();
    }
    // join the thread before exiting the program
    t1.join();
    sp.close();
    return 0;
}
