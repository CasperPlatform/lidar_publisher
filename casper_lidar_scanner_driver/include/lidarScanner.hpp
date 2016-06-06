/**
*	Casper drive_server Header 
*	Author @Pontus Pohl and @Linus Eiderström Swahn
*/

#ifndef lidarScanner_hpp
#define lidarScanner_hpp

#include <serialhandler.hpp>
#include <sensor_msgs/LaserScan.h>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <string>
#include <stdio.h>

typedef boost::shared_ptr<serialhandler> serial_handler_ptr;

class lidarScanner
{
	public:
		uint16_t rpms;
		lidarScanner(const std::string& port, uint32_t baud_rate, boost::asio::io_service& io);
		~lidarScanner();
		void poll(sensor_msgs::LaserScan::Ptr scan);
		void startLidar();
		void close(){ shutting_down = true; };
		void updateScan(std::string scanMessage);
		
	private:
	    std::string               port;
	    uint32_t                  baud_rate;
	    bool                      shutting_down;
	    boost::asio::serial_port  serial;
		serial_handler_ptr serialHandler;
		int scan_position;
		int distance;
		double degreesPerSecond;
		int scan_time_ms;
		char dummy;
		bool scanRecieved;
};


#endif // lidarScanner.hpp