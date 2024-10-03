#ifndef nodeInfo_h
#define nodeInfo_h

#include <iostream>
#include <vector>
#include <map>

#include "port.h"
#include "M.h"

using namespace std;
#define ppsl pair< pair<string,int> , lli >
typedef long long int lli;

class NODE_INFO{
	private:
		lli id;
		ppsl predecessor;
		ppsl successor;
		vector< ppsl > fingerTable;
		map<lli,string> dictionary;
		vector< ppsl > successorList;

		bool isInRing;

	public:
		SocketAndPort sp;
		
		NODE_INFO();

		ppsl findSuccessor(lli nodeId);
		ppsl closestPrecedingNode(lli nodeId);
		void fixFingers();
		void stabilize();
		void notify(ppsl node);
		void checkPredecessor();
		void checkSuccessor();
		void updateSuccessorList();

		void printKeys();
		void storeKey(lli key,string val);
		vector< pair<lli , string> > getAllKeysForSuccessor();
		vector< pair<lli , string> > getKeysForPredecessor(lli nodeId);

		void setSuccessor(string ip,int port,lli hash);
		void setSuccessorList(string ip,int port,lli hash);
		void setPredecessor(string ip,int port,lli hash);
		void setFingerTable(string ip,int port,lli hash);
		void setId(lli id);
		void setStatus();
		
		lli getId();
		string getValue(lli key);
		vector< ppsl > getFingerTable();
		ppsl getSuccessor();
		ppsl getPredecessor(); 
		vector< ppsl > getSuccessorList();
		bool getStatus();
};

#endif