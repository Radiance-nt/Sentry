#!/home/radiance/miniconda3/envs/ros/bin/python

import rospy
import tf
from geometry_msgs.msg import PoseWithCovarianceStamped
import time
from tf.transformations import euler_from_quaternion, quaternion_from_euler

# 定义两个全局变量，用于保存位置信息，并初始化为零位姿信息
pose1 = PoseWithCovarianceStamped()
pose1.pose.pose.position.x = 0
pose1.pose.pose.position.y = 0
pose1.pose.pose.position.z = 0
pose1.pose.pose.orientation.x = 0
pose1.pose.pose.orientation.y = 0
pose1.pose.pose.orientation.z = 0
pose1.pose.pose.orientation.w = 1

pose2 = PoseWithCovarianceStamped()
pose2.pose.pose.position.x = 0
pose2.pose.pose.position.y = 0
pose2.pose.pose.position.z = 0
pose2.pose.pose.orientation.x = 0
pose2.pose.pose.orientation.y = 0
pose2.pose.pose.orientation.z = 0
pose2.pose.pose.orientation.w = 1



def callback(msg, index):
    # 根据传入的index参数，使用全局变量pose1或pose2保存消息中的位姿信息
    global pose1, pose2
    if index == 1:
        pose1 = msg
    elif index == 0:
        pose2 = msg
    else:
        rospy.logerr("Invalid callback index: %d", index)


if __name__ == '__main__':
    rospy.init_node('map_tf')

    # 监听/map_convert/pose1和/map_convert/pose2话题
    # pose1 指向2d
    rospy.Subscriber("/map_tf/pose1", PoseWithCovarianceStamped, callback, callback_args=0)

    # pose1 指向3d
    rospy.Subscriber("/map_tf/pose2", PoseWithCovarianceStamped, callback, callback_args=1)


    # 发布TF
    br = tf.TransformBroadcaster()
    # rate = rospy.Rate(1)  # 控制发布频率为10Hz
    while not rospy.is_shutdown():
        # 将四元数转换为欧拉角，分别计算roll、pitch、yaw的差值
        (roll1, pitch1, yaw1) = euler_from_quaternion(
            [pose1.pose.pose.orientation.x, pose1.pose.pose.orientation.y, pose1.pose.pose.orientation.z, pose1.pose.pose.orientation.w])
        (roll2, pitch2, yaw2) = euler_from_quaternion(
            [pose2.pose.pose.orientation.x, pose2.pose.pose.orientation.y, pose2.pose.pose.orientation.z, pose2.pose.pose.orientation.w])
        roll_diff = roll1 - roll2
        pitch_diff = pitch1 - pitch2
        yaw_diff = yaw1 - yaw2
        # roll_diff = - roll_diff
        # pitch_diff = - pitch_diff
        # yaw_diff = -yaw_diff
        # print()
        # print((roll1, pitch1, yaw1))
        # print((roll2, pitch2, yaw2))
        # print((roll_diff, pitch_diff, yaw_diff))
        # print()

        # 将欧拉角差值转换为四元数
        quat_diff = quaternion_from_euler(roll_diff, pitch_diff, yaw_diff)

        # 计算相对位置pose的差异作为map_3d相对于map的变换
        translation = (pose1.pose.pose.position.x - pose2.pose.pose.position.x,
                       pose1.pose.pose.position.y - pose2.pose.pose.position.y,
                       pose1.pose.pose.position.z - pose2.pose.pose.position.z)
        rotation = quat_diff

        # 发布TF消息
        br.sendTransform(translation, rotation, rospy.Time.now() - rospy.Duration(0.2), "map", "map_3d")
        # print("translation publish")
        time.sleep(0.05)
