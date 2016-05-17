/**
*	Casper serialHandler Implementation 
*	Author @Pontus Pohl and @Linus Eiderström Swahn
*/

#include "serialhandler.hpp"


serialhandler::serialhandler(void) : end_of_line_char('\n')
{
    
}
serialhandler::~serialhandler(void)
{
    stop();
}
char serialhandler::get_eol_char() const
{
    return this->end_of_line_char;
}
void serialhandler::set_eol_char(const char & c){
    this->end_of_line_char = c;
}
bool serialhandler::start(const char * com_port_name, int baud_rate){
    boost::system::error_code ec;
    printf("35 in start()\n");
    if(port)
    {
        std::cout << "Port allready open." << std::endl;

        return false;
    }
    
    port = serial_port_ptr(new boost::asio::serial_port(io_service));
    
    port->open(com_port_name, ec);

    if(ec)
    {
        std::cout << "Open Failed. com port name = " << com_port_name << ", e= " << ec.message().c_str() << std::endl;

        return false;
    }

    port->set_option(boost::asio::serial_port_base::baud_rate(baud_rate));

    boost::thread t(boost::bind(&boost::asio::io_service::run, &io_service));

    async_read();

    return true;
}

void serialhandler::stop()
{
    boost::mutex::scoped_lock look(mutex);

    if(port)
    {
        port->cancel();
        port->close();
        port.reset();
    }

    io_service.stop();
    io_service.reset();
}

int serialhandler::write_string(const std::string &buf)
{
    return write_bytes(buf.c_str(), buf.size());
}

int serialhandler::write_bytes(const char *buf, const int &size)
{
    boost::system::error_code ec;

    if(!port) return -1;
    if(size==0) return 0;

    return port->write_some(boost::asio::buffer(buf, size), ec);
}

void serialhandler::async_read()
{
    if( port.get() == NULL || !port->is_open()){
        return;
    } 
    
    port->async_read_some(
        boost::asio::buffer(read_buf_raw, SERIAL_PORT_READ_BUF_SIZE), 
        boost::bind(
            &serialhandler::on_receive,
            this, boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
}
void serialhandler::on_receive(const boost::system::error_code& ec, size_t bytes_transferred)
{
    boost::mutex::scoped_lock look(mutex);
    
    if( port.get() == NULL || !port->is_open())
    {
        return;
    }
    if(ec) {
        async_read();
        return;
    }    
    for(unsigned int i = 0; i < bytes_transferred; i++){
        char c = read_buf_raw[i];
        if(c == end_of_line_char) {
            this->on_receive(read_buf_str);
            read_buf_str.clear();
        }
        else
        {
            read_buf_str += c;
        }
    }
}
void serialhandler::on_receive(const std::string & data){
    std::cout << "serialhandler::on_receive() : " << data << std::endl;
}