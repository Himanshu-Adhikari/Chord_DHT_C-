#ifndef nodeInfo_h
#define nodeInfo_h

#include <iostream>
#include <vector>
#include <map>

#include "PortHeader.h"
#include "M.h"

#define lli long long int
#define ppsl pair<pair<string, int>, lli>
using namespace std;

class Node_information
{
private:
    lli id;
    ppsl predecessor, successor;
    vector<ppsl> finger_table;
    map<lli, string> dictionary;
    vector<ppsl> successor_list;

    bool isInRing;

public:
    Socket_and_Port sp;
    Node_information();
    ppsl findSuccessor(lli nodeId);
    ppsl closestPrecedingNode(lli nodeId);
    void FixFingerTable();
    void Stabilize();
    void notify(ppsl node);
    void checkPredecessor();
    void checkSuccessor();
    void updateSuccessorList();
    void printKeys();
    void StoreKeys(lli key, string val);

    vector<pair<lli, string>> getAllKeysFromSuccessor();
    vector<pair<lli, string>> getAllKeysFromPredecessor(lli nodeId);

    void setSuccessor(string ip, int port, lli hash_val);
    void setSuccessorList(string ip, int port, lli hash_val);
    void setPredecessor(string ip, int port, lli hash_val);
    void setFingerTable(string ip, int port, lli hash_val);
    void setId(lli nodeId);
    void setStatus();

    string getValues(lli key);
    vector<ppsl> getFingerTable();
    ppsl getSuccessor();
    ppsl getPredeccessor();
    vector<ppsl> getSuccessorList();
    lli getId();
    bool getStatus();
};

#endif