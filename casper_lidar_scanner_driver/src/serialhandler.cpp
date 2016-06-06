/**
*	Casper serialHandler Implementation 
*	Author @Pontus Pohl and @Linus Eiderström Swahn
*/

#include "serialhandler.hpp"
#include "lidarScanner.hpp"

serialhandler::serialhandler(lidarScanner* scanner, 
                            const char * com_port, 
                            int baud) : 
end_of_line_char('\n'), 
com_port_name(com_port),
baud_rate(baud),
service_thread(std::bind(&serialhandler::initialize, this))
{   

    printf("serialHandler constructor\n");
    this->lidar_scanner = lidarScannerPointer(scanner);
}
serialhandler::~serialhandler(void)
{
    stop();
}

void serialhandler::initialize(){
    this->start(this->com_port_name,this->baud_rate);
    // async_read();
    // io_service.run();
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
    port->set_option( boost::asio::serial_port_base::flow_control( boost::asio::serial_port_base::flow_control::none ) );
    port->set_option( boost::asio::serial_port_base::parity( boost::asio::serial_port_base::parity::none ) );
    port->set_option( boost::asio::serial_port_base::stop_bits( boost::asio::serial_port_base::stop_bits::one ) );
    // port.set_option( asio_serial::character_size( 8 ) );

    return true;


}
void serialhandler::start_read(){
    //printf("calling async_read()\n");
    
   

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

void serialhandler::sync_read(){

    uint8_t start_char;
    uint8_t temp_char;

    double angle;
    uint16_t distance;
    float degrees;
    double scan_time;
    int count = 0;
    boost::asio::read(*port, boost::asio::buffer(&start_char,1));
    
    if(start_char == 0x10){
        count++;
        boost::asio::read(*port, boost::asio::buffer(&start_char,1));
        if(start_char == 0x2D){
            count++;
            boost::asio::read(*port, boost::asio::buffer(&start_char,1));
            if(start_char == 0x70){
                count++;
            }
        }
    }
    if( count == 3){    
        count = 0;
        printf("found start char\n");
        // int count = 0;

        // do {
        //     boost::asio::read(*port, boost::asio::buffer(&temp_char,1));
        //     printf("char : %d\n",(uint8_t)temp_char);
        //     count++;
        // } while(temp_char != 0x10);

        // printf("count is: %d\n",count);

        boost::asio::read(*port, boost::asio::buffer(&angle,8));
        boost::asio::read(*port, boost::asio::buffer(&temp_char,1));
        boost::asio::read(*port, boost::asio::buffer(&distance,2));
        boost::asio::read(*port, boost::asio::buffer(&temp_char,1));        
        boost::asio::read(*port, boost::asio::buffer(&degrees,4));
        boost::asio::read(*port, boost::asio::buffer(&temp_char,1));
        boost::asio::read(*port, boost::asio::buffer(&scan_time,8));
        boost::asio::read(*port, boost::asio::buffer(&temp_char,1));
        0x0A, 0x2D, 0x70
        if(temp_char == '\n'){
            printf("got complete message\n");
            
            printf("angle: %f\n",angle);
            printf("distance: %d\n", distance);
            printf("degrees/s: %f\n", degrees);
            printf("scan_time_ms: %f\n", scan_time);
        }
        else{
            printf("wrong end char\n");
        }
    }
    else{
        printf("got wrong start sequence\n");
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
        //printf("on recieve value: %d\n",i);
        //printf("%c\n",c);
        
        if(c == end_of_line_char) {
            printf("got end of line\n");

            this->on_received(read_buf_str);
            read_buf_str.clear();
            async_read();
            return;
        }
        else
        {
            read_buf_str += c;
        }
    }
    async_read();
}
void serialhandler::on_received(const std::string & data){
    printf("updatescan in serialhandler\n");
    printf("data from serial: %s\n", data.c_str());
    this->lidar_scanner->updateScan(data);
}