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

// Actual IP and port data of the Node(client) in a DHT ring
class Node_Information
{
    string ip;
    int port;

public:
    Node_Information(string ip, int port)
    {
        this->ip = ip;
        this->port = port;
    }
    string get_ip() { return ip; }
    int get_port()
    {
        return port;
    }
    // Concatenating IP and port so that this can be used for hashing
    string get_ip_port()
    {
        return ip + ":" + to_string(port);
    }
};

// Class to pair a serial_no with a node in the DHT ring
/*
Useful when status of a node changes and need to be maintained in the finger tables of other nodes
*/

class SerialNo_and_Node_Pair
{
public:
    int serial_no; // serial_no representing the position of the node in the DHT ring (e.g., hash value)
    Node *node;    // Pointer to the actual node object in the ring

    // Constructor to initialize the pair with a serial_no and node
    SerialNo_and_Node_Pair(int serial_no, Node *node)
    {
        this->serial_no = serial_no;
        this->node = node;
    }

    int get_serialno()
    {
        return serial_no;
    }

    Node *get_node()
    {
        return node;
    }

    /*
    Sets the node to some other node or we can say when its status changes to like dead, not responding
    */
    void setNode(Node *newNode)
    {
        node = newNode;
    }
};

class Finger_Table
{
public:
    vector<SerialNo_and_Node_Pair *> f_table;
    int mod = 1 << M;

    /* Initializing Finger Table for a node */
    Finger_Table(int node_id)
    {
        for (int i = 0; i < M; i++)
        {
            int x = 1 << i;
            int entry = (node_id + x) % mod;
            Node *initialise_node = NULL;
            SerialNo_and_Node_Pair *snpair = new SerialNo_and_Node_Pair(entry, initialise_node);
            f_table.push_back(snpair);
        }
    }

    // Print FingerTable for a node
    void print_fingertable()
    {
        int id = 0;
        for (auto &c : f_table)
        {
            if (c->get_node())
            {
                cout << "SrNo. " << (id++) << " Starting Interval : " << c->get_serialno() << " Successor : "
                     << c->get_node()->get_serialno() << endl;
            }
            else
            {

                cout << "SrNo. " << (id++) << " Starting Interval : " << c->get_serialno() << " Successor : "
                     << "None" << endl;
            }
        }
    }
    string get_fingertable()
    {
        string fngr_table = "";
        for (int i = 0; i < f_table.size(); i++)
        {
            SerialNo_and_Node_Pair *pair = f_table[i]; // Access each SerialNo_and_Node_Pair in the table
            if (pair->get_node() == nullptr)           // Check if the node inside the pair is null
            {
                fngr_table += "Entry : " + to_string(i) + " Starting Interval : " + to_string(pair->get_serialno()) + " Successor: None\n";
            }
            else
            {
                fngr_table += "Entry : " + to_string(i) + " Starting Interval : " + to_string(pair->get_serialno()) + " Successor: " + to_string(pair->get_node()->id) + "\n";
            }
        }
        return fngr_table;
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
class Request_Handler
{
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
    string sendMessage(string ip, int port, string message)
    {
        int sock = 0; // Socket descriptor
        struct sockaddr_in serv_addr;
        char buffer[1024] = {0};

        try
        {
            // Creating a UDP socket
            sock = socket(AF_INET, SOCK_DGRAM, 0);
            if (sock < 0)
            {
                throw runtime_error("Socket creation failed");
            }

            // Setting up the server address
            serv_addr.sin_family = AF_INET;
            serv_addr.sin_port = htons(port);

            // Converting IP address from text to binary form
            if (inet_pton(AF_INET, ip.c_str(), &serv_addr.sin_addr) <= 0)
            {
                throw runtime_error("Invalid address/Address not supported");
            }

            // Sending the message using sendto()
            if (sendto(sock, message.c_str(), message.length(), 0,
                       (const struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
            {
                throw runtime_error("Failed to send message");
            }

            // Receiving the response
            socklen_t len = sizeof(serv_addr);
            int valread = recvfrom(sock, buffer, sizeof(buffer), 0,
                                   (struct sockaddr *)&serv_addr, &len);
            if (valread < 0)
            {
                throw runtime_error("Failed to receive response");
            }

            // Close the socket and return the response
            close(sock);
            return string(buffer, valread);
        }
        catch (const runtime_error &e)
        {
            cerr << "Error: " << e.what() << endl;
            if (sock > 0)
            {
                close(sock); // Ensure socket is closed on error
            }
            return "";
        }
    }
};

class Node
{
private:
    vector<string> split(const string &str, char delimiter)
    {
        vector<string> tokens;
        istringstream tokenStream(str);
        string token;
        while (getline(tokenStream, token, delimiter))
        {
            tokens.push_back(token);
        }
        return tokens;
    }

public:
    string ip;
    int port;
    int id;
    Node_Information *nodeInfo;
    Node *predecessor;
    Node *successor;
    Finger_Table *fingerTable;
    Node_data_storage *dataStore;
    Request_Handler *requestHandler;

    // Constructor
    Node(string ip, int port) : ip(ip), port(port)
    {
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
    ~Node()
    {
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
    int hash(const string &message)
    {
        unsigned char digest[SHA256_DIGEST_LENGTH]; // SHA-256 generates 32 bytes (256 bits)
        SHA256_CTX sha256;
        SHA256_Init(&sha256);
        SHA256_Update(&sha256, message.c_str(), message.size());
        SHA256_Final(digest, &sha256);

        // Convert first 4 bytes of digest to an integer
        int result = 0;
        memcpy(&result, digest, sizeof(int));

        // Handle sign and modulo to ensure positive value within range
        return abs(result) % (int)pow(2, M);
    }

    /*
    Process the incoming request message and performs the corressponding operation.
    * @param message the incoming request.
     */
    string processRequest(string message)
    {
        // Splitting the message to extract operation and arguments
        vector<string> parts;
        stringstream ss(message);
        string item;
        while (getline(ss, item, '|'))
        {
            parts.push_back(item);
        }

        // Extracting the operation from the message
        string operation = parts[0];

        // Extracting arguments from the message
        vector<string> args(parts.begin() + 1, parts.end());

        string result = "Done";

        // Handling different operations based on the message
        if (operation == "Insert_Server")
        {
            vector<string> data;
            stringstream dataStream(parts[1]);
            while (getline(dataStream, item, ':'))
            {
                data.push_back(item);
            }

            string key = data[0];
            string value = data[1];
            this->dataStore->insert_key(key, value);
            result = "Inserted";
        }
        else if (operation == "Delete_Server")
        {
            string dataToDelete = parts[1];
            this->dataStore->delete_key(dataToDelete);
            result = "Deleted";
        }
        else if (operation == "Search_Server")
        {
            string searchData = parts[1];
            string searchResult = this->dataStore->Search_key(searchData);
            if (!searchResult.empty())
            {
                return searchResult;
            }
            else
            {
                return "NOT FOUND";
            }
        }
        else if (operation == "Send_Keys")
        {
            int idOfJoiningNode = stoi(args[0]);
            result = sendKeys(idOfJoiningNode);
        }
        else if (operation == "Insert")
        {
            vector<string> insertData;
            stringstream insertStream(parts[1]);
            while (getline(insertStream, item, ':'))
            {
                insertData.push_back(item);
            }

            string insertKey = insertData[0];
            string insertValue = insertData[1];
            result = this->insertKey(insertKey, insertValue);
        }
        else if (operation == "Delete")
        {
            string deleteKey = parts[1];
            result = this->deleteKey(deleteKey);
        }
        else if (operation == "Search")
        {
            string searchKey = parts[1];
            result = this->searchKey(searchKey);
        }
        else if (operation == "Join_Request")
        {
            int nodeId = stoi(args[0]);
            result = this->joinRequestFromOtherNode(nodeId);
        }
        else if (operation == "Find_Predecessor")
        {
            int searchID = stoi(args[0]);
            result = this->findPredecessor(searchID);
        }
        else if (operation == "Find_Successor")
        {
            int searchID = stoi(args[0]);
            result = this->findSuccessor(searchID);
        }
        else if (operation == "Get_Successor")
        {
            result = this->getSuccessor();
        }
        else if (operation == "Get_Predecessor")
        {
            result = this->getPredecessor();
        }
        else if (operation == "Get_Id")
        {
            result = to_string(this->getId());
        }
        else if (operation == "Get_Finger_Table")
        {
            result = this->fingerTable->get_fingertable();
        }
        else if (operation == "Get_Data_Store")
        {
            result = this->dataStore->get_data_as_string();
        }
        else if (operation == "Get_Info")
        {
            result = this->ip + "/" + to_string(this->port) + "/" + to_string(this->id) + "/" +
                     (this->predecessor ? to_string(this->predecessor->id) : "None") + "/" +
                     (this->successor ? to_string(this->successor->id) : "None");
        }
        else if (operation == "Notify")
        {
            int nodeID = stoi(args[0]);
            string nodeIP = args[1];
            int nodePort = stoi(args[2]);
            this->notify(nodeID, nodeIP, nodePort);
        }

        return result;
    }

    void serveRequests(int clientSocket, sockaddr_in clientAddr)
    {
        try
        {
            char buffer[1024];
            memset(buffer, 0, sizeof(buffer));

            // Reading the incoming message
            int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
            if (bytesRead < 0)
            {
                throw runtime_error("Error reading from socket");
            }

            string data(buffer);
            data = data.substr(0, bytesRead); // Trim to the number of bytes read

            // Processing the request and getting the result
            string result = this->processRequest(data);

            // Writing the result to the output stream
            send(clientSocket, result.c_str(), result.size(), 0);
        }
        catch (const exception &e)
        {
            cerr << "Error occurred while serving request: " << e.what() << endl;
        }
        // Closing the connection
        close(clientSocket);
    }
    void start()
    {
        int serverSocket;
        struct sockaddr_in serverAddr;

        // Create a server socket
        serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket < 0)
        {
            cerr << "Error creating socket" << endl;
            return;
        }

        // Set socket options
        int opt = 1;
        setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        // Initialize server address
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(nodeInfo->get_port());
        inet_pton(AF_INET, nodeInfo->get_ip().c_str(), &serverAddr.sin_addr);

        // Bind the server socket
        if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
        {
            cerr << "Error binding socket" << endl;
            close(serverSocket);
            return;
        }

        // Listen for incoming connections
        listen(serverSocket, 50);

        // Starting a new thread for stabilizing the node
        thread threadForStabilize(&Node::stabilize, this);
        threadForStabilize.detach(); // Detach to run independently

        // Starting a new thread for fixing fingers of the node
        thread threadForFixFinger(&Node::fixFingers, this);
        threadForFixFinger.detach(); // Detach to run independently

        // Continuously accepting incoming connections
        while (true)
        {
            socklen_t addrLen = sizeof(sockaddr_in);
            struct sockaddr_in clientAddr;
            int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &addrLen);
            if (clientSocket < 0)
            {
                cerr << "Error accepting connection" << endl;
                continue; // Continue to accept other connections
            }

            // Start a new thread to serve the incoming request
            thread t(&Node::serveRequests, this, clientSocket, clientAddr);
            t.detach(); // Detach to run independently
        }

        close(serverSocket); // Close the server socket when done (although this point will never be reached)
    }

    string insertKey(const string &key, const string &value)
    {
        try
        {
            // Calculate the hash value of the key
            int idOfKey = hash(key);

            // Find the successor node responsible for the key's hash value
            string succ = findSuccessor(idOfKey);

            // Extract IP address and port number of the successor node
            string ipPort = getIpPort(succ);
            if (ipPort == "Invalid IP:Invalid Port")
            {
                throw runtime_error("Invalid Port information received");
            }

            string ip = ipPort.substr(0, ipPort.find(':'));
            int port = stoi(ipPort.substr(ipPort.find(':') + 1));

            // Send a message to the successor node to insert the key-value pair
            requestHandler->sendMessage(ip, port, "Insert_Server|" + key + ":" + value);

            // Return a message indicating successful insertion
            return "Inserted at node id " + to_string(Node(ip, port).id) +
                   " key was " + key + " key hash was " + to_string(idOfKey);
        }
        catch (const invalid_argument &e)
        {
            // Handle any number format exceptions (e.g., parsing port number)
            cerr << e.what() << endl;
            return "Error inserting key: " + string(e.what());
        }
        catch (const runtime_error &e)
        {
            cerr << e.what() << endl;
            return "Error inserting key: " + string(e.what());
        }
    }

    string deleteKey(const string &key)
    {
        try
        {
            // Calculate the hash value of the key
            int idOfKey = hash(key);
            // Find the successor node responsible for the key
            string succ = findSuccessor(idOfKey);
            // Get the IP address and port of the successor node
            string ipPort = getIpPort(succ);
            // Error handling for invalid IP and port
            if (ipPort == "Invalid IP:Invalid Port")
            {
                throw runtime_error("Invalid Port information received");
            }

            string ip = ipPort.substr(0, ipPort.find(':'));
            int port = stoi(ipPort.substr(ipPort.find(':') + 1));

            // Send a delete request to the successor node
            requestHandler->sendMessage(ip, port, "Delete_Server|" + key);

            // Return a message confirming the deletion
            return "deleted at node id " + to_string(Node(ip, port).id) +
                   " key was " + key + " key hash was " + to_string(idOfKey);
        }
        catch (const runtime_error &e)
        {
            cerr << e.what() << endl;
            return "Error deleting key: " + string(e.what());
        }
    }

    string searchKey(const string &key)
    {
        try
        {
            // Calculate the hash value of the key
            int idOfKey = hash(key);
            // Find the successor node responsible for the key
            string succ = findSuccessor(idOfKey);
            // Get the IP address and port of the successor node
            string ipPort = getIpPort(succ);
            // Error handling for invalid IP and port
            if (ipPort == "Invalid IP:Invalid Port")
            {
                throw runtime_error("Invalid Port information received");
            }

            string ip = ipPort.substr(0, ipPort.find(':'));
            int port = stoi(ipPort.substr(ipPort.find(':') + 1));

            // Send a search request to the successor node and get the response
            string data = requestHandler->sendMessage(ip, port, "Search_Server|" + key);
            // Return the data received from the successor node
            return data;
        }
        catch (const runtime_error &e)
        {
            cerr << e.what() << endl;
            return "Error searching key: " + string(e.what());
        }
    }
    // Handles a join request from another node by finding its successor.
    string joinRequestFromOtherNode(int nodeId)
    {
        return findsuccessor(nodeId);
    }

    void join(const string &nodeIp, int nodePort)
    {
        try
        {
            // Check if the requestHandler is initialized
            if (requestHandler == nullptr)
            {
                throw runtime_error("requestHandler is not initialized");
            }

            // Prepare the message to send for joining
            string data = "Join_Request|" + to_string(this->id);
            // Send the join request to the existing node and get the successor information
            string succ = requestHandler->sendMessage(nodeIp, nodePort, data);

            // Check if successor information is received successfully
            if (succ.empty() || succ == "None")
            {
                throw runtime_error("Failed to get successor information");
            }

            // Extract the IP address and port of the successor node
            string ipPort = getIpPort(succ);
            // Error Handling for invalid ip and port
            if (ipPort == "Invalid IP:Invalid Port")
            {
                throw runtime_error("Invalid Port information received");
            }

            // Set the successor node and update the finger table
            string ip = ipPort.substr(0, ipPort.find(':'));
            int port = stoi(ipPort.substr(ipPort.find(':') + 1));
            this->successor = new Node(ip, port);
            this->fingerTable->f_table[0].setNode(this->successor);
            this->predecessor = NULL; // Assuming Node() initializes to a null state

            if (this->successor->id != this->id)
            {
                // Retrieve keys from the successor node
                data = requestHandler->sendMessage(this->successor->ip, this->successor->port, "Send_Keys|" + to_string(this->id));
                if (data.empty())
                {
                    cerr << "No keys received from the successor" << endl;
                }
                else
                {
                    // Parse and store the received key-value pairs
                    vector<string> key_values = split(data, ':');
                    for (const auto &key_value : key_values)
                    {
                        if (key_value.length() > 1)
                        {
                            vector<string> parts = split(key_value, '|');
                            if (parts.size() >= 2)
                            {
                                this->dataStore->data[parts[0]] = parts[1];
                            }
                            else
                            {
                                cerr << "Invalid key-value pair: " << key_value << endl;
                            }
                        }
                    }
                }
            }
        }
        catch (const runtime_error &e)
        {
            cerr << "Error: " << e.what() << endl;
        }
        catch (const exception &e)
        {
            cerr << "Unexpected error occurred: " << e.what() << endl;
        }
    }
    /**
     * Finds the predecessor node responsible for the given key.
     *
     * @param searchId The ID of the key for which the predecessor node is searched.
     * @return The information about the predecessor node.
     */

    string findPredecessor(int searchId)
    {
        try
        {
            // If the current node is the predecessor
            if (searchId == this->id)
            {
                return nodeInfo->get_ip_port();
            }
            // If the current node is the only node in the network
            if (predecessor->id != -1 && successor->id == this->id)
            {
                return nodeInfo->get_ip_port();
            }
            // If the successor of the current node is the closest node to the key
            if (successor->id != -1 && getForwardDistance(successor->id) > getForwardDistance(searchId))
            {
                return nodeInfo->get_ip_port();
            }
            else
            {
                // Recursively find the predecessor node on the routing path
                Node newHopNode = closestPrecedingNode(searchId);
                if (newHopNode.id == -1)
                {
                    return "None";
                }
                // Extract IP and port of the new hop node
                string ipPort = getIpPort(newHopNode.nodeInfo);
                // Error Handling for invalid ip and port
                if (ipPort == "Invalid IP|Invalid Port")
                {
                    throw runtime_error("Invalid Port information received");
                }
                // Split the ipPort string to get individual IP and port
                size_t pos = ipPort.find('|');
                string newIp = ipPort.substr(0, pos);
                int newPort = stoi(ipPort.substr(pos + 1));

                // If the new hop node is the current node, return its information
                if (newIp == this->ip && newPort == this->port)
                {
                    return nodeInfo->get_ip_port();
                }
                // Send request to the new hop node to find the predecessor node
                return requestHandler->sendMessage(newIp, newPort, "Find_Predecessor|" + to_string(searchId));
            }
        }
        catch (const runtime_error &e)
        {
            cerr << "Error inserting key: " << e.what() << endl;
            return "Error inserting key: " + string(e.what());
        }
        catch (const exception &e)
        {
            cerr << "Error occurred: " << e.what() << endl;
            return "Error";
        }
    }
    string findSuccessor(int searchId)
    {
        try
        {
            // If the current node is responsible for the key, return its information
            if (searchId == this->id)
            {
                return nodeInfo->get_ip_port();
            }
            // Find the predecessor node for the given key
            string predecessorInfo = findPredecessor(searchId);
            // If no predecessor node found, return "None"
            if (predecessorInfo == "None")
            {
                return "None";
            }
            else
            {
                // Extract IP and port of the predecessor node
                string ipPort = getIpPort(predecessorInfo);
                // Error Handling for invalid ip and port
                if (ipPort == "Invalid IP|Invalid Port")
                {
                    throw runtime_error("Invalid Port information received");
                }
                // Split the ipPort string to get individual IP and port
                size_t pos = ipPort.find('|');
                string newIp = ipPort.substr(0, pos);
                int newPort = stoi(ipPort.substr(pos + 1));

                // Send request to the predecessor node to get its successor
                string data = requestHandler->sendMessage(newIp, newPort, "Get_Successor");
                return data;
            }
        }
        catch (const runtime_error &e)
        {
            cerr << "Error inserting key: " << e.what() << endl;
            return "Error inserting key: " + string(e.what());
        }
        catch (const exception &e)
        {
            cerr << "Error occurred: " << e.what() << endl;
            return "Error";
        }
    }

    Node * closestPrecedingNode(int searchId)
    {
        Node * closestNode=NULL;
        int minDistance=(1<<M)+1;
        try
        {
            // Iterate through the finger table entries to find the closest preceding node
            for (int i = M - 1; i >= 0; i--)
            {
                SerialNo_and_Node_Pair en_pair = fingerTable->f_table[i];
                Node * node = en_pair.getNode();

                // Update the closest node if the current node is closer to the key
                if (node->id != -1 && getForwardDistance2Nodes(node->id, searchId) < minDistance)
                { // Assuming a sentinel value for invalid nodes
                    closestNode = node;
                    minDistance = getForwardDistance2Nodes(node->id, searchId);
                }
            }
        }
        catch (const nodeInfo->get_ip_port();exception &e)
        {
            // Handle any exceptions that occur during the iteration
            nodeInfo->get_ip_port();cerr << "Error occurred: " << e.what() << nodeInfo->get_ip_port();endl;
        }

        return closestNode; // This may need to return an optional or a special value if no closest node is found
    }
};
