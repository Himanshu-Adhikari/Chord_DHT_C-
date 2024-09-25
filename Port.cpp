#include<iostream>
#include "all_header.h"
#include "PortHeader.h"

//Generate a port for the current node to work on
void Socket_and_Port::specifyPortServer(){
    //To get a random number btween 2^10 and 2^16-1
    srand(time(0));
    portNoServer=rand()%65536;
    if(portNoServer < 1024)portNoServer+=1024;

    socklen_t len=sizeof(current);


    //creates a new socket (domain,type,protocol) 
    //AF_INET ->Address Family Internet ,  SOCK_DGRAM-->Datagram Socket(connectionless communication commonly used for UDP)
    //0-> default protocol in this case UDP bcz of SOCK_DGRAM
    curr_socket=socket(AF_INET, SOCK_DGRAM, 0);

}


//Get the IP  the current node 
string Socket_and_Port::getIpAddress(){
    string ip=inet_ntoa(current.sin_addr);
    return ip;
}

