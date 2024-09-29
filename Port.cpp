#include <iostream>
#include "all_header.h"
#include "PortHeader.h"

// Generate a port for the current node to work on
void Socket_and_Port::specifyPortServer()
{
    // To get a random number btween 2^10 and 2^16-1
    srand(time(0));
    Portno_to_serve = rand() % 65536;
    if (Portno_to_serve < 1024)
        Portno_to_serve += 1024;

    socklen_t len = sizeof(current);

    // creates a new socket (domain,type,protocol)
    // AF_INET ->Address Family Internet ,  SOCK_DGRAM-->Datagram Socket(connectionless communication commonly used for UDP)
    // 0-> default protocol in this case UDP bcz of SOCK_DGRAM
    curr_socket = socket(AF_INET, SOCK_DGRAM, 0);
    current.sin_family = AF_INET;

    // Host TO Network Short ,host byte order to network byte order
    current.sin_port = htons(Portno_to_serve);

    // sets the IP address for the sockaddr_in
    current.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Try to bind the socket (curr_socket) to the specified address and port (stored in current)
    if (bind(curr_socket, (struct sockaddr *)&current, len) < 0)
    {

        // Cases like: If port already in use
        perror("Error ");
        exit(-1);
    }
}

// Chane Port
void Socket_and_Port::changePortNumber(int new_port_no)
{
    if (new_port_no < 1024 || new_port_no > 65535)
    {
        cout << "Not a valid port number.Enter Again. " << endl;
    }
    else
    {
        if (portInUse(new_port_no))
        {
            cout << "Sorry the port is already in use " << endl;
        }
        else
        {
            close(curr_socket);
            socklen_t len = sizeof(current);
            curr_socket = socket(AF_INET, SOCK_DGRAM, 0);
            current.sin_port = htons(new_port_no);
            if (bind(curr_socket, (struct sockaddr *)&current, len) < 0)
            {

                // Cases like: If port already in use
                perror("Error ");
                exit(-1);
            }
            else
            {
                Portno_to_serve = new_port_no;
                cout << "Port Number successfully changed to " << new_port_no << endl;
            }
        }
    }
}

// Check if Port No Already in use

bool Socket_and_Port::portInUse(int port_no)
{
    int new_socket = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in newCurr;
    socklen_t len = sizeof(newCurr);
    newCurr.sin_port = htons(port_no);
    newCurr.sin_family = AF_INET;
    newCurr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (bind(new_socket, (struct sockaddr *)&newCurr, len) < 0)
    {
        perror("Socket Already in Use");
        return true;
    }
    else
    {
        close(new_socket);
        return false;
    }
}

// Get the IP  the current node
string Socket_and_Port::getIpAddress()
{
    string ip = inet_ntoa(current.sin_addr);
    return ip;
}

// get port number
int Socket_and_Port::getPortNumber()
{
    return Portno_to_serve;
}

// Get current Socket

int Socket_and_Port::getSocketFd()
{
    return curr_socket;
}

// Close socket
void Socket_and_Port::closeSocket()
{
    close(curr_socket);
}
