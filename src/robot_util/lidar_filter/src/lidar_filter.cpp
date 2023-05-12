#include <ros/ros.h>
#include <sensor_msgs/PointCloud2.h>
#include <nav_msgs/OccupancyGrid.h>
#include <pcl_ros/transforms.h>
#include <pcl_ros/point_cloud.h>
#include <pcl/point_types.h>
#include <tf/transform_listener.h>

ros::Publisher cloud_filtered_pub;
ros::Publisher cloud_filtered_out_pub;
tf::TransformListener *tf_listener;
nav_msgs::OccupancyGrid::ConstPtr map;

void map_callback(const nav_msgs::OccupancyGrid::ConstPtr &map_msg) {
    map = map_msg;
}

void cloud_callback(const sensor_msgs::PointCloud2::ConstPtr &cloud_msg) {
    if (!map) {
        ROS_WARN("Map not received. Publishing original point cloud.");
        cloud_filtered_pub.publish(cloud_msg);
        return;
    }

    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZ>);
    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_transformed(new pcl::PointCloud<pcl::PointXYZ>);
    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_filtered(new pcl::PointCloud<pcl::PointXYZ>);
    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_filtered_out(new pcl::PointCloud<pcl::PointXYZ>);

    // Convert sensor_msgs/PointCloud2 to pcl::PointCloud<pcl::PointXYZ>
    pcl::fromROSMsg(*cloud_msg, *cloud);

    // Transform point cloud from lidar frame to map frame
    tf_listener->waitForTransform(cloud_msg->header.frame_id, map->header.frame_id, cloud_msg->header.stamp,
                                  ros::Duration(1.0));
    pcl_ros::transformPointCloud(map->header.frame_id, *cloud, *cloud_transformed, *tf_listener);


    // Process the point cloud and remove points within the occupied area of the map
    for (const auto &point: cloud_transformed->points) {
        int map_x = static_cast<int>((point.x - map->info.origin.position.x) / map->info.resolution);
        int map_y = static_cast<int>((point.y - map->info.origin.position.y) / map->info.resolution);
//        std::cout << map_y << " "  << map->info.height << " ";
//        std::cout << point.x << " " << point.y << " " << point.z << " # " << map_x << " " << map_y << " "
//                  << map->info.origin.position.x << " " << map->info.origin.position.y << " ###         ";
//        if ((map_y > 150 ) && (map_x > 4000)) {
//            cloud_filtered->points.push_back(point);
//        } else {
//            cloud_filtered_out->points.push_back(point);
//        }

        int map_idx = map_y * map->info.width + map_x;
        bool flag = 0;
        if (map_x >= 0 && map_x < map->info.width && map_y >= 0 && map_y < map->info.height) {
            int occupancy_value = map->data[map_idx];
            if (occupancy_value < 30) { // You can adjust this threshold according to your requirement
                cloud_filtered->points.push_back(point);
                flag = 1;
            }
        }
        if (flag == 0) {
            cloud_filtered_out->points.push_back(point);
        }
    }
    // Transform point cloud back to lidar frame
    cloud_filtered->header.frame_id = cloud_transformed->header.frame_id;
    cloud_filtered_out->header.frame_id = cloud_transformed->header.frame_id;
    tf_listener->waitForTransform(map->header.frame_id, cloud_msg->header.frame_id, cloud_msg->header.stamp,
                                  ros::Duration(1.0));
    pcl_ros::transformPointCloud(cloud_msg->header.frame_id, *cloud_filtered, *cloud_filtered, *tf_listener);
    pcl_ros::transformPointCloud(cloud_msg->header.frame_id, *cloud_filtered_out, *cloud_filtered_out, *tf_listener);

    // Convert pcl::PointCloud<pcl::PointXYZ> to sensor_msgs/PointCloud2
    sensor_msgs::PointCloud2 cloud_filtered_msg;
    sensor_msgs::PointCloud2 cloud_filtered_out_msg;
    pcl::toROSMsg(*cloud_filtered, cloud_filtered_msg);
    pcl::toROSMsg(*cloud_filtered_out, cloud_filtered_out_msg);
    cloud_filtered_out_msg.header = cloud_msg->header;
    cloud_filtered_msg.header = cloud_msg->header;
//    ROS_INFO_STREAM("Cloud size: " << cloud_msg->data.size() << " bytes, number of points: " << cloud->points.size());
//    ROS_INFO_STREAM("Filtered point cloud size: " << cloud_filtered_msg.data.size() << " bytes, number of points: "
//                                                  << cloud_filtered->points.size());
//    ROS_INFO_STREAM(
//            "Filtered out point cloud size: " << cloud_filtered_out_msg.data.size() << " bytes, number of points: "
//                                              << cloud_filtered_out->points.size());
    // Publish the filtered point cloud
    cloud_filtered_pub.publish(cloud_filtered_msg);
    cloud_filtered_out_pub.publish(cloud_filtered_out_msg);
}

int main(int argc, char **argv) {
    ros::init(argc, argv, "cloud_filter");
    ros::NodeHandle nh;

    tf_listener = new tf::TransformListener();

    ros::Subscriber cloud_sub = nh.subscribe("/cloud_registered", 1, cloud_callback);
    ros::Subscriber map_sub = nh.subscribe("/filter_map", 1, map_callback);
    cloud_filtered_pub = nh.advertise<sensor_msgs::PointCloud2>("/cloud_in", 1);
    cloud_filtered_out_pub = nh.advertise<sensor_msgs::PointCloud2>("/cloud_out", 1);

    ros::spin();

    delete tf_listener;

    return 0;
}
