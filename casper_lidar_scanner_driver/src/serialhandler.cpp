/**
*	Casper serialHandler Implementation 
*	Author @Pontus Pohl and @Linus Eiderström Swahn
*/

#include "serialhandler.hpp"
#include "lidarScanner.hpp"

serialhandler::serialhandler(lidarScanner* scanner) : end_of_line_char('\n')
{   
    printf("serialHandler constructor\n");
    this->lidar_scanner = lidarScannerPointer(scanner);
}
serialhandler::~serialhandler(void)
{
    stop();
}

void serialHandler::initialize(){
    
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
    io_service.run();
    return true;
}
void serialhandler::start_read(){
    printf("calling async_read()\n");
    
   

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

int serialhandler::write_bytes(const char * buf, const int &size)
{
    boost::system::error_code ec;
    // port.set_option( asio_serial::baud_rate( 115200) );
    // port.set_option( asio_serial::flow_control( asio_serial::flow_control::none ) );
    // port.set_option( asio_serial::parity( asio_serial::parity::none ) );
    // port.set_option( asio_serial::stop_bits( asio_serial::stop_bits::one ) );
    // port.set_option( asio_serial::character_size( 8 ) );
    
    if(!port){
        printf("port not open\n");
        return -1;

    } 
    if(size==0) return 0;

    
    int result = port->write_some(boost::asio::buffer(buf, size), ec);
    
    if(ec)
    {
        printf("%s\n", ec.message().c_str());
    }
    else 
    {
        return result;
    }
   
}

void serialhandler::async_read()
{
    if( port.get() == NULL || !port->is_open()){
        printf("async read error in async_read()\n");
        return;
    } 
    printf("in async_read()\n");
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
    printf("on recieve()\n");
    if( port.get() == NULL || !port->is_open())
    {
        printf("error in on_receive\n");
        return;
    }
    if(ec) {
        printf("ec in on_receive\n");
        async_read();
        return;
    }    
    for(unsigned int i = 0; i < bytes_transferred; i++){
        char c = read_buf_raw[i];
        printf("%c",c);
        if(c == end_of_line_char && c == '\r') {
            printf("got end of line\n");
            this->on_received(read_buf_str);
            read_buf_str.clear();
            async_read();
        }
        else
        {
            read_buf_str += c;
        }
    }
}
void serialhandler::on_received(const std::string & data){
    printf("updatescan in serialhandler\n");
    this->lidar_scanner->updateScan(data);
}