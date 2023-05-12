cmake_minimum_required(VERSION 3.0.2)
project(lidar_filter)

## is used, also find other catkin packages
find_package(catkin REQUIRED COMPONENTS
  nav_msgs
  pcl_ros
  roscpp
  rospy
  std_msgs
  geometry_msgs
  sensor_msgs
)

catkin_package(
#  INCLUDE_DIRS include
#  LIBRARIES lidar_filter
#  CATKIN_DEPENDS nav_msgs pcl roscpp rospy std_msgs
#  DEPENDS system_lib
)


include_directories(
# include
  ${catkin_INCLUDE_DIRS}
)


find_package(PCL REQUIRED)

include_directories(
  include
		${PCL_INCLUDE_DIRS})


add_executable(lidar_filter src/lidar_filter.cpp)
target_link_libraries(lidar_filter ${catkin_LIBRARIES} ${PCL_LIBRARIES}   )


