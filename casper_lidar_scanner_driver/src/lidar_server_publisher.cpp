// Include the ROS C++ APIs
#include <lidarserver.hpp>

// Standard C++ entry point
int main(int argc, char** argv) {

lidarserver lidar("/dev/ttyUSB0",115200, argc, argv);
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