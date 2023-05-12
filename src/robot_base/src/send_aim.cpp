#include <ros/ros.h>
#include <std_msgs/String.h>

ros::Publisher serial_pub;

bool allow_aim = false;

void serialDataCallback(const std_msgs::String::ConstPtr &msg) {
    ros::param::get("/RobotControl/allow_aim", allow_aim);

    if (allow_aim) {
        serial_pub.publish(msg);
    }
}

int main(int argc, char **argv) {
    ros::init(argc, argv, "serial_pub_sub");

    ros::NodeHandle nh;
    ros::Publisher serial_pub = nh.advertise<std_msgs::String>("/serial_data/write", 64);
    // create subscriber object for serial data

    ros::Subscriber serial_sub = nh.subscribe<std_msgs::String>("/cmd_aim", 64,
                                                                &serialDataCallback);
    ros::spin();
    return 0;
}