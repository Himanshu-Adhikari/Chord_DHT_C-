#include <iostream>

#include "functions.h"
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
    for (auto k : dictionary)
    {
        cout << k.first << " " << k.second << endl;
    }
}

void Node_information::updateSuccessorList()
{
    Helper_Functions helper;
    vector<pair<string, int>> list = helper.getSuccessorListFromNode(successor.first.first, successor.first.second);

    if (list.size() != successors)
        return;
    successor_list[1]=successor;
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
    for (auto k : dictionary)
    {
        res.push_back({k.first, k.second});
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
    ppsl self = {{sp.getIpAddress(), sp.getPortNumber()}, id};

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
        ppsl node = closestPrecedingNode(nodeid);
        if (node.second == id)
        {
            return successor;
        }
        else
        {

            /* connect to node which will now find the successor */
            struct sockaddr_in serverToConnectTo;
            socklen_t len = sizeof(serverToConnectTo);

            string ip;
            int port;

            /* if this node couldn't find closest preciding node for given node id then now ask it's successor to do so */
            if (node.second == -1)
            {
                node = successor;
            }

            Helper_Functions helper;

            helper.setServerDetails(serverToConnectTo, node.first.first, node.first.second);

            /* set timer on this socket */
            struct timeval timer;
            helper.setTimer(timer);

            int sockT = socket(AF_INET, SOCK_DGRAM, 0);

            setsockopt(sockT, SOL_SOCKET, SO_RCVTIMEO, (char *)&timer, sizeof(struct timeval));

            if (sockT < 0)
            {
                cout << "socket creation error";
                perror("error");
                exit(-1);
            }

            /* send the node's id to the other node */
            char nodeIdChar[40];
            strcpy(nodeIdChar, to_string(nodeid).c_str());
            sendto(sockT, nodeIdChar, strlen(nodeIdChar), 0, (struct sockaddr *)&serverToConnectTo, len);

            /* receive ip and port of node's successor as ip:port*/
            char ipAndPort[40];

            int l = recvfrom(sockT, ipAndPort, 1024, 0, (struct sockaddr *)&serverToConnectTo, &len);

            close(sockT);

            if (l < 0)
            {
                pair<pair<string, int>, lli> node;
                node.first.first = "";
                node.second = -1;
                node.first.second = -1;
                return node;
            }

            ipAndPort[l] = '\0';

            /* set ip,port and hash for this node and return it */
            string key = ipAndPort;
            lli hash = helper.getHash(ipAndPort);
            pair<string, int> ipAndPortPair = helper.Get_Ip_and_Port(key);
            node={{ipAndPortPair.first,ipAndPortPair.second},hash};
            return node;
        }
    }
    return {{"",-1},-1};
}

ppsl Node_information::closestPrecedingNode(lli nodeid)
{
    Helper_Functions helper;
    for (int i = nodes_count; i >= 1; i--)
    {
        if (finger_table[i].first.first == "" || finger_table[i].first.second == -1 || finger_table[i].second == -1)
            continue;
        if (finger_table[i].second > id && finger_table[i].second < nodeid)
            return finger_table[i];
        else
        {
            lli successor_id = helper.getSuccessorId(finger_table[i].first.first, finger_table[i].first.second);
            if (successor_id == -1)
                continue;

            if (finger_table[i].second > successor_id)
            {
                if ((nodeid <= finger_table[i].second && nodeid <= successor_id) || (nodeid >= finger_table[i].second && nodeid >= successor_id))
                {
                    return finger_table[i];
                }
            }
            else if (finger_table[i].second < successor_id && nodeid > finger_table[i].second && nodeid < successor_id)
            {
                return finger_table[i];
            }

            pair<pair<string, int>, lli> predNode = helper.getPredecessorNode(finger_table[i].first.first, finger_table[i].first.second, "", -1, false);
            lli predecessorId = predNode.second;

            if (predecessorId != -1 && finger_table[i].second < predecessorId)
            {
                if ((nodeid <= finger_table[i].second && nodeid <= predecessorId) || (nodeid >= finger_table[i].second && nodeid >= predecessorId))
                {
                    return predNode;
                }
            }
            if (predecessorId != -1 && finger_table[i].second > predecessorId && nodeid >= predecessorId && nodeid <= finger_table[i].second)
            {
                return predNode;
            }
        }
    }

    // when don't have a preceding node for a node in a ring
    ppsl node = {{"", -1}, -1};
    return node;
}

void Node_information::Stabilize()
{
    Helper_Functions helper;

    string node_ip = sp.getIpAddress();
    int node_port = sp.getPortNumber();
    if (helper.isNodeAlive(successor.first.first, successor.first.second) == false)
        return;

    // get predecessor of successor

    ppsl successors_pred_node = helper.getPredecessorNode(successor.first.first, successor.first.second, node_ip, node_port, true);

    lli successors_pred_hash = successors_pred_node.second;

    if (successors_pred_hash == -1 || successors_pred_node.second == -1)
        return;

    if (successors_pred_hash > id || (successors_pred_hash > id && successors_pred_hash < successor.second) || (successors_pred_hash < id && successors_pred_hash < successor.second))
    {
        successor = successors_pred_node;
    }
}

// check if current node's predecessor is still alive or not
void Node_information::checkPredecessor()
{
    if (predecessor.second == -1)
        return;
    Helper_Functions helper;

    string ip = predecessor.first.first;
    int port = predecessor.first.second;

    if (helper.isNodeAlive(ip, port) == false)
    {
        if (predecessor.second == successor.second)
        {
            successor.first.first = sp.getIpAddress();
            successor.first.second = sp.getPortNumber();
            successor.second = id;
            setSuccessorList(successor.first.first, successor.first.second, id);
        }

        predecessor = {{"", -1}, -1};
    }
}

// Check if current node's successor is still  alive or not

void Node_information::checkSuccessor()
{
    if (successor.second == id)
        return;

    Helper_Functions helper;
    string ip = successor.first.first;
    int port = successor.first.second;

    if (helper.isNodeAlive(ip, port) == false)
    {
        successor = successor_list[2];
        updateSuccessorList();
    }
}

void Node_information::notify(ppsl node)
{
    // get id of node and predecessor
    lli predecessor_hash = predecessor.second;
    lli node_hash = node.second;
    predecessor = node;

    /* if node's successor is node itself then set it's successor to this node */
    if (successor.second == id)
    {
        successor = node;
    }
}



void Node_information::FixFingerTable(){

	Helper_Functions helper;

	//if(helper.isNodeAlive(successor.first.first,successor.first.second) == false)
		//return;
	//cout<<"in fix fingers - "<<successor.second<<endl;
	
	int next = 1;
	lli mod = pow(2,nodes_count);

	while(next <= nodes_count){
		if(helper.isNodeAlive(successor.first.first,successor.first.second) == false)
			return;
		
		lli newId = id + pow(2,next-1);
		newId = newId % mod;
		ppsl node = findSuccessor(newId);
		if(node.first.first == "" || node.second == -1 || node.first.second == -1 )
			break;
		finger_table[next] = node;
		next++;	
	}

}

vector< ppsl > Node_information::getFingerTable(){
	return finger_table;
}

lli Node_information::getId(){
	return id;
}

ppsl Node_information::getSuccessor(){
	return successor;
}

ppsl Node_information::getPredeccessor(){
	return predecessor;
}

string Node_information::getValues(lli key){
	if(dictionary.find(key) != dictionary.end()){
		return dictionary[key];
	}
	else
		return "";
}

vector< ppsl > Node_information::getSuccessorList(){
	return successor_list;
}

bool Node_information::getStatus(){
	return isInRing;
}