#!/home/radiance/miniconda3/envs/ros/bin/python

# coding=utf8
from __future__ import print_function, division, absolute_import

import argparse
import time

import rospy
import tf.transformations
from geometry_msgs.msg import Pose, Point, Quaternion, PoseStamped, PoseWithCovarianceStamped

if __name__ == '__main__':


    rospy.init_node('publish_initial_pose')
    pub_pose = rospy.Publisher('/initialpose', PoseWithCovarianceStamped, queue_size=1)

    # 转换为pose
    # parser = argparse.ArgumentParser()
    # parser.add_argument('x', type=float)
    # parser.add_argument('y', type=float)
    # parser.add_argument('z', type=float)
    # parser.add_argument('yaw', type=float)
    # parser.add_argument('pitch', type=float)
    # parser.add_argument('roll', type=float)
    # args = parser.parse_args()
    # quat = tf.transformations.quaternion_from_euler(args.roll, args.pitch, args.yaw)
    # xyz = [args.x, args.y, args.z]
    # initial_pose = PoseWithCovarianceStamped()
    # initial_pose.pose.pose = Pose(Point(*xyz), Quaternion(*quat))
    # initial_pose.header.stamp = rospy.Time().now()
    # initial_pose.header.frame_id = 'map_3d'

    initial_pose = [24, 7, 0, 0, 0, 0.707, 0.70]
    print(initial_pose)
    count = 0

    # quat_zeros = tf.transformations.quaternion_from_euler(0.0, 0.0, 0.0)
    xyz_zeros = initial_pose[:3]
    quat_zeros = initial_pose[3:]

    initial_pose_zeros = PoseWithCovarianceStamped()
    initial_pose_zeros.pose.pose = Pose(Point(*xyz_zeros), Quaternion(*quat_zeros))
    initial_pose_zeros.header.stamp = rospy.Time().now()
    initial_pose_zeros.header.frame_id = 'map'

    point_preTransform = PoseStamped()
    point_preTransform.pose = initial_pose_zeros.pose.pose
    point_preTransform.header = initial_pose_zeros.header

    tf_listener = tf.TransformListener()
    retry = 0
    while not rospy.is_shutdown():
        try:
            initial_pose_zeros.header.stamp = rospy.Time().now()

            tf_listener.waitForTransform('map_3d', initial_pose_zeros.header.frame_id, initial_pose_zeros.header.stamp,
                                         rospy.Duration(3))
            tf_listener.lookupTransform('map_3d', initial_pose_zeros.header.frame_id, initial_pose_zeros.header.stamp)
            transformed_pose = tf_listener.transformPose('map_3d', point_preTransform)
            break
        except tf.Exception as e:
            retry += 1
            if retry > 5:
                rospy.logerr("Failed to lookup transform from map to map_3d")
            # print(e)
            time.sleep(0.2)

    initial_pose_zeros.header = transformed_pose.header
    initial_pose_zeros.pose.pose = transformed_pose.pose
    initial_pose = initial_pose_zeros
    for i in range(20):
        rospy.sleep(1)
        pub_pose.publish(initial_pose)
        if count % 5 == 0:
            rospy.loginfo('Initial Pose published')
        count = (count + 1) % 5
