/**
*   Casper drive_server Implementation
*   Author @Pontus Pohl and @Linus Eiderström Swahn
*/
#include <lidarserver.hpp>
#include <SocketHandler.hpp>
#include <serialhandler.hpp>
#include <lidarScanner.cpp>
lidarserver::lidarserver(const std::string port, int baud_rate){
    read_buf.fill(0);
    for(int i = 0; i>1; i++){
        lidarStart[i]   = 'L';
        lidarStart[i+1] = 'S';
        lidarStop[i]    = 'L';
        lidarStop[i+1]  = 's';
    }
    token = "";
    this->port = port;
    this->baud_rate = baud_rate;
    this->polling = false;
    this->serialHandler = serial_handler_ptr(new serialhandler());
    this->lidarScanner  = lidar_scanner_ptr(new lidarScanner(port,
                                                            baud_rate));
    this->socketHandler = socket_handler_ptr(new SocketHandler(this));

}
driveserver::~driveserver(){}

int lidarserver::start(){
    printf("driveserver start()\n");

    //this->socketHandler->setServer(*this);
    if(serialHandler->start("/dev/ttyACM0",9600) != false){
        printf("Serial started successfully\n");
    }
    this->socketHandler->start("0.0.0.0",9999);
    //this->socketHandler->startServer("0.0.0.0", "9999");
    return 0;
}
int lidarserver::startPolling(){
    
    if(serialHandler->write_bytes(this->lidarStart,2) == -1){
        printf("serialwrite failed\n");
        return -1;
    }
    
}
int lidarserver::parseRequest(std::array<char,20> buf, int len) {
    // parse message and verify token
    printf("got a buffer to send to arduino\n%s\n",buf);

    printf("length is %d\n",len);

    int cr = 0;
    int lf = 0;
    bool r  = false;
    // parse buffer
    for(int i = 0; i<len;i++){
        read_buf[i] = buf[i];
        if (buf[i] == CARRIAGE_RETURN){
            cr = buf[i];
            printf("Got Carriage-return\n");
        }
        if(buf[i] == LINE_FEED){
            lf = buf[i];
            printf("Got Line-Feed\n");
        }
        if(cr != 0 && lf != 0){
            if( buf[i] == END_OF_TRANSMISSION){
                printf("Got CL,RF,EOF\n");
                r = true;
                break;
            }
        }
        //printf("parsing nr %d: %d\n",i,buf[i]);
    }

    // check if message is error free.
    if(r && read_buf.size() == 24){
        printf("got a complete drive message\n");
    }
    else{
        printf("Message error found.\n");
        return -1;
    }
    // extract token.
    char tmp[17];
    for(int i = 1; i < 17; i++){
        tmp[i-1] = read_buf[i];
    }
    tmp[16] = '\0';
    printf("token is %s\n",tmp);
    // Do sqlite stuff if needed
    if(token == ""){
        if(verifyToken(tmp) == -1)
        {
            printf("token verification failed\n");
            return -1;
        }
        token = std::string(tmp);
    }
    // prepare for serial delivery
    char fmsg[8];
    fmsg[0] = read_buf[0];
    for(int i = 0; i < 7; i++){
        fmsg[i+1] = read_buf[i+17];
    }
    for(int i = 0; i < 8;i++){
        printf("serial msg nr %d: %d\n",i,fmsg[i]);
    }

    if(serialHandler->write_bytes(fmsg,8) == -1){
        printf("serialwrite failed\n");
        return -1;
    }
    printf("successfully wrote to serial\n");
    return 0;
}
int lidarserver::verifyToken(const char token[]) const{

    sqlite3 *sqlite_conn;
    sqlite3_stmt *stmt;
    char *query;



    if ((sqlite_conn = sqlite_open()) == NULL)
    {
        printf("Failed to open db\n");
        return -1;
    }


    time_t rawtime;
    time(&rawtime);
    struct tm *currentTime;
    currentTime = localtime ( &rawtime );
    const int TIME_STRING_LENGTH = 20;
    char buffer [TIME_STRING_LENGTH];

    strftime(buffer, TIME_STRING_LENGTH, "%Y-%m-%d %H:%M:%S", currentTime);

    asprintf(&query,
            "select userId from tokens where token=\"%s\" and expiration>\"%s\""
            ,token
            ,buffer);

    printf("query is : %s\n", query);
    //printf("%s\n",buffer);

    if (sqlite3_prepare_v2(sqlite_conn, query, strlen(query), &stmt, NULL) != SQLITE_OK)
    {
        free(query);
        return -1;
    }

    int uid;
    int c = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        c++;
        printf("got something back\n");
        uid = sqlite3_column_int(stmt,0);
        printf("uid is: %d\n", uid);
    }
    if(c == 0){
        printf("no token found\n");
        free(query);
        sqlite3_finalize(stmt);
        sqlite3_close(sqlite_conn);
        //free(currentTime);
        return -1;
    }
    else
    {
        //free(currentTime);

        time(&rawtime);
        currentTime = localtime ( &rawtime );

        for(int i = 0; i < 25; i++){
            if(currentTime->tm_min == 59){
                currentTime->tm_min = 0;
                currentTime->tm_hour += 1;
                continue;
            }
            currentTime->tm_min++;
        }
        strftime(buffer, TIME_STRING_LENGTH, "%Y-%m-%d %H:%M:%S", currentTime);

        asprintf(&query,"");

        asprintf(&query,
                "update tokens set expiration=\"%s\" where userId=%d",
                buffer,
                uid);
        printf("query2 is : %s\n", query);

        if (sqlite3_prepare_v2(sqlite_conn, query, strlen(query), &stmt, NULL) != SQLITE_OK)
        {
            free(query);
            sqlite3_finalize(stmt);
            sqlite3_close(sqlite_conn);
            return -1;
        }
        free(query);
        // sqlite3_bind_text(stmt,1,buffer,-1,SQLITE_TRANSIENT);
        // sqlite3_bind_int(stmt,2,uid);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        sqlite3_close(sqlite_conn);

    }

    // free(token);
    // free(tm);


    printf("statement ok\n");
    return 1;
}
sqlite3 * lidarserver::sqlite_open()
{
    sqlite3 *sqlite_conn;

    int error;
    if ((error = sqlite3_open("/home/pi/CASPER/db.db", &sqlite_conn)) != 0)
    {
        printf("ohoh %d\n", error);
        return NULL;
    }
    return sqlite_conn;
}