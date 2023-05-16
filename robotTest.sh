#!/bin/bash

#if [ $# -eq 0 ]; then
#   echo "Usage: $0 <color>"
#   exit 1
#fi
#
#color=$1

time=$(date +%X)
# Define a function to kill all background processes
function kill_bg_processes() {
   echo "Killing all background processes..."
   kill $(jobs -p)
}

# Register the kill_bg_processes function to execute on SIGINT (Ctrl+C)
trap kill_bg_processes SIGINT

echo "Start lidar"

tmux new-session -s my-tmux-session1 -n lidar -d
tmux send-keys -t my-tmux-session1:lidar 'bash lidar.sh' C-m
sleep .5s

echo "Start base"
tmux new-session -s my-tmux-session2 -n base -d
tmux send-keys -t my-tmux-session2:base 'bash base.sh' C-m
sleep .5s

echo "Start slam"
tmux new-session -s my-tmux-session3 -n slam -d
tmux send-keys -t my-tmux-session3:slam 'bash slam.sh' C-m

sleep .5s

echo "Start autoaim"
tmux new-session -s my-tmux-session4 -n autoaim -d
tmux send-keys -t my-tmux-session4:autoaim 'bash autoaim.sh' C-m

sleep 5s

echo "Start nav"
tmux new-session -s my-tmux-session5 -n nav -d
tmux send-keys -t my-tmux-session5:nav 'bash nav.sh' C-m

# nohup bash lidar.sh   > /home/xjturm/scripts/log/lidar_out_${time}.log 2>&1 &

# nohup bash start_slam.sh > /home/xjturm/scripts/log/slam_out_${time}.log 2>&1 &

# nohup bash start_nav.sh > /home/xjturm/scripts/log/nav_out_${time}.log 2>&1 &

# nohup bash strategy.sh > /home/xjturm/scripts/log/strategy_out.log 2>&1 &

# 结束脚本
wait

