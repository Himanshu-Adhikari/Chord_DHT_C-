#ifndef port_h
#define port_h

#include <iostream>
#include <netinet/in.h>
using namespace std;

class SocketAndPort{
	private:
		int portNoServer;
		int sock;
		struct sockaddr_in current;

	public:
		void specifyPortServer();
		void changePortNumber(int portNo);
		void closeSocket();
		bool CURRENT_PORT_IN_USE(int portNo);
		string GET_IP();
		int GET_PORT();
		int getSocketFd();
};

#endif