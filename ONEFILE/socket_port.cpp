

#include <iostream>

#include "macros_and_headers.h"
#include "socket_port.h"

/* Generate a port number for the server */
void socket_and_port::specifyPortServer()
{

    /* Seed the random number generator with the current time */
    srand(time(0));

    /* Generate a random port number between 0 and 65535 */
    serving_port = rand() % 65536;

    /* Ensure the port number is at least 1024 (valid range for non-privileged ports) */
    if (serving_port < 1024)
        serving_port += 1024; // Shift the number up to be within the valid range

    /* Define the length of the address structure */
    socklen_t len = sizeof(current);

    /* Create a UDP socket */
    serving_socket = socket(AF_INET, SOCK_DGRAM, 0);

    /* Specify the address family (IPv4) */
    current.sin_family = AF_INET;

    /* Set the port number, converting from host byte order to network byte order */
    current.sin_port = htons(serving_port);

    /* Set the IP address to localhost */
    current.sin_addr.s_addr = inet_addr("127.0.0.1");

    /* Bind the socket to the specified address and port */
    if (bind(serving_socket, (struct sockaddr *)&current, len) < 0)
    {
        perror("error"); // Print an error message if binding fails
        exit(-1);        // Exit the program with an error status
    }
}

/* Change the port number for the server */
void socket_and_port::Change_port_no_to(int new_port)
{
    // Check if the new port number is within the valid range (1024 - 65535)
    if (new_port < 1024 || new_port > 65535)
    {
        cout << "Please enter a valid port number\n";
    }
    else
    {
        // Check if the new port number is already in use
        if (IF_CURRENT_PORT_IN_USE(new_port))
        {
            cout << "Sorry, but the port number is already in use\n";
        }
        else
        {
            // Close the existing socket before changing the port
            close(serving_socket);

            // Define the length of the address structure
            socklen_t addressLength = sizeof(current);

            // Create a new UDP socket
            serving_socket = socket(AF_INET, SOCK_DGRAM, 0);

            // Set the new port number in network byte order
            current.sin_port = htons(new_port);

            // Attempt to bind the new socket to the specified address and port
            if (bind(serving_socket, (struct sockaddr *)&current, addressLength) < 0)
            {
                perror("error"); // Print an error message if binding fails
                // Revert the port number back to the previous one if binding fails
                current.sin_port = htons(serving_port);
            }
            else
            {
                // Update the current port number to the new port
                serving_port = new_port;
                cout << "Port number changed to: " << serving_port << endl;
            }
        }
    }
}
/* Check if a port number is already in use */
bool socket_and_port::IF_CURRENT_PORT_IN_USE(int port_no)
{
    // Create a new socket for checking the port status
    int checkSock = socket(AF_INET, SOCK_DGRAM, 0);

    // Prepare the address structure for the specified port
    struct sockaddr_in address;
    socklen_t addressLength = sizeof(address);
    address.sin_port = htons(port_no);                // Set the port number in network byte order
    address.sin_family = AF_INET;                     // Specify the address family (IPv4)
    address.sin_addr.s_addr = inet_addr("127.0.0.1"); // Use localhost

    // Attempt to bind the socket to the specified address and port
    if (bind(checkSock, (struct sockaddr *)&address, addressLength) < 0)
    {
        perror("error"); // Print an error message if binding fails
        return true;     // Port is in use
    }
    else
    {
        close(checkSock); // Close the socket if binding succeeded
        return false;     // Port is available
    }
}
/* Get the IP address of the server */
string socket_and_port::GET_IP()
{
    // Convert the IP address from network byte order to a human-readable string
    string ipAddress = inet_ntoa(current.sin_addr);
    return ipAddress; // Return the IP address
}

/* Get the port number on which the server is listening */
int socket_and_port::GET_PORT()
{
    return serving_port; // Return the currently set port number
}

/* Get the socket file descriptor */
int socket_and_port::getSocketFd()
{
    return serving_socket; // Return the socket file descriptor
}

void socket_and_port::closeSocket()
{
    close(serving_socket); // Close the socket to release the resource
}
