/**
*	Casper SocketHandler Header 
*	Author @Pontus Pohl and @Linus Eiderström Swahn
*/

#ifndef SOCKETHANDLER_SOCKETHANDLER_H
#define SOCKETHANDLER_SOCKETHANDLER_H



class lidarserver;
#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>
#include <boost/system/error_code.hpp>
#include <boost/system/system_error.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>

#define UDP_IN_BUF 20
//#include <driveserver.hpp>


typedef boost::shared_ptr<lidarserver> lidar_server_ptr;

using boost::asio::ip::udp;

class SocketHandler {
private:
    udp::socket socket;
    udp::endpoint remote_endpoint;
    boost::asio::io_service io_service;
    //boost::asio::io_service io_service;
    lidarserver_server_ptr lidarserver;
    std::array<char,UDP_IN_BUF> read_buf;
    void startreceive();
    //struct addrinfo hints, *result, *p;
    //int status;
    //char ipString[INET6_ADDRSTRLEN];  // will point to the results
    //void getAddress(char* address, char* port);
    //void *get_in_addr(struct sockaddr *sa);
public:
    SocketHandler();
    ~SocketHandler();
    SocketHandler(lidarserver * server);
    //int setServer(driveserver & server) const;
    

    void start(char *address,int port);
    void startServer(char *address, char *port);
    void listenForMessages();
    void showAddress(char* address);
};


#endif //SOCKETHANDLER_SOCKETHANDLER_H
