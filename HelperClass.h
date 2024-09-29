#ifndef helper_h
#define helper_h

#include<iostream>
#include "Node_info.h"

using namespace std;

#define lli long long int
#define ppsl pair<pair<string, int>, lli>

class Helper_Functions{
    public:
        vector<string>splitCommand(string command);
        string Combine_Ip_and_Port(string ip,string port);
        vector<pair<lli,string>> Separate_Keys_and_Values(string keysandvalues);
        vector<pair<string,int>> SeparateSuccessorList(string successorList);
        string SplitSuccessorList(vector<ppsl>succlist);

        lli getHash(string key);
        pair<string, int> Get_Ip_and_Port(string key);

        bool isKeyValue(string id);
        bool isNodeAlive(string ip,int port);

        void setServerDetails(struct sockaddr_in &server,string ip,int port);
        void setTimer(struct timeval &timer);

        void sendNecessaryKeys(Node_information &nodeinfo,int newSocket,struct sockaddr_in client,string nodeIdString);
        void sendKeyToNode(ppsl node,lli keyHash,string value);
		void sendValToNode(Node_information nodeInfo,int newSocket,struct sockaddr_in client,string nodeIdString);
		string getKeyFromNode(ppsl node,string keyHash);
		pair<lli,string> getKeyAndVal(string keyAndVal);
		void getKeysFromSuccessor(Node_information &nodeInfo,string ip,int port);
		void storeAllKeys(Node_information &nodeInfo,string keysAndValues);

		ppsl getPredecessorNode(string ip,int port,string ipClient,int ipPort,bool forStabilize);
		lli getSuccessorId(string ip,int port);

		void sendPredecessor(Node_information nodeInfo,int newSocket,struct sockaddr_in client);
		void sendSuccessor(Node_information nodeInfo,string nodeIdString,int newSocket,struct sockaddr_in client);
		void sendSuccessorId(Node_information nodeInfo,int newSocket,struct sockaddr_in client);
		void sendAcknowledgement(int newSocket,struct sockaddr_in client);

		vector< pair<string,int> > getSuccessorListFromNode(string ip,int port);
		void sendSuccessorList(Node_information &nodeInfo,int sock,struct sockaddr_in client);
};


#endif