#include <lidarScanner.hpp>
#include <ros/console.h>

	lidarScanner::lidarScanner(const std::string& port,
							   uint32_t baud_rate, 
							   boost::asio::io_service& io)
							   : port(port), 
							   baud_rate(baud_rate), 
							   serial(io,port), 
							   shutting_down(false)
	{
		printf("in lidarScanne Constructor\n");
		//serial.set_option(boost::asio::serial_port_base::baud_rate(baud_rate));
		this->serialHandler = serial_handler_ptr(new serialhandler(this,"/dev/ttyUSB0",115200));
    	//this->serialHandler->start("/dev/ttyUSB0",115200);
	}	
						   
	lidarScanner::~lidarScanner(){}
	void lidarScanner::startLidar(){

		// char buf[]{'L','S'};
		char * buf = "LS";
		
		this->serialHandler->write_bytes(buf,2);
		printf("sent LS to lidar\n");
		this->serialHandler->start_read();
	}
	
	void lidarScanner::poll(sensor_msgs::LaserScan::Ptr scan) {
		printf("in poll method\n");
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

		while (!this->shutting_down && !scan_ready) 
		{	

			this->serialHandler->sync_read();
			//printf("in poll method while loop\n");
		    //ROS_INFO( "Read Point: %d, %d, %f, %d" , scan_position, distance, degreesPerSecond, scan_time_ms );
			if(scanRecieved)
			{	
				printf("scan received\n");
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
					printf("scan ready\n");
				}
				
				scanRecieved = false;
			}
		} 
	}
	
	void lidarScanner::updateScan(std::string scanMessage)
	{	
		printf("got data from serial\n");
		std::istringstream response_stream(scanMessage);
		
		response_stream >> scan_position;
		response_stream >> dummy;
		response_stream >> distance;
		response_stream >> dummy;
		response_stream >> degreesPerSecond;
		response_stream >> dummy;
		response_stream >> scan_time_ms;
		
		printf("scan_position: %d\n",scan_position);
		printf("distance: %d\n", distance);
		printf("degrees/s: %f\n", degreesPerSecond);
		printf("scan_time_ms: %d\n", scan_time_ms);


		scanRecieved = true;
	}
