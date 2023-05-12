import rospy
from geometry_msgs.msg import Twist
import keyboard


def move_robot():
    # 初始化节点
    rospy.init_node("move_robot_node", anonymous=True)
    # 创建/cmd_vel消息发布者
    pub = rospy.Publisher("/cmd_vel", Twist, queue_size=10)
    # 创建/cmd_vel消息
    msg = Twist()
    # 设置初始线速度和角速度为0
    msg.linear.x = 0.0
    msg.angular.z = 0.0

    while not rospy.is_shutdown():
        if keyboard.is_pressed("up"):
            msg.linear.x = 1  # 前进，线速度设置为0.2m/s
        elif keyboard.is_pressed("down"):
            msg.linear.x = -1  # 后退，线速度设置为-0.2m/s
        else:
            msg.linear.x = 0.0  # 停止，线速度设置为0

        if keyboard.is_pressed("left"):
            msg.angular.z = 0.5  # 左转，角速度设置为0.5rad/s
        elif keyboard.is_pressed("right"):
            msg.angular.z = -5  # 右转，角速度设置为-0.5rad/s
        else:
            msg.angular.z = 0.0  # 停止转弯，角速度设置为0

        # 发送/cmd_vel消息
        pub.publish(msg)

        # 等待0.1秒，降低CPU占用率
        rospy.sleep(0.1)


if __name__ == '__main__':
    try:
        move_robot()
    except rospy.ROSInterruptException:
        pass