

## 设置ros远程连接 (PC上)
#### 添加hosts
打开/etc/hosts，添加下面这一行
192.168.2.100	xjturm-NUC

#### 连接 xjturm-NUC，设置wifi ipv4属性
地址  掩码  网关
192.168.2.103  255.255.255.0   192.168.2.1

#### 启动

export ROS_MASTER_URI="http://192.168.2.100:11311"
export ROST_HOSTNAME=$(whoami)
cd $(rospack find robot_nav)/launch
roslaunch rviz.launch

    连上wifi输入这两句之后，roscore便是NUC上的roscore，即rostopic等命令可以直接代指NUC上的topics
    export ROS_MASTER_URI="http://192.168.2.100:11311"
    export ROST_HOSTNAME=$(whoami)


    建议在`~/.bashrc`中添加`alias`
    alias remote_rviz="export ROS_MASTER_URI="http://192.168.2.100:11311"
    export ROST_HOSTNAME=$(whoami)
    cd $(rospack find robot_nav)/launch
    roslaunch rviz.launch"

## 运行机器人程序 (NUC上)

#### 启动雷达和底盘
./lidar.sh
./base.sh

#### 建图
记住**初始点**
./get_map.sh

#### 保存地图
机器人走便地图后ctrl+c关闭get_map程序，自动保存3维地图pcd文件
./save_map.sh
save_map将刚刚保存的3维地图转化为2维地图, ctrl+c关闭save_map程序

#### slam
保持 lidar.sh 和 base.sh
将车开回**初始点**
./slam.sh
./nav.sh

此时可以通过pc上的 rviz.launch看到机器人状态

#### 执行导航
##### 1 开启策略，此时会自动导航，允许发布自瞄数据
./strategy.sh

##### 2 在rviz上设置goal
运行
rosparam set /RobotControl "true"
运行rviz，发布goal