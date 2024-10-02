#define functions_h
#ifndef functions.h

#include <iostream>
#include "PortHeader.h"
#include "Node_info.h"

using namespace std;

#define lli long long int

void put(string key,string value,Node_information &nodeinfo);
void get(string key,Node_information nodeinfo);
void create(Node_information &nodeinfo);
void join(Node_information &nodeinfo,string ip ,string port);
void printState(Node_information nodeinfo);
void listento(Node_information &nodeinfo);
void doStabilize(Node_information &nodeinfo);
void call_notify(Node_information &nodeinfo,string ip_port);
void call_fixfingers(Node_information &nodeinfo);
void dotask(Node_information &nodeinfo,int newsocket,struct sockaddr_in client,string node_id);
void leave(Node_information &nodeinfo);
void show_help();

#endif