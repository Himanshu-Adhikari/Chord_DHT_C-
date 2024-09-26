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
#include <unistd.h>

using namespace std;

    const int M = 8;

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
    Node_Information(string ip, int port){
        this->ip = ip;
        this->port = port;
    }

    // Concatenating IP and port so that this can be used for hashing
    string get_ip_port(){
        return ip + ":" + to_string(port);
    }
};

// Class to pair a serial_no with a node in the DHT ring
/*
Useful when status of a node changes and need to be maintained in the finger tables of other nodes
*/

class SerialNo_and_Node_Pair{
public:
    int serial_no;  // serial_no representing the position of the node in the DHT ring (e.g., hash value)
    Node* node;  // Pointer to the actual node object in the ring

    // Constructor to initialize the pair with a serial_no and node
    SerialNo_and_Node_Pair(int serial_no, Node* node){
        this->serial_no = serial_no;
        this->node = node;
    }

    int get_serialno(){
        return serial_no;
    }

    Node* get_node(){
        return node;
    }

    /*
    Sets the node to some other node or we can say when its status changes to like dead, not responding
    */
    void setNode(Node* newNode){
        node = newNode;
    }
};

class Finger_Table{
public:
    vector<SerialNo_and_Node_Pair*> f_table;
    int mod = 1 << M;

    /* Initializing Finger Table for a node */
    Finger_Table(int node_id){
        for(int i = 0; i < M; i++){
            int x = 1 << i;
            int entry = (node_id + x) % mod;
            Node* initialise_node = NULL;
            SerialNo_and_Node_Pair* snpair = new SerialNo_and_Node_Pair(entry, initialise_node);
            f_table.push_back(snpair);
        }
    }

    // Print FingerTable for a node
    void print_fingertable(){
        int id = 0;
        for(auto& c : f_table){
            cout << "SrNo. " << (id++) << " Starting Interval : " << c->get_serialno() << " Successor : " 
                 << (c->get_node() ? c->get_node()->id : -1) << endl;
        }
    }
};

/*
 * Class to handle requests and responsible for sending messages over a network
 * using a Datagram Socket (UDP).
 * 
 * Methods:
 * - sendMessage(ip, port, message): Sends a message to the provided address.
 * 
 * Example:
 * - Message Sent: "join_request|<node_id>" (Joining node sends a request to an
 * existing node to join the ring)
 * - Response Received:
 * "successor_info|<successor_id>|<successor_ip>|<successor_port>"
 */
class Request_Handler {
public:
    /**
     * Sends a message to a specified IP address and port using a Datagram socket (UDP).
     *
     * @param ip      The IP address of the destination.
     * @param port    The port number of the destination.
     * @param message The message to be sent.
     * @return The response received from the destination, or an empty string if an
     *         error occurs or no response is received.
     */
    string sendMessage(string ip, int port, string message) {
        int sock = 0;  // Socket descriptor
        struct sockaddr_in serv_addr;
        char buffer[1024] = {0};

        try {
            // Creating a UDP socket
            sock = socket(AF_INET, SOCK_DGRAM, 0);
            if (sock < 0) {
                throw runtime_error("Socket creation failed");
            }

            // Setting up the server address
            serv_addr.sin_family = AF_INET;
            serv_addr.sin_port = htons(port);

            // Converting IP address from text to binary form
            if (inet_pton(AF_INET, ip.c_str(), &serv_addr.sin_addr) <= 0) {
                throw runtime_error("Invalid address/Address not supported");
            }

            // Sending the message using sendto()
            if (sendto(sock, message.c_str(), message.length(), 0, 
                       (const struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
                throw runtime_error("Failed to send message");
            }

            // Receiving the response
            socklen_t len = sizeof(serv_addr);
            int valread = recvfrom(sock, buffer, sizeof(buffer), 0, 
                                   (struct sockaddr*)&serv_addr, &len);
            if (valread < 0) {
                throw runtime_error("Failed to receive response");
            }

            // Close the socket and return the response
            close(sock);
            return string(buffer, valread);

        } catch (const runtime_error& e) {
            cerr << "Error: " << e.what() << endl;
            if (sock > 0) {
                close(sock);  // Ensure socket is closed on error
            }
            return "";
        }
    }
};

class Node {
public:
    string ip;
    int port;
    int id;
    Node_Information* nodeInfo;
    Node* predecessor;
    Node* successor;
    Finger_Table* fingerTable;
    Node_data_storage* dataStore;
    Request_Handler* requestHandler;

    // Constructor
    Node(string ip, int port) : ip(ip), port(port) {
        // Create Node_Info
        nodeInfo = new Node_Information(ip, port);

        // Calculate the hash value of the Node_Info object
        id = hash(nodeInfo->get_ip_port());

        // Set predecessor and successor to nullptr
        predecessor = nullptr;
        successor = nullptr;

        // Initialize Finger Table with the node's ID
        fingerTable = new Finger_Table(id);

        // Initialize Data Store and Request Handler
        dataStore = new Node_data_storage();
        requestHandler = new Request_Handler();
    }

    // Destructor to clean up pointers
    ~Node() {
        delete nodeInfo;
        delete fingerTable;
        delete dataStore;
        delete requestHandler;
    }

    /**
     * Calculates the SHA-256 hash value of a given message and returns an integer.
     *
     * @param message The message to be hashed.
     * @return The integer hash value.
     */
    int hash(const string& message) {
        unsigned char digest[SHA256_DIGEST_LENGTH];  // SHA-256 generates 32 bytes (256 bits)
        SHA256_CTX sha256;
        SHA256_Init(&sha256);
        SHA256_Update(&sha256, message.c_str(), message.size());
        SHA256_Final(digest, &sha256);

        // Convert first 4 bytes of digest to an integer
        int result = 0;
        memcpy(&result, digest, sizeof(int));

        // Handle sign and modulo to ensure positive value within range
        return abs(result) % (int) pow(2,M);
    }
};
