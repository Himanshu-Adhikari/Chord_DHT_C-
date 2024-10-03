#ifndef functions_h
#define functions_h

#include <iostream>

#include "port.h"
#include "nodeInformation.h"

using namespace std;
#define psi pair<string,int>
typedef long long int lli;

void put(string key,string value,NODE_INFO &nodeInfo);
void get(string key,NODE_INFO nodeInfo);
void create(NODE_INFO &nodeInfo);
void join(NODE_INFO &nodeInfo,string ip,string port);
void printState(NODE_INFO nodeInfo);
void listenTo(NODE_INFO &nodeInfo);
void doStabilize(NODE_INFO &nodeInfo);
void callNotify(NODE_INFO &nodeInfo,string ipAndPort);
void callFixFingers(NODE_INFO &nodeInfo);
void doTask(NODE_INFO &nodeInfo,int newSock,struct sockaddr_in client,string nodeIdString);
void leave(NODE_INFO &nodeInfo);
void showHelp();

#endif