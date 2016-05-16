/**
*	Casper SocketHandler Implementation 
*	Author @Pontus Pohl and @Linus Eiderström Swahn
*/



#include <SocketHandler.hpp>
#include <lidarserver.hpp>
SocketHandler::SocketHandler(){}
SocketHandler::~SocketHandler(){}
SocketHandler::SocketHandler(lidarserver * server)
{
    this->lidarserver = lidar_server_ptr(server);
}

// int SocketHandler::setServer(driveserver & server) const {
//     //this->driveServer = drive_server_ptr(server);
//     return 0;
// }
void SocketHandler::start_receive()
{
    socket.async_receive_from(
        boost::asio::buffer(read_buf), remote_endpoint,
        boost::bind
        (
            &lidarserver::handle_receive, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred
        )
    );
}
void handle_receive(const boost::system::error_code& error,
      std::size_t bytes_transferred)
  {
    // if no error
    if (!error || error == boost::asio::error::message_size)
    { 
      // check token 
      int res = this->lidarserver->parseRequest(read_buf,read_buf.size());
      if(res = -1){
         printf("token verification failed\n");
         start_receive();
         return;
      } 
      else if(res = 0) {
         this->lidarserver->startPolling();
      }
      else if(res = 1) {
         this->lidarserver->stopPolling();
      }
    start_receive();
    }
    else{
        printf("error in read\n");
    }
}
void SocketHandler::start(char * addr, int port){
    this->socket(this->io_service, udp::endpoint(udp::v4(), port));
    start_receive();
    this->io_service.run();
}






