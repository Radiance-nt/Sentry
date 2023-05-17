

### 运行机器人程序 (on NUC)

#### 1 启动实体雷达和底盘（同时启动雷达tf）
./lidar.sh
./base.sh

#### 1* 启动仿真（自带雷达数据和雷达tf）
./sim.sh

#### 2 建图
记住**初始点**
./get_map.sh

#### 3 保存地图
机器人走便地图后ctrl+c关闭get_map程序，自动保存3维地图pcd文件
./save_map.sh
save_map将刚刚保存的3维地图转化为2维地图, ctrl+c关闭save_map程序

#### 4 slam
保持 lidar.sh 和 base.sh
将车开回**初始点**
./slam.sh
./nav.sh

    *实体机上的1-4可以直接使用./robotTest.sh替代*

此时可以通过pc上的 rviz.launch看到机器人状态

#### 4* 地图对齐（非运行必须）
根据pc上rviz的可视化修正map2d(frame: map)到map3d(frame: map_3d)的tf
rosparam set /map_tf/...


#### 5 执行导航
##### 5.1 开启策略，此时会自动导航，允许发布自瞄数据
./strategy.sh

##### 5.2 在rviz上设置goal
运行
rosparam set /RobotControl "true"
运行rviz，发布goal



### 设置ros远程连接 (on PC)
#### 添加hosts
打开/etc/hosts，添加下面这一行
192.168.2.100	xjturm-NUC

#### 连接 xjturm-NUC，设置wifi ipv4属性
地址  掩码  网关
192.168.2.103  255.255.255.0   192.168.2.1

#### PC远程连接及启动Rviz

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
