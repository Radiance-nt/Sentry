#!/usr/bin/python
# coding=utf-8

import itertools
import threading
import time
from typing import List

import rospy
import actionlib
from move_base_msgs.msg import MoveBaseAction, MoveBaseGoal
from actionlib_msgs.msg import GoalStatus
from geometry_msgs.msg import Pose, Point, Quaternion, PoseStamped
from config.nav_points import points
from config.state import State

"""
TODO: 改为系数乘距离
"""

WAIT_TIME = 30

"""
TODO: 在send_control节点中，
"""
ok_ = False


class RobotController:
    def __init__(self, points):
        rospy.init_node('robot_controller', anonymous=True)

        self.client = actionlib.SimpleActionClient('move_base', MoveBaseAction)
        self.client.wait_for_server()

        self.state = State.WAIT
        self.goals = points
        self.goals_iter = itertools.cycle(self.goals)
        self.current_goal = None
        self.seq_ = 1
        self.seq_last_ = 1

        # 状态跳转函数表
        self.jump = {}
        self.jump.setdefault(lambda _: None)
        init_jump_table(self.jump)

        # 通信变量
        self.allow_aim_ = False
        self.allow_nav_ = False
        self.rotate_ = False
        self.allow_hit_back_ = True
        self.health_ = None
        self.remainTime_ = None
        self.bulletNum_ = None

        # 通信线程
        self.worker_thread = threading.Thread(target=self.daemon_)
        self.worker_thread.daemon = True
        self.worker_thread.start()

        while not rospy.is_shutdown() and not ok_:
            rospy.loginfo("[Init] Not ok")
            time.sleep(2)

    def run(self):
        while not rospy.is_shutdown():
            if self.state == State.WAIT:
                self.allow_nav_ = False
                if isinstance(self.remainTime_, int) and self.remainTime_ < 600:
                    # 新建导航点
                    self.current_goal = next(self.goals_iter)
                    self.changeState(State.NAV_FORWARD)
            elif self.state == State.NAV_FORWARD or self.state == State.NAV_HOME:  # 导航或回家
                self.run_nav()
            elif self.state == State.OUT_OF_BULLET:
                self.run_out_of_bullet()

            self.checkInterrupt()

    def changeState(self, state):
        # 取消所有目标
        self.client.cancel_all_goals()
        self.seq_ += 1
        # 调用跳转表函数，传入self指针
        if (self.state, state) in self.jump.keys():
            self.jump[(self.state, state)](self)
        # 切换状态
        self.state = state

    """
    导航与回家
    """

    def run_nav(self):
        self.allow_nav_ = True
        self.allow_aim_ = True if self.state == State.NAV_FORWARD else False
        self.allow_hit_back_ = True if self.health_ > 400 else False

        '''
        // 1 处
         中断返回若状态切换导致self.seq_ > self.seq_last_后
            重新执行到这里，还是发送旧导航点，
        如果导航结束或超时，self.changeState(self.state)
            导致self.seq_ > self.seq_last，新导航点后在此处发布
        '''
        if self.seq_ > self.seq_last_:
            # 发送导航点
            self.seq_last_ = self.seq_
            rospy.loginfo("{%s} Send Goal...", self.state)
            # print(self.current_goal)
            self.send_goal_(self.current_goal)
            return

        start_time = rospy.Time.now()
        while not rospy.is_shutdown():
            self.checkInterrupt()
            # //1 中断发生
            if self.seq_ > self.seq_last_:
                return

            # 导航结束
            if self.client.get_state() in [GoalStatus.SUCCEEDED, GoalStatus.ABORTED]:
                rospy.loginfo("{%s} Nav Done", self.state)
                self.seq_ += 1
                # 新建导航点
                self.current_goal = next(self.goals_iter)
                self.changeState(self.state)
            # 导航超时
            elif (rospy.Time.now() - start_time).to_sec() > WAIT_TIME:  # 如果导航时间超过WAIT_TIME秒
                self.client.cancel_goal()  # 取消导航
                rospy.loginfo("{%s} Time out", self.state)
                # 新建导航点
                self.current_goal = next(self.goals_iter)
                self.changeState(self.state)
                return
            # 仍在导航中
            time.sleep(0.02)

    """
    没子弹，走到角落防止被打到
    """

    def run_out_of_bullet(self):
        while True:
            pass

    # 检查中断，跳转状态或其他处理
    def checkInterrupt(self):
        if self.health_ < 300 and self.state != State.NAV_HOME:
            self.changeState(State.NAV_HOME)
            rospy.loginfo("[Interrupt] Switch to HOME")
            # TODO: 更改巡航点为Home
            return

        if self.bulletNum_ <= 3 and self.state != State.OUT_OF_BULLET:
            self.changeState(State.OUT_OF_BULLET)
            rospy.loginfo("[Interrupt] Out of Bullet!!")
            return

        if self.state == State.NAV_HOME and self.health_ > 500:
            self.changeState(State.NAV_FORWARD)
            rospy.loginfo("[Interrupt] Switch to NAV_FORWARD")
            # TODO: 更改巡航点为Attack
            return

    def daemon_(self):
        global ok_
        while not rospy.is_shutdown():
            # 上位机控制线程
            setparam("RobotControl/allow_aim", self.allow_aim_)
            setparam("RobotControl/allow_nav", self.allow_nav_)
            # 给电控发的 允许小陀螺 和 允许反击
            setparam("RobotControl/allow_rotate", self.rotate_)
            setparam("RobotControl/allow_hitBack", self.allow_hit_back_)
            ok_ = getparam("RobotStatus/ok", default=None)
            self.health_ = getparam("RobotStatus/health", default=600)
            self.remainTime_ = getparam("RobotStatus/remainTime", default=600)
            self.bulletNum_ = getparam("RobotControl/bulletNum", default=700)

    def send_goal_(self, pose):
        goal = MoveBaseGoal()
        goal.target_pose.pose = pose
        goal.target_pose.header.frame_id = 'map'
        goal.target_pose.header.stamp = rospy.Time.now() - rospy.Duration(0.2)
        self.client.send_goal(goal)


def getparam(param, **kwargs):
    return rospy.get_param(param, **kwargs)


def setparam(param, value):
    rospy.set_param(param, value)


def init_jump_table(jump_table):
    def WAIT2NAV_FORWARD(self):
        rospy.loginfo("[Jump] From WAIT Switch to NAV_FORWARD")

    jump_table[(State.WAIT, State.NAV_FORWARD)] = WAIT2NAV_FORWARD


if __name__ == '__main__':
    points = [Pose(Point(a, b, c), Quaternion(d, e, f, g)) for (a, b, c, d, e, f, g) in points]
    controller = RobotController(points)
    controller.run()
