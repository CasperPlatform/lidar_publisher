// Include the ROS C++ APIs
#include <ros/ros.h>
#include <boost/asio.hpp>
#include <sensor_msgs/LaserScan.h>
#include <std_msgs/UInt16.h>
#include <lidarScanner.hpp>
#include <sqlite3.h>
// Standard C++ entry point
int main(int argc, char** argv) {

ros::init(argc, argv, "lidar_scanner_publisher");
ros::NodeHandle nh;
ros::NodeHandle priv_nh("~");
 
 // Start the node resource managers (communication, time, etc)
 //ros::start();
 // Broadcast a simple log message
 ROS_INFO_STREAM("Lidar Scanner Publisher");
 // Process ROS callbacks until receiving a SIGINT (ctrl-c)
 ros::spin();
 // Stop the node's resources
 ros::shutdown();
 // Exit tranquilly
 return 0;
}