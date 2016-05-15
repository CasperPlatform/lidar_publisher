// Include the ROS C++ APIs
#include <ros/ros.h>
#include <boost/asio.hpp>
// Standard C++ entry point
int main(int argc, char** argv) {

 boost::asio::io_service io_service;
 // Announce this program to the ROS master as a "node" called "hello_world_node"
 ros::init(argc, argv, "hello_world_node");
 // Start the node resource managers (communication, time, etc)
 ros::start();
 // Broadcast a simple log message
 ROS_INFO_STREAM("Hello, world!");
 // Process ROS callbacks until receiving a SIGINT (ctrl-c)
 ros::spin();
 // Stop the node's resources
 ros::shutdown();
 // Exit tranquilly
 return 0;
}