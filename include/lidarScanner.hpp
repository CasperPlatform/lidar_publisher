/**
*	Casper drive_server Header 
*	Author @Pontus Pohl and @Linus Eiderström Swahn
*/

#ifndef lidarScanner_hpp
#define lidarScanner_hpp

#include <sensor_msgs/LaserScan.h>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <string>

namespace lidar_scanner_driver{
	class lidarScanner
	{
	public:
		uint16_t rpms;
		lidarScanner(const std::string& port, uint32_t baud_rate, boost::asio::io_service& io);
		~lidarScanner();
		void poll(sensor_msgs::LaserScan::Ptr scan);
		void close(){ shutting_down = true; };
	private:
	    std::string               port;
	    uint32_t                  baud_rate;
	    bool                      shutting_down;
	    boost::asio::serial_port  serial;
	};
}

#endif // lidarServer.hpp