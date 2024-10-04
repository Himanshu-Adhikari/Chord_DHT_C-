#ifndef functions_h
#define functions_h

#include "socket_port.h"
#include "Node_info.h"
void put(string key, string value, NODE_INFORMATION &nodeInfo);
void get(string key, NODE_INFORMATION nodeInfo);
void create(NODE_INFORMATION &nodeInfo);
void join(NODE_INFORMATION &nodeInfo, string ip, string port);
void printState(NODE_INFORMATION nodeInfo);
void leave(NODE_INFORMATION &nodeInfo);
void doTask(NODE_INFORMATION &nodeInfo, int newSock, struct sockaddr_in client, string nodeIdString);
void listenTo(NODE_INFORMATION &nodeInfo);
void doStabilize(NODE_INFORMATION &nodeInfo);
void callNotify(NODE_INFORMATION &nodeInfo, string ipAndPort);
void showHelp();
#endif