#include "all_header.h"
#include "M.h"
#include "functions.h"
#include "HelperClass.h"

#define lli long long int
using namespace std;

Helper_Functions helper = Helper_Functions();

/*Adding the entered key to the proper node*/
void put(string key, string value, Node_information &nodeinfo)
{
    if (key == "" || value == "")
    {
        cout << "Key or Value is empty" << endl;
        return;
    }
    lli key_hash = helper.getHash(key);
    cout << "Key is " << key << " and it's hash :" << key_hash << endl;

    ppsl node = nodeinfo.findSuccessor(key_hash);
    helper.sendKeyToNode(node, key_hash, value);
    cout << "Key successfully stored" << endl;
}

/*Get the key from the node it is stored in*/
void get(string key, Node_information nodeinfo)
{
    if (key == "")
    {
        cout << "key is empty" << endl;
        return;
    }
    else
    {
        lli key_hash = helper.getHash(key);
        ppsl node = nodeinfo.findSuccessor(key_hash);

        string val = helper.getKeyFromNode(node, to_string(key_hash));

        if (val == "")
        {
            cout << "Key not found" << endl;
        }
        else
        {
            cout << "Key : " << key << " value : " << val << endl;
        }
    }
}


/*Create a new ring*/

void create(Node_information &nodeinfo)
{
    string ip = nodeinfo.sp.getIpAddress();
    int port = nodeinfo.sp.getPortNumber();

    /*Key to be hashed for a particular node if ip:port pair*/
    string key = ip + ":" + to_string(port);
    lli hash = helper.getHash(key);

    /*setting id,successor,predecessor,successor list,finger table and status of node*/
    nodeinfo.setId(hash);
    nodeinfo.setSuccessor(ip, port, hash);
    nodeinfo.setSuccessorList(ip, port, hash);
    nodeinfo.setPredecessor("", -1, -1);
    nodeinfo.setFingerTable(ip, port, hash);
    nodeinfo.setStatus();

    /* launch threads,one thread will listen to request from other nodes,one will do stabilization */
    thread second(listento, ref(nodeinfo));
    second.detach();

    thread fifth(doStabilize, ref(nodeinfo));
    fifth.detach();
}



/*Join a DHT Ring*/
void join(Node_information &nodeinfo, string ip, string port)
{

    if (helper.isNodeAlive(ip, atoi(port.c_str())) == false)
    {
        cout << "Sorry but no node is active on this ip or port\n";
        return;
    }

    /* set server socket details */
    struct sockaddr_in server;

    socklen_t l = sizeof(server);

    helper.setServerDetails(server, ip, stoi(port));

    int sock = socket(AF_INET, SOCK_DGRAM, 0);

    if (sock < 0)
    {
        perror("error");
        exit(-1);
    }

    string currIp = nodeinfo.sp.getIpAddress();
    string currPort = to_string(nodeinfo.sp.getPortNumber());

    /* generate id of current node */
    lli nodeId = helper.getHash(currIp + ":" + currPort);

    char charNodeId[41];
    strcpy(charNodeId, to_string(nodeId).c_str());

    /* node sends it's id to main node to find it's successor */
    if (sendto(sock, charNodeId, strlen(charNodeId), 0, (struct sockaddr *)&server, l) == -1)
    {
        cout << "yaha 1\n";
        perror("error");
        exit(-1);
    }

    /* node receives id and port of it's successor */
    char ipAndPort[40];
    int len;
    if ((len = recvfrom(sock, ipAndPort, 1024, 0, (struct sockaddr *)&server, &l)) == -1)
    {
        cout << "yaha 2\n";
        perror("error");
        exit(-1);
    }
    ipAndPort[len] = '\0';

    close(sock);

    cout << "Successfully joined the ring\n";

    string key = ipAndPort;
    lli hash = helper.getHash(key);
    pair<string, int> ipAndPortPair = helper.Get_Ip_and_Port(key);

    /* setting id, successor , successor list , predecessor, finger table and status */
    nodeinfo.setId(nodeId);
    nodeinfo.setSuccessor(ipAndPortPair.first, ipAndPortPair.second, hash);
    nodeinfo.setSuccessorList(ipAndPortPair.first, ipAndPortPair.second, hash);
    nodeinfo.setPredecessor("", -1, -1);
    nodeinfo.setFingerTable(ipAndPortPair.first, ipAndPortPair.second, hash);
    nodeinfo.setStatus();

    /* get all keys from it's successor which belongs to it now */
    helper.getKeysFromSuccessor(nodeinfo, ipAndPortPair.first, ipAndPortPair.second);

    /* launch threads,one thread will listen to request from other nodes,one will do stabilization */
    thread fourth(listento, ref(nodeinfo));
    fourth.detach();

    thread third(doStabilize, ref(nodeinfo));
    third.detach();
    return;
}


/*print successor,predecessor ,successor list and finger table of a node*/
void printState(Node_information nodeinfo)
{
    string ip = nodeinfo.sp.getIpAddress();
    lli id = nodeinfo.getId();
    int port = nodeinfo.sp.getPortNumber();
    vector<ppsl> fingertable = nodeinfo.getFingerTable();
    cout << "Node ip: " << ip << " Port :" << port << " Nodeid :" << id << endl;
    ppsl succ = nodeinfo.getSuccessor();
    ppsl pred = nodeinfo.getPredeccessor();
    vector<ppsl> succ_list = nodeinfo.getSuccessorList();
    cout << "successor ip: " << succ.first.first << " port : " << succ.first.second << " id:" << succ.second << endl;
    cout << "predecessor ip: " << pred.first.first << " port : " << pred.first.second << " id:" << pred.second << endl;

    cout << "___ Finger Table ___" << endl;
    for (int i = 1; i <= nodes_count; i++)
    {
        ip = fingertable[i].first.first;
        port = fingertable[i].first.second;
        id = fingertable[i].second;
        cout << i << " :" << id << "  ip: " << ip << " port:" << port << endl;
    }
    cout << endl;
    cout << "___ Successor Table ___" << endl;
    for (int i = 1; i <= successors; i++)
    {
        ip = succ_list[i].first.first;
        port = succ_list[i].first.second;
        id = succ_list[i].second;
        cout << i << " :" << id << "  ip: " << ip << " port:" << port << endl;
    }
}




/* nodes leaves the ring */

void leave(Node_information &nodeinfo)
{

    ppsl succ = nodeinfo.getSuccessor();
    lli id = nodeinfo.getId();

    if (id == succ.second)
    {
        return;
    }

    /*Transfer all keys to the successorbefore leaving the ring */

    vector<pair<lli, string>> Keys_Values = nodeinfo.getAllKeysFromSuccessor();
    if (Keys_Values.size() == 0)
        return;

    string key_value_string = "";

    for (auto [k, val] : Keys_Values)
    {
        key_value_string += to_string(k) + ":" + val + ";";
    }
    key_value_string += "storeKeys";
    struct sockaddr_in server_to_connect_to;
    socklen_t len = sizeof(server_to_connect_to);

    helper.setServerDetails(server_to_connect_to, succ.first.first, succ.first.second);

    int sockt = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockt < 0)
    {
        perror("error");
        exit(-1);
    }

    char Keys_Values_char[2000];
    strcpy(Keys_Values_char, key_value_string.c_str());

    sendto(sockt, Keys_Values_char, strlen(Keys_Values_char), 0, (struct sockaddr *)&server_to_connect_to, len);
    close(sockt);
}




void doTask(Node_information &nodeinfo, int newsocket, struct sockaddr_in client, string nodeidstring)
{

    /* predecessor of this node has left the ring and has sent all it's keys to this node(it's successor) */
    if (nodeidstring.find("storeKeys") != -1)
    {
        helper.storeAllKeys(nodeinfo, nodeidstring);
    }

    /* check if the sent msg is in form of key:val, if yes then store it in current node (for put ) */
    else if (helper.isKeyValue(nodeidstring))
    {
        pair<lli, string> keyAndVal = helper.getKeyAndVal(nodeidstring);
        nodeinfo.StoreKeys(keyAndVal.first, keyAndVal.second);
    }

    else if (nodeidstring.find("alive") != -1)
    {
        helper.sendAcknowledgement(newsocket, client);
    }

    /* contacting node wants successor list of this node */
    else if (nodeidstring.find("sendSuccList") != -1)
    {
        helper.sendSuccessorList(nodeinfo, newsocket, client);
    }

    /* contacting node has just joined the ring and is asking for keys that belongs to it now */
    else if (nodeidstring.find("getKeys") != -1)
    {
        helper.sendNecessaryKeys(nodeinfo, newsocket, client, nodeidstring);
    }

    /* contacting node has run get command so send value of key it requires */
    else if (nodeidstring.find("k") != -1)
    {
        helper.sendValToNode(nodeinfo, newsocket, client, nodeidstring);
    }

    /* contacting node wants the predecessor of this node */
    else if (nodeidstring.find("p") != -1)
    {
        helper.sendPredecessor(nodeinfo, newsocket, client);

        /* p1 in msg means that notify the current node about this contacting node */
        if (nodeidstring.find("p1") != -1)
        {
            call_notify(nodeinfo, nodeidstring);
        }
    }

    /* contacting node wants successor Id of this node for helper in finger table */
    else if (nodeidstring.find("finger") != -1)
    {
        helper.sendSuccessorId(nodeinfo, newsocket, client);
    }

    /* contacting node wants current node to find successor for it */
    else
    {
        helper.sendSuccessor(nodeinfo, nodeidstring, newsocket, client);
    }
}


/* listen to any contacting node */
void listento(Node_information &nodeInfo)
{
    struct sockaddr_in client;
    socklen_t l = sizeof(client);

    /* wait for any client to connect and create a new thread as soon as one connects */
    while (1)
    {
        char charNodeId[40];
        int sock = nodeInfo.sp.getSocketFd();
        int len = recvfrom(sock, charNodeId, 1024, 0, (struct sockaddr *)&client, &l);
        charNodeId[len] = '\0';
        string nodeIdString = charNodeId;

        /* launch a thread that will perform diff tasks acc to received msg */
        thread f(doTask, ref(nodeInfo), sock, client, nodeIdString);
        f.detach();
    }
}

void doStabilize(Node_information &nodeInfo)
{

    /* do stabilize tasks */
    while (1)
    {

        nodeInfo.checkPredecessor();

        nodeInfo.checkSuccessor();

        nodeInfo.Stabilize();

        nodeInfo.updateSuccessorList();

        nodeInfo.FixFingerTable();

        this_thread::sleep_for(chrono::milliseconds(300));
    }
}

/* call notify of current node which will notify curr node of contacting node */
void call_notify(Node_information &nodeInfo, string ipAndPort)
{

    ipAndPort.pop_back();
    ipAndPort.pop_back();

    /* get ip and port of client node */
    pair<string, int> ipAndPortPair = helper.Get_Ip_and_Port(ipAndPort);
    string ip = ipAndPortPair.first;
    int port = ipAndPortPair.second;
    lli hash = helper.getHash(ipAndPort);

    pair<pair<string, int>, lli> node;
    node.first.first = ip;
    node.first.second = port;
    node.second = hash;

    /* notify current node about this node */
    nodeInfo.notify(node);
}

/* tell about all commands */
void show_help()
{
    cout << "1) create : will create a DHT ring\n\n";
    cout << "2) join <ip> <port> : will join ring by connecting to main node having ip and port\n\n";
    cout << "3) printstate : will print successor, predecessor, fingerTable and Successor list\n\n";
    cout << "4) print : will print all keys and values present in that node\n\n";
    cout << "5) port : will display port number on which node is listening\n\n";
    cout << "6) port <number> : will change port number to mentioned number if that port is free\n\n";
    cout << "7) put <key> <value> : will put key and value to the node it belongs to\n\n";
    cout << "8) get <key> : will get value of mentioned key\n\n";
}