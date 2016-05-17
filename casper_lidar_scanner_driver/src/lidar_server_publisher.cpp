// Include the ROS C++ APIs
#include <lidarserver.hpp>
#include <stdio.h>
// Standard C++ entry point
int main(int argc, char** argv) {

printf("starting lidarserver\n");
lidarserver lidar("/dev/ttyAMA0",115200, argc, argv);
lidar.start();
 
 // Start the node resource managers (communication, time, etc)
 //ros::start();
 // Broadcast a simple log message
 
 // Process ROS callbacks until receiving a SIGINT (ctrl-c)
 //ros::spin();
 // Stop the node's resources
 //ros::shutdown();
 // Exit tranquilly
 return 0;
}