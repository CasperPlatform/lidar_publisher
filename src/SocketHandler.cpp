/**
*	Casper SocketHandler Implementation 
*	Author @Pontus Pohl and @Linus Eiderström Swahn
*/



#include <SocketHandler.hpp>
#include <driveserver.hpp>
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
void SocketHandler::start(char * addr, int port){
    this->socket(this->io_service, udp::endpoint(udp::v4(), port));
    start_receive();
}
void SocketHandler::start_receive()
{
    socket.async_receive_from(
        boost::asio::buffer(read_buf), remote_endpoint,
        boost::bind(&lidarserver::handle_receive, this,
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));
}
void handle_receive(const boost::system::error_code& error,
      std::size_t bytes_transferred)
  {
    // if no error
    if (!error || error == boost::asio::error::message_size)
    { 
      // check token 

      if(this->lidarserver->parseRequest(read_buf,read_buf.size()) == -1){
         printf("token verification failed\n");
         return;
      }  
      //#######
      // if ok  
      std::array<int,8000> msg;
      if(msg = this->lidarserver->getMapMsg() == -1){
        return;
      }

    socket_.async_send_to(boost::asio::buffer(msg), remote_endpoint,
          boost::bind(&udp_server::handle_send, this, message,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));

      start_receive();
    }
    else{
        pritnf("error in read\n");
    }
}


void SocketHandler::startServer(char *address, char *port)
{
    struct sockaddr_storage their_addr;
    socklen_t addr_size;
    int socketDescriptor, newSocketDescriptor;
    int numbytes;

    int MAXBUFLEN = 25;
    char buf[MAXBUFLEN];
    char s[INET6_ADDRSTRLEN];

    getAddress(address, port);

    socketDescriptor = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

    int bindResult = bind(socketDescriptor, result->ai_addr, result->ai_addrlen);

    if(bindResult == -1)
    {
        printf("error");
    }
    else if(bindResult == 0)
    {
        printf("Now listening on: %s", result->ai_addr);
        freeaddrinfo(result);
    }

    printf("listener: waiting to recvfrom...\n");

    while(1)
    {
        addr_size = sizeof their_addr;
        if ((numbytes = recvfrom(socketDescriptor, buf, MAXBUFLEN - 1, 0, (struct sockaddr *) &their_addr, &addr_size)) == -1) {
            perror("recvfrom");
            exit(1);
        }

        printf("listener: got packet from %s\n",
               inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *) &their_addr), s, sizeof s));
        printf("listener: packet is %d bytes long\n", numbytes);
        
     
        
        buf[numbytes] = '\0';
        
        for(int i = 0; i<24;i++){
            printf("packet nr :%d: %d\n",i,buf[i]);
        }
        
        if(driveServer){
            
            printf("Recieved buf with content\n\"%s\"\n", buf);
            printf("sending buf to driveserver\n");
            if(this->driveServer->parseAndSend(buf,numbytes) == -1){
                
            }
            
        }
        else{
            printf("driveServer is null, cannot send buf\n");
        }
     
        //printf("listener: packet contains \"%s\"\n", buf);
    }
}

void listenForMessages()
{

}

void *SocketHandler::get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void SocketHandler::getAddress(char *address, char *port)
{
    memset(&hints, 0, sizeof hints); // make sure the struct is empty
    hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
    hints.ai_socktype = SOCK_DGRAM; // UDP stream sockets
    hints.ai_flags = AI_PASSIVE;     // fill in my IP for me

    if ((status = getaddrinfo(address, port, &hints, &result)) != 0)
    {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }
}

void SocketHandler::showAddress(char *address)
{
    getAddress(address, NULL);

    printf("IP addresses for %s:\n\n", address);

    for(p = result; p != NULL; p = p->ai_next) {
        void *addr;
        char *ipVersion;

        // get the pointer to the address itself,
        // different fields in IPv4 and IPv6:
        if (p->ai_family == AF_INET) // IPv4
        {
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
            addr = &(ipv4->sin_addr);
            ipVersion = "IPv4";
        }
        else // IPv6
        {
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
            addr = &(ipv6->sin6_addr);
            ipVersion = "IPv6";
        }

        // convert the IP to a string and print it:
        inet_ntop(p->ai_family, addr, ipString, sizeof ipString);
        printf("  %s: %s\n", ipVersion, ipString);
    }

    freeaddrinfo(result); // free the linked list
}
