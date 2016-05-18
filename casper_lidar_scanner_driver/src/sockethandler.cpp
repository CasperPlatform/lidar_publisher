/**
*	Casper sockethandler Implementation 
*	Author @Pontus Pohl and @Linus Eiderström Swahn
*/

 

#include <sockethandler.hpp>
#include <lidarserver.hpp>

//sockethandler::~sockethandler(){}
sockethandler::sockethandler(lidarserver* server, int port) :
socket(io_service, udp::endpoint(udp::v4(), port ))
{ 
    printf("sockethandler constructor\n");
    this->lidar_server = lidar_server_ptr(server);
    printf("end of sockethandler constructor\n");
    //this->lidarserver = lidar_server_ptr(server);
}
sockethandler::~sockethandler(){
  this->io_service.stop();
}

// int sockethandler::setServer(driveserver & server) const {
//     //this->driveServer = drive_server_ptr(server);
//     return 0;
// }
void sockethandler::start_receive()
{
    socket.async_receive_from(
        boost::asio::buffer(read_buf), remote_endpoint,
        boost::bind
        (
            &sockethandler::handle_receive, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred
        )
    );
}
void sockethandler::handle_receive(const boost::system::error_code& error,
      std::size_t bytes_transferred)
  {
    // if no error
    if (!error || error == boost::asio::error::message_size)
    { 
      // check token 
      int res = this->lidar_server->parseRequest(read_buf,read_buf.size());
      if(res = -1){
         printf("token verification failed\n");
         start_receive();
         return;
      } 
      else if(res = 0) {
         this->lidar_server->startPolling();
      }
      else if(res = 1) {
         this->lidar_server->stopPolling();
      }
    start_receive();
    }
    else{
        printf("error in read\n");
    }
}
void sockethandler::start(){
    start_receive();
    this->io_service.run();
}






