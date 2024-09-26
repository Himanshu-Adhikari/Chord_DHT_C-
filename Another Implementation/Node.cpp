#include <bits/stdc++.h>
// using bits/stdc++.h for the commented ones
//  #include <iostream>
//  #include <fstream>
//  #include <vector>
//  #include <map>
//  #include <string>
//  #include <cstring>
#include <openssl/sha.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdexcept>

using namespace std;


// To store data at a client(node) in a CHORD DHT ring
class Node_data_storage
{
public:
    map<string, string> data;

    void insert_key(string key, string value)
    {
        data[key] = value;
    }
    void delete_key(string key)
    {
        data.erase(key);
    }

    string Search_key(string key)
    {
        if (data.count(key))
        {
            return data[key];
        }
        else
        {
            cout << "Key not Found" << endl;
            return "-1";
        }
    }
};

//Actual IP and port data of the Node(client) in a DHT ring
class Node_Information
{
    string ip;
    int port;

public:
    Node_Information(string ip,int port){
        this->ip=ip;
        this->port=port;
    }

    // Concatenating IP and port so that this can be used for hashing
    string get_ip_port(){
        return ip+":"+to_string(port);
    }
};

// Class to pair an serial_no with a node in the DHT ring
/*
Useful when status of a node changes and need to be maintained in the finger tables of other nodes
*/
class SerialNo_and_Node_Pair{
public:
    int serial_no;  // serial_no representing the position of the node in the DHT ring (e.g., hash value)
    Node node;  // Reference to the actual node object in the ring

    // Constructor to initialize the pair with an serial_no and node
    SerialNo_and_Node_Pair(int serial_no, Node node){
        this->serial_no = serial_no;
        this->node = node;
    }

    int get_serialno(){
        return serial_no;
    }

    Node get_node(){
        return node;
    }

    /*
    Sets the node to some other node or we can say when its status changes to like dead,not responding
    */
    void setNode(Node newNode){
        node=newNode;
    }
};



// Placeholder for Node class
class Node{
public:

};
