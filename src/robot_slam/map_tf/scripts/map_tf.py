#!/usr/bin/env python

import rospy
import tf
from tf.transformations import quaternion_from_euler

def main():
    rospy.init_node('dynamic_tf_broadcaster')

    br = tf.TransformBroadcaster()
    rate = rospy.Rate(1000.0)

    while not rospy.is_shutdown():
        # Get current configuration values from the parameter server
        x = rospy.get_param('~x', 0)
        y = rospy.get_param('~y', 0)
        z = rospy.get_param('~z', 0)
        roll = rospy.get_param('~roll', 0)
        pitch = rospy.get_param('~pitch', 0)
        yaw = rospy.get_param('~yaw', 0)

        # Broadcast transform with current values
        br.sendTransform((x, y, z),
                         quaternion_from_euler(roll, pitch, yaw),
                         rospy.Time.now(),
                         "map_3d",
                         "map")

        rate.sleep()

if __name__ == "__main__":
    main()