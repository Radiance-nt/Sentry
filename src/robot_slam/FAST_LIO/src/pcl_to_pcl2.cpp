#include <ros/ros.h>
#include <sensor_msgs/PointCloud.h>
#include <sensor_msgs/PointCloud2.h>
#include <pcl_conversions/pcl_conversions.h>
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl_conversions/pcl_conversions.h>
#include <sensor_msgs/PointCloud2.h>

ros::Publisher pcl2_pub;

void pclCallback(const sensor_msgs::PointCloud::ConstPtr& msg)
{
  // Convert sensor_msgs::PointCloud to pcl::PointCloud<pcl::PointXYZ>
  pcl::PointCloud<pcl::PointXYZ>::Ptr pcl_cloud(new pcl::PointCloud<pcl::PointXYZ>());
  for (const auto& p : msg->points) {
    pcl_cloud->push_back(pcl::PointXYZ(p.x, p.y, p.z));
  }

  // Convert pcl::PointCloud<pcl::PointXYZ> to sensor_msgs::PointCloud2
  sensor_msgs::PointCloud2 pcl2_cloud;
  pcl::toROSMsg(*pcl_cloud, pcl2_cloud);
  pcl2_cloud.header = msg->header;

  // Publish the sensor_msgs::PointCloud2 to /livox_horizon_points
  pcl2_pub.publish(pcl2_cloud);
}

int main(int argc, char** argv)
{
  ros::init(argc, argv, "pcl_to_pcl2_converter");
  ros::NodeHandle nh;

  pcl2_pub = nh.advertise<sensor_msgs::PointCloud2>("livox_horizon_points", 1);
  ros::Subscriber pcl_sub = nh.subscribe("pcl", 1, pclCallback);

  ros::spin();

  return 0;
}
