#include <lidarScanner.hpp>
#include <ros/console.h>
namespace lidar_scanner_driver
{
	lidarScanner::lidarScanner(const std::string& port,
							   uint32_t baud_rate, 
							   boost::asio::io_service& io)
							   : port(port), 
							   baud_rate(baud_rate), 
							   serial(io,port), 
							   shutting_down(false)
	{
		serial.set_option(boost::asio::serial_port_base::baud_rate(baud_rate));
	}						   
	lidarScanner::~lidarScanner()
	void lidarScanner::poll(sensor_msgs::LaserScan::Ptr scan) {
		uint8_t points = 0;
		bool scan_ready = false;
		rpms = 0;

		scan->angle_min = 0.0;
		scan->angle_max = 2.0*M_PI;
		scan->angle_increment = (2.0*M_PI/360.0);
		scan->range_min = 0.01;
		scan->range_max = 6.0;
		scan->ranges.resize(360);
		scan->intensities.resize(360);

		int scan_position;
		int distance;
		double degreesPerSecond;
		int scan_time_ms;
		char dummy;

		while (!shutting_down_ && !scan_ready) 
		{
			boost::asio::streambuf response;

			boost::asio::read_until(serial_, response, "\r\n" );

		    std::istream response_stream(&response);
		    response_stream >> scan_position;
		    response_stream >> dummy;
		    response_stream >> distance;
		    response_stream >> dummy;
		    response_stream >> degreesPerSecond;
		    response_stream >> dummy;
		    response_stream >> scan_time_ms;

		    //ROS_INFO( "Read Point: %d, %d, %f, %d" , scan_position, distance, degreesPerSecond, scan_time_ms );

		    if ( scan_time_ms > 25 )
		    {
		    	ROS_WARN( "LIDAR-Lite sampling took %d milliseconds", scan_time_ms );
		    }

		    rpms = degreesPerSecond * 60.0 / 360.0;

			scan->ranges[scan_position] = distance / 100.0; //centimeter to meter conversion
			scan->intensities[scan_position] = distance;
			scan->time_increment = degreesPerSecond; //seconds between scan poins

			if ( ++points >= 5)
			{
				scan_ready = true;
			}
		} 
	}
};