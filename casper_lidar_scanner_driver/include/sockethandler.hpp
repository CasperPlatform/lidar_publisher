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
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <array>
#include <iostream>
#include <stdio.h>
#include <string>

#define UDP_IN_BUF 20
//#include <driveserver.hpp>


typedef boost::shared_ptr<lidarserver> lidar_server_ptr;

using boost::asio::ip::udp;

class sockethandler{

    private:
        boost::asio::ip::udp::socket socket;
        udp::endpoint remote_endpoint;
        boost::asio::io_service io_service;
        //boost::asio::io_service io_service;
        lidar_server_ptr lidar_server;
        std::array<char,UDP_IN_BUF> read_buf;
        
        void start_receive();
        void handle_receive(const boost::system::error_code& error,
        std::size_t bytes_transferred);
    public:
        //sockethandler(void);
        sockethandler(lidarserver* server, int port);
        ~sockethandler();
        void start();
        
    };


#endif //SOCKETHANDLER_SOCKETHANDLER_H
