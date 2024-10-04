#include "task_functions.h"
#include "macros_and_headers.h"
#include "Helper_class.h"

Helper_Class help;


/* put the entered key to the proper node */
void put(string key, string value, NODE_INFORMATION &nodeInfo)
{
    if (key == "" || value == "")
    {
        cout << "Key or value field empty\n";
        return;
    }

    else
    {

        lli keyHash = help.Get_hash(key);
        cout << "Key is " << key << " with hash : " << keyHash << endl;
        ppsl node = nodeInfo.find_successor(keyHash);

        help.Send_Key_to_Node(node, keyHash, value);

        cout << "key  successfully stored\n";
    }
}

/* get key from the desired node */
void get(string key, NODE_INFORMATION nodeInfo)
{

    if (key == "")
    {
        cout << "Key field empty\n";
        return;
    }
    else
    {

        lli keyHash = help.Get_hash(key);

        ppsl node = nodeInfo.find_successor(keyHash);

        string val = help.getKeyFromNode(node, to_string(keyHash));

        if (val == "")
            cout << "Key Not found\n";

        else
            cout << "Found " << key << " with " << val << endl;
    }
}

/* create a new ring */
void create(NODE_INFORMATION &nodeInfo)
{

    string ip = nodeInfo.sp.GET_IP();
    int port = nodeInfo.sp.GET_PORT();

    /* key to be hashed for a node is ip:port */
    string key = ip + ":" + (to_string(port));

    lli hash = help.Get_hash(key);

    /* setting id, successor , successor list , predecessor ,finger table and status of node */
    nodeInfo.set_nodeid(hash);
    nodeInfo.set_Successor(ip, port, hash);
    nodeInfo.set_SuccList(ip, port, hash);
    nodeInfo.set_Predecessor("", -1, -1);
    nodeInfo.set_Finger_table(ip, port, hash);
    nodeInfo.set_status();
    cout << "DHT Created with ip: " << ip << " port: " << port << endl;
    /* launch threads,one thread will listen to request from other nodes,one will do stabilization */
    thread second(listenTo, ref(nodeInfo));
    second.detach();

    thread fifth(doStabilize, ref(nodeInfo));
    fifth.detach();
}

/* join in a DHT ring */
void join(NODE_INFORMATION &nodeInfo, string ip, string port)
{

    if (help.is_node_alive(ip, atoi(port.c_str())) == false)
    {
        cout << "A ring with such ip:port don't exit \n";
        return;
    }

    /* set server socket details */
    struct sockaddr_in server;

    socklen_t l = sizeof(server);

    help.setServerDetails(server, ip, stoi(port));

    int sock = socket(AF_INET, SOCK_DGRAM, 0);

    if (sock < 0)
    {
        perror("error");
        exit(-1);
    }

    string currIp = nodeInfo.sp.GET_IP();
    string currPort = to_string(nodeInfo.sp.GET_PORT());

    /* generate id of current node */
    lli nodeId = help.Get_hash(currIp + ":" + currPort);

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
    lli hash = help.Get_hash(key);
    pair<string, int> ipAndPortPair = help.GET_IP_PORT(key);

    /* setting id, successor , successor list , predecessor, finger table and status */
    nodeInfo.set_nodeid(nodeId);
    nodeInfo.set_Successor(ipAndPortPair.first, ipAndPortPair.second, hash);
    nodeInfo.set_SuccList(ipAndPortPair.first, ipAndPortPair.second, hash);
    nodeInfo.set_Predecessor("", -1, -1);
    nodeInfo.set_Finger_table(ipAndPortPair.first, ipAndPortPair.second, hash);
    nodeInfo.set_status();

    /* get all keys from it's successor which belongs to it now */
    help.Get_Keys_from_successor(nodeInfo, ipAndPortPair.first, ipAndPortPair.second);

    /* launch threads,one thread will listen to request from other nodes,one will do stabilization */
    thread fourth(listenTo, ref(nodeInfo));
    fourth.detach();

    thread third(doStabilize, ref(nodeInfo));
    third.detach();
}

/* print successor,predecessor,successor list and finger table of node */
void printState(NODE_INFORMATION nodeInfo)
{
    string ip = nodeInfo.sp.GET_IP();
    lli id = nodeInfo.get_nodeid();
    int port = nodeInfo.sp.GET_PORT();
    vector<ppsl> fingerTable = nodeInfo.get_Finger_table();
    cout << "Self " << ip << " " << port << " " << id << endl;
    ppsl succ = nodeInfo.get_Successor();
    ppsl pre = nodeInfo.get_Predecessor();
    vector<ppsl> succList = nodeInfo.get_SuccList();
    cout << "Succ " << succ.first.first << " " << succ.first.second << " " << succ.second << endl;
    cout << "Pred " << pre.first.first << " " << pre.first.second << " " << pre.second << endl;
    for (int i = 1; i <= nodes; i++)
    {
        ip = fingerTable[i].first.first;
        port = fingerTable[i].first.second;
        id = fingerTable[i].second;
        cout << "Finger[" << i << "] " << id << " " << ip << " " << port << endl;
    }
    for (int i = 1; i <= succ_cnt; i++)
    {
        ip = succList[i].first.first;
        port = succList[i].first.second;
        id = succList[i].second;
        cout << "Successor[" << i << "] " << id << " " << ip << " " << port << endl;
    }
}

/* node leaves the DHT ring */
void leave(NODE_INFORMATION &nodeInfo)
{
    ppsl succ = nodeInfo.get_Successor();
    lli id = nodeInfo.get_nodeid();

    if (id == succ.second)
        return;

    /* transfer all keys to successor before leaving the ring */

    vector<pair<lli, string>> keysAndValuesVector = nodeInfo.get_all_keys_for_successor();

    if (keysAndValuesVector.size() == 0)
        return;

    string keysAndValues = "";

    /* will arrange all keys and val in form of key1:val1;key2:val2; */
    for (int i = 0; i < keysAndValuesVector.size(); i++)
    {
        keysAndValues += to_string(keysAndValuesVector[i].first) + ":" + keysAndValuesVector[i].second;
        keysAndValues += ";";
    }

    keysAndValues += "storeKeys";

    struct sockaddr_in serverToConnectTo;
    socklen_t l = sizeof(serverToConnectTo);

    help.setServerDetails(serverToConnectTo, succ.first.first, succ.first.second);

    int sock = socket(AF_INET, SOCK_DGRAM, 0);

    if (sock < 0)
    {
        perror("error");
        exit(-1);
    }

    char keysAndValuesChar[2000];
    strcpy(keysAndValuesChar, keysAndValues.c_str());

    sendto(sock, keysAndValuesChar, strlen(keysAndValuesChar), 0, (struct sockaddr *)&serverToConnectTo, l);

    close(sock);
}

/* perform different tasks according to received msg */
void doTask(NODE_INFORMATION &nodeInfo, int newSock, struct sockaddr_in client, string nodeIdString)
{

    /* predecessor of this node has left the ring and has sent all it's keys to this node(it's successor) */
    if (nodeIdString.find("storeKeys") != -1)
    {
        help.store_all_keys(nodeInfo, nodeIdString);
    }

    /* check if the sent msg is in form of key:val, if yes then store it in current node (for put ) */
    else if (help.isKeyValue(nodeIdString))
    {
        pair<lli, string> keyAndVal = help.GET_KEY_VAL(nodeIdString);
        nodeInfo.store_Key(keyAndVal.first, keyAndVal.second);
    }

    else if (nodeIdString.find("alive") != -1)
    {
        help.send_acknowledgement(newSock, client);
    }

    /* contacting node wants successor list of this node */
    else if (nodeIdString.find("sendSuccList") != -1)
    {
        help.send_succ_list(nodeInfo, newSock, client);
    }

    /* contacting node has just joined the ring and is asking for keys that belongs to it now */
    else if (nodeIdString.find("getKeys") != -1)
    {
        help.sendNecessaryKeys(nodeInfo, newSock, client, nodeIdString);
    }

    /* contacting node has run get command so send value of key it requires */
    else if (nodeIdString.find("k") != -1)
    {
        help.send_val_to_node(nodeInfo, newSock, client, nodeIdString);
    }

    /* contacting node wants the predecessor of this node */
    else if (nodeIdString.find("p") != -1)
    {
        help.send_predecessor(nodeInfo, newSock, client);

        /* p1 in msg means that notify the current node about this contacting node */
        if (nodeIdString.find("p1") != -1)
        {
            callNotify(nodeInfo, nodeIdString);
        }
    }

    /* contacting node wants successor Id of this node for help in finger table */
    else if (nodeIdString.find("finger") != -1)
    {
        help.send_successorid(nodeInfo, newSock, client);
    }

    /* contacting node wants current node to find successor for it */
    else
    {
        help.send_successor(nodeInfo, nodeIdString, newSock, client);
    }
}

/* listen to any contacting node */
void listenTo(NODE_INFORMATION &nodeInfo)
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

void doStabilize(NODE_INFORMATION &nodeInfo)
{

    /* do stabilize tasks */
    while (1)
    {

        nodeInfo.check_predecessor();

        nodeInfo.check_successor();

        nodeInfo.stabilise();

        nodeInfo.update_successor_list();

        nodeInfo.fix_finger_table();

        this_thread::sleep_for(chrono::milliseconds(300));
    }
}

/* call notify of current node which will notify curr node of contacting node */
void callNotify(NODE_INFORMATION &nodeInfo, string ipAndPort)
{

    ipAndPort.pop_back();
    ipAndPort.pop_back();

    /* get ip and port of client node */
    psi ipAndPortPair = help.GET_IP_PORT(ipAndPort);
    string ip = ipAndPortPair.first;
    int port = ipAndPortPair.second;
    lli hash = help.Get_hash(ipAndPort);

    ppsl node;
    node.first.first = ip;
    node.first.second = port;
    node.second = hash;

    /* notify current node about this node */
    nodeInfo.notify(node);
}

/* tell about all commands */
void showHelp()
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