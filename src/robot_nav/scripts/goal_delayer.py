#!/usr/bin/python
# coding=utf-8

import rospy
from move_base_msgs.msg import MoveBaseActionGoal


def callback(data):
    # 修改时间戳，减2秒
    data.header.stamp = data.header.stamp - rospy.Duration(2)
    data.goal.target_pose.header.stamp = data.goal.target_pose.header.stamp - rospy.Duration(2)

    # 发布新的消息
    pub.publish(data)


def goal_changer():
    # 初始化节点
    rospy.init_node('goal_changer', anonymous=True)

    # 创建发布器
    global pub
    pub = rospy.Publisher('/move_base/goal', MoveBaseActionGoal, queue_size=10)

    # 创建订阅器，订阅"/move_base_simple/goal_future"的消息，收到消息后调用callback函数
    rospy.Subscriber("/move_base_simple/goal_future", MoveBaseActionGoal, callback)

    # 让节点继续运行直到节点停止
    rospy.spin()


if __name__ == '__main__':
    try:
        goal_changer()
    except rospy.ROSInterruptException:
        pass
