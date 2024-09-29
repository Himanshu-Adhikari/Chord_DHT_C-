#include "all_header.h"
#include "HelperClass.h"
mutex mt;

/* Split command into separate arguments using stringstream */
vector<string> Helper_Functions::splitCommand(string cmd)
{
    vector<string> arg;
    stringstream ss(cmd);
    string s;
    while (ss >> s)
        arg.push_back(s);
    return arg;
}

/* Use SHA1 for hashing a key */
lli Helper_Functions::getHash(string key)
{
    unsigned char output_buffer[41];
    char FinalHash[41];
    string Hashed_key = "";

    lli mod = pow(2, nodes_count);

    /* convert string to an unsigned char array because SHA1 takes unsigned char array as parameter */
    unsigned char unsigned_key[key.length() + 1];
    for (int i = 0; i < key.length(); i++)
    {
        unsigned_key[i] = key[i];
    }

    SHA1(unsigned_key, sizeof(unsigned_key), output_buffer);

    for (int i = 0; i < nodes_count / 8; i++)
    {
        sprintf(FinalHash, "%d", output_buffer[i]);
        Hashed_key += FinalHash;
    }
    return stoll(Hashed_key) % mod;
}

pair<string, int> Helper_Functions::Get_Ip_and_Port(string key)
{
    int id = key.find(':');
    string ip = key.substr(0, id), port = key.substr(id + 1);

    pair<string, int> ip_and_port = {ip, atoi(port.c_str())};

    return ip_and_port;
}

pair<lli, string> Helper_Functions::getKeyAndVal(string key_val)
{
    int id = key_val.find(':');
    string key = key_val.substr(0, id), val = key_val.substr(id + 1);
    pair<lli, string> key_val_pair = {stoll(key), val};
    return key_val_pair;
}

bool Helper_Functions::isKeyValue(string cast)
{
    int id = cast.find(':');
    if (id == -1)
        return false;
    for (int i = 0; i < id; i++)
    {
        if (!(cast[i] >= '0' && cast[i] <= '9'))
            return false;
    }
    return true;
}

string Helper_Functions::getKeyFromNode(ppsl node, string hashedkey)
{
    auto [ip, port] = node.first;
    struct sockaddr_in serverToconnetTo;
    socklen_t len = sizeof(serverToconnetTo);
    setServerDetails(serverToconnetTo, ip, port);

    int sockt = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockt < 0)
    {
        perror("error");
        exit(-1);
    }

    hashedkey += "k";
    char KeyHashChar[40];
    strcpy(KeyHashChar, hashedkey.c_str());

    sendto(sockt, KeyHashChar, strlen(KeyHashChar), 0, (struct sockaddr *)&serverToconnetTo, len);

    char valChar[100];
    int len = recvfrom(sockt, valChar, 1024, 0, (struct sockaddr *)&serverToconnetTo, &len);

    valChar[len] = '\0';
    string res = valChar;
    close(sockt);
    return res;
}

/* set details of server to which you want to connect to */
void Helper_Functions::setServerDetails(struct sockaddr_in &server, string ip, int port)
{
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(ip.c_str());
}

// Send Key to node who asked for it
void Helper_Functions::sendKeyToNode(ppsl node, lli keyhash, string value)
{
    auto [ip, port] = node.first;

    struct sockaddr_in serverToconnetTo;
    socklen_t len = sizeof(serverToconnetTo);
    setServerDetails(serverToconnetTo, ip, port);

    int sockt = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockt < 0)
    {
        perror("error");
        exit(-1);
    }

    string Key_and_Val = Combine_Ip_and_Port(to_string(keyhash), value);

    char keyValChar[100];
    strcpy(keyValChar, Key_and_Val.c_str());

    sendto(sockt, keyValChar, strlen(keyValChar), 0, (struct sockaddr *)&serverToconnetTo, len);

    close(sockt);
}

// A newly joined node asks its successor for the keys that now belongs to the new node

void Helper_Functions::getKeysFromSuccessor(Node_information &nodeinfo, string ip, int port)
{
    struct sockaddr_in serverToConnecto;
    socklen_t len = sizeof(serverToConnecto);

    setServerDetails(serverToConnecto, ip, port);

    int sockt = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockt < 0)
    {
        perror("error");
        exit(-1);
    }

    string id = to_string(nodeinfo.getId());

    string msg = "getKeys:" + id;

    char msgChar[40];
    strcpy(msgChar, msg.c_str());

    sendto(sockt, msgChar, strlen(msgChar), 0, (struct sockaddr *)&serverToConnecto, 1);

    char keysAndValuesChar[2000];
    int c_len = recvfrom(sockt, keysAndValuesChar, 2000, 0, (struct sockaddr *)&serverToConnecto, &len);

    keysAndValuesChar[c_len] = '\0';

    close(sockt);

    string KeysandValues = keysAndValuesChar;

    vector<pair<lli, string>> KeysandValuesvector = Separate_Keys_and_Values(KeysandValues);

    for (auto &[key, val] : KeysandValuesvector)
    {
        nodeinfo.StoreKeys(key, val);
    }
}

/*  key1:value1;key2:value2;.. to {{key_i,val_i}}*/
vector<pair<lli, string>> Helper_Functions::Separate_Keys_and_Values(string key_values)
{
    vector<pair<lli, string>> res;
    int n = key_values.size(), i = 0;
    while (i < n)
    {
        string key = "", val = "";
        while (i < n && key_values[i] != ':')
        {
            key += key_values[i++];
        }
        i++;
        while (i < n && key_values[i] != ';')
        {
            val += key_values[i++];
        }
        i++;
        res.push_back({stoll(key), val});
    }
    return res;
}
/* ip_1:port_1;ip_2:port_2... to vector<pair>> form */
vector<pair<string, int>> Helper_Functions::SeparateSuccessorList(string successor_list)
{
    vector<pair<string, int>> res;
    int n = successor_list.size(), i = 0;
    while (i < n)
    {
        string ip = "", port = "";
        while (i < n && successor_list[i] != ':')
        {
            ip += successor_list[i++];
        }
        i++;
        while (i < n && successor_list[i] != ';')
        {
            port += successor_list[i++];
        }
        i++;
        res.push_back({ip, stoi(port)});
    }
    return res;
}

// Combine ip and port as one string

string Helper_Functions::Combine_Ip_and_Port(string ip, string port)
{
    string res = "";
    res += ip;
    res += ":";
    res += port;
    return res;
}

// Node receives all keys from its predecing node who leaves the ring

void Helper_Functions::storeAllKeys(Node_information &nodeinfo, string keys_values)
{
    int id = keys_values.find("storeKeys");
    vector<pair<lli, string>> res = Separate_Keys_and_Values(keys_values.substr(0, id));
    for (auto [key, val] : res)
    {
        nodeinfo.StoreKeys(key, val);
    }
}

// Give all keys that belong to the newly joined node
void Helper_Functions::sendNecessaryKeys(Node_information &nodeinfo, int new_socket, struct sockaddr_in client, string nodeidString)
{
    socklen_t len = sizeof(client);
    int id = nodeidString.find(":");
    lli nodeId = stoll(nodeidString.substr(id + 1));

    vector<pair<lli, string>> key_val_vector = nodeinfo.getAllKeysFromPredecessor(nodeId);

    string key_and_values = "";
    for (auto [key, val] : key_val_vector)
    {
        key_and_values += to_string(key) + ":" + val + ";";
    }
    char KeyAndValuesChar[2000];
    strcpy(KeyAndValuesChar, key_and_values.c_str());

    sendto(new_socket, KeyAndValuesChar, strlen(KeyAndValuesChar), 0, (struct sockaddr *)&client, len);
}

void Helper_Functions::sendValToNode(Node_information nodeinfo, int new_socket, struct sockaddr_in client, string nodeIdString)
{
    nodeIdString.pop_back();
    lli key = stoll(nodeIdString);

    string val = nodeinfo.getValues(key);
    socklen_t len = sizeof(client);

    char valChar[100];
    strcpy(valChar, val.c_str());
    sendto(new_socket, valChar, strlen(valChar), 0, (struct sockaddr *)&client, len);
}

/* send Successor id of current node to the contacting node */

void Helper_Functions::sendSuccessorId(Node_information nodeinfo, int new_socket, struct sockaddr_in client)
{
    ppsl successor = nodeinfo.getSuccessor();
    string successor_id = to_string(successor.second);
    char succIdChar[40];

    socklen_t len = sizeof(client);
    strcpy(succIdChar, successor_id.c_str());
    sendto(new_socket, succIdChar, strlen(succIdChar), 0, (struct sockaddr *)&client, len);
}

/* Find successor of contacting node and send its ip:port to it */

void Helper_Functions::sendSuccessor(Node_information nodeinfo, string nodeidstring, int new_socket, struct sockaddr_in client)
{
    lli nodeid = stoll(nodeidstring);
    socklen_t len = sizeof(client);

    ppsl successor_node = nodeinfo.getSuccessor();

    /* get Ip and port of successor as ip:port in char array to send */
    char ip_port[40];
    auto [successor_ip,successor_port]=successor_node.first;

    strcpy(ip_port,Combine_Ip_and_Port(successor_ip,to_string(successor_port)).c_str());

    //send ip-port to respective node
    sendto(new_socket,ip_port,strlen(ip_port),0,(struct sockaddr*)&client,len);
}

 /* Find predecessor of contacting node and send its ip:port to it */
 
 void Helper_Functions::sendPredecessor(Node_information nodeinfo,int new_socket,struct sockaddr_in client){
    ppsl predecessor =nodeinfo.getPredeccessor();
    auto [ip,port]=predecessor.first;
    socklen_t len=sizeof(client);

    //if no predecessor
    if(ip==""){
        sendto(new_socket,"",0,0,(struct sockaddr *)&client,len);
    }
    else{
        string ip_port=Combine_Ip_and_Port(ip,to_string(port));
        char ip_port_char[40];
        strcpy(ip_port_char,ip_port.c_str());

        sendto(new_socket,ip_port_char,strlen(ip_port_char),0,(struct sockaddr *)&client,len);
    }
 }

 /*get successorId of a node */

 lli Helper_Functions::getSuccessorId(string ip,string port){
    struct sockaddr_in serverToConnectTo;
    socklen_t len=sizeof(serverToConnectTo);
    
    
 }