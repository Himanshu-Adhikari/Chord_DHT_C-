#ifndef socket_port_h
#define socket_port_h
#include <iostream>
#include <netinet/in.h>
using namespace std;

class socket_and_port
{
private:
    int serving_port;
    int serving_socket;
    struct sockaddr_in current;

public:
    void specifyPortServer();

    void Change_port_no_to(int new_port);
    bool IF_CURRENT_PORT_IN_USE(int port_no);
    string GET_IP();
    int GET_PORT();
    int getSocketFd();
    void closeSocket();
};

#endif