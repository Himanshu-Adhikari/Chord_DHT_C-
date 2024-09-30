#include <iostream>
#include "all_header.h"
#include "M.h"
#include "Node_info.h"
#include "HelperClass.h"

using namespace std;

Node_information::Node_information()
{
    finger_table = vector<ppsl>(nodes_count + 1);
    successor_list = vector<ppsl>(successors + 1);
    isInRing = false;
}

void Node_information::setStatus()
{
    isInRing = true;
}
void Node_information::setSuccessor(string ip, int port, lli hashed_key)
{
    successor = {{ip, port}, hashed_key};
}

void Node_information::setSuccessorList(string ip, int port, lli hashed_key)
{
    for (int i = 1; i <= successors; i++)
    {
        successor_list[i] = {{ip, port}, hashed_key};
    }
}

void Node_information::setPredecessor(string ip, int port, lli hashed_key)
{
    predecessor = {{ip, port}, hashed_key};
}

void Node_information::setId(lli nodeid)
{
    id = nodeid;
}

void Node_information::setFingerTable(string ip, int port, lli hashed_key)
{
    for (int i = 1; i <= nodes_count; i++)
    {
        finger_table[i] = {{ip, port}, hashed_key};
    }
}

void Node_information::StoreKeys(lli key, string val)
{
    dictionary[key] = val;
}

void Node_information::printKeys()
{
    for (auto &[key, val] : dictionary)
    {
        cout << key << " " << val << endl;
    }
}

void Node_information::updateSuccessorList()
{
    Helper_Functions helper;
    vector<pair<string, int>> list = helper.getSuccessorListFromNode(successor.first.first, successor.first.second);

    if (list.size() == successors)
        return;
    for (int i = 2; i <= successors; i++)
    {
        successor_list[i].first.first = list[i - 2].first;
        successor_list[i].first.second = list[i - 2].second;
        successor_list[i].second = helper.getHash(list[i - 2].first + ":" + to_string(list[i - 2].second));
    }
}

/*send all keys of this node to it's successor after it leaves the ring*/
vector<pair<lli, string>> Node_information::getAllKeysFromSuccessor()
{
    vector<pair<lli, string>> res;
    for (auto &[key, val] : dictionary)
    {
        res.push_back({key, val});
    }
    dictionary.clear();
    return res;
}

vector<pair<lli, string>> Node_information::getAllKeysFromPredecessor(lli nodeid)
{
    vector<pair<lli, string>> res;
    for (auto &[key, val] : dictionary)
    {
        if (id < nodeid)
        {

            // if predecessor's id is more than current node's id
            if (key > id && key <= nodeid)
            {
                res.push_back({key, val});
                dictionary.erase(key);
            }
        }
        else
        {

            /*if predecessor's id is less than current node's id*/
            if (key <= nodeid || key > id)
            {
                res.push_back({key, val});
                dictionary.erase(key);
            }
        }
    }
    return res;
}

ppsl Node_information::findSuccessor(lli nodeid)
{
    ppsl self = {{sp.getIpAddress(), sp.getPortNumber()}, sp.getSocketFd()};

    if (nodeid > id && nodeid <= successor.second)
        return successor;
    else if (id == successor.second || nodeid == id)
    {
        return self;
    }
    else if (successor.second == predecessor.second)
    {
        if (successor.second >= id)
        {
            if (nodeid > successor.second || nodeid < id)
                return self;
        }
        else
        {
            if ((nodeid > id && nodeid > successor.second) || nodeid < id && nodeid < successor.second)
            {
                return successor;
            }
            else
                return self;
        }
    }
    else
    {
        ppsl node=closestPrecedingNode(nodeid);
        if(node.second==id){
            return successor;
        }
        else{
            
			/* connect to node which will now find the successor */
			struct sockaddr_in serverToConnectTo;
			socklen_t len = sizeof(serverToConnectTo);

			string ip;
			int port;

			/* if this node couldn't find closest preciding node for given node id then now ask it's successor to do so */
			if(node.second == -1){
				node = successor;
			}

			Helper_Functions helper;

			helper.setServerDetails(serverToConnectTo,node.first.first,node.first.second);

			/* set timer on this socket */
    		struct timeval timer;
    		helper.setTimer(timer);


			int sockT = socket(AF_INET,SOCK_DGRAM,0);

			setsockopt(sockT,SOL_SOCKET,SO_RCVTIMEO,(char*)&timer,sizeof(struct timeval));

			if(sockT < 0){
				cout<<"socket cre error";
				perror("error");
				exit(-1);
			}

			/* send the node's id to the other node */
			char nodeIdChar[40];
			strcpy(nodeIdChar,to_string(nodeid).c_str());
			sendto(sockT, nodeIdChar, strlen(nodeIdChar), 0, (struct sockaddr*) &serverToConnectTo, len);

			/* receive ip and port of node's successor as ip:port*/
			char ipAndPort[40];

			int l = recvfrom(sockT, ipAndPort, 1024, 0, (struct sockaddr *) &serverToConnectTo, &len);

			close(sockT);

			if(l < 0){
				pair < pair<string,int> , lli > node;
				node.first.first = "";
				node.second = -1;
				node.first.second = -1;
				return node;
			}

			ipAndPort[l] = '\0';

			/* set ip,port and hash for this node and return it */
			string key = ipAndPort;
    		lli hash = helper.getHash(ipAndPort);
    		pair<string,int> ipAndPortPair = helper.Get_Ip_and_Port(key);
    		node.first.first = ipAndPortPair.first;
    		node.first.second = ipAndPortPair.second;
    		node.second = hash;

    		return node;
        }
    }
}

ppsl Node_information::closestPrecedingNode(lli nodeid){
    Helper_Functions helper;
    for(int i=nodes_count;i>=1;i--){
        if(finger_table[i].first.first=="" || finger_table[i].first.second==-1 || finger_table[i].second==-1)continue;
        if(finger_table[i].second>id && finger_table[i].second<nodeid)return finger_table[i];
        else{
            
        }
    }
}