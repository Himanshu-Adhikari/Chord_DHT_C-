#ifndef helper_h
#define helper_h

#include <iostream>

#include "nodeInformation.h"

using namespace std;

typedef long long int lli;

class HelperFunctions{

	public:

		vector<string> splitCommand(string command);
		string combineIpAndPort(string ip,string port);
		vector< pair<lli,string> > seperateKeysAndValues(string keysAndValues);
		vector< pair<string,int> > seperateSuccessorList(string succList);
		string splitSuccessorList(vector< ppsl > list);
		
		lli getHash(string key);
		pair<string,int> getIpAndPort(string key);
		
		bool isKeyValue(string id);

		bool isNodeAlive(string ip,int port);
		
		void setServerDetails(struct sockaddr_in &server,string ip,int port);
		void setTimer(struct timeval &timer);

		void sendNeccessaryKeys(NODE_INFO &nodeInfo,int newSock,struct sockaddr_in client,string nodeIdString);
		void sendKeyToNode(ppsl node,lli keyHash,string value);
		void sendValToNode(NODE_INFO nodeInfo,int newSock,struct sockaddr_in client,string nodeIdString);
		string getKeyFromNode(ppsl node,string keyHash);
		pair<lli,string> getKeyAndVal(string keyAndVal);
		void getKeysFromSuccessor(NODE_INFO &nodeInfo,string ip,int port);
		void storeAllKeys(NODE_INFO &nodeInfo,string keysAndValues);

		ppsl getPredecessorNode(string ip,int port,string ipClient,int ipPort,bool forStabilize);
		lli getSuccessorId(string ip,int port);

		void sendPredecessor(NODE_INFO nodeInfo,int newSock,struct sockaddr_in client);
		void sendSuccessor(NODE_INFO nodeInfo,string nodeIdString,int newSock,struct sockaddr_in client);
		void sendSuccessorId(NODE_INFO nodeInfo,int newSock,struct sockaddr_in client);
		void sendAcknowledgement(int newSock,struct sockaddr_in client);

		vector< pair<string,int> > getSuccessorListFromNode(string ip,int port);
		void sendSuccessorList(NODE_INFO &nodeInfo,int sock,struct sockaddr_in client);
};


#endif