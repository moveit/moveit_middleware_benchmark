echo "The configurations for rmw_zenoh_cpp is started!"
export RMW_IMPLEMENTATION=rmw_zenoh_cpp
sudo sysctl -w "net.ipv4.tcp_rmem=102400 102400 102400"
sudo sysctl -w "net.ipv4.tcp_wmem=102400 102400 102400"
sudo sysctl -w "net.ipv4.tcp_mem=102400 102400 102400"

sudo sysctl -w net.ipv4.udp_mem="102400 873800 16777216"
sudo sysctl -w net.core.netdev_max_backlog="30000"
sudo sysctl -w net.core.rmem_max="20971520"
sudo sysctl -w net.core.wmem_max="20971520"
sudo sysctl -w net.core.rmem_default="20971520"
sudo sysctl -w net.core.wmem_default="20971520"
echo "The configurations for rmw_zenoh_cpp is finished!"
