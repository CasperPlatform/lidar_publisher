/**
*	Casper SerialHandler Header 
*	Author @Pontus Pohl and Linus Eiderström Swahn
*/

#ifndef serial_handler_h
#define serial_handler_h


//class driveserver;
//#include <driveserver.hpp>
#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>
#include <boost/system/error_code.hpp>
#include <boost/system/system_error.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>

typedef boost::shared_ptr<boost::asio::serial_port> serial_port_ptr;

#define SERIAL_PORT_READ_BUF_SIZE 256

class serialhandler
{
protected:
	boost::asio::io_service io_service;
	serial_port_ptr port;
	boost::mutex mutex;

	char read_buf_raw[SERIAL_PORT_READ_BUF_SIZE];
	std::string read_buf_str;

	char end_of_line_char;

/*private:
	SerialPort(const SerialPort &p);
	SerialPort &operator=(const SerialPort &p); 
*/
public:
	serialhandler(void);
	virtual ~serialhandler(void);

	char get_eol_char() const;
	void set_eol_char(const char &c);

	virtual bool start(const char *com_port_name, int baud_rate=9600);
	virtual void stop();

	int write_string(const std::string &buf);
	int write_bytes(const char *buf, const int &size);

	//static int get_port_number();
	//static std::string get_port_name(const unsigned int &idx);
	//static std::vector<std::string> get_port_names();
	//static void print_devices();

protected:
	virtual void async_read();
	virtual void on_receive(const boost::system::error_code& ec, size_t bytes_transferred);
	virtual void on_receive(const std::string &data);

};
#endif // serial_handler_h