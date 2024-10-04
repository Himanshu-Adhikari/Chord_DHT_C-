#include "Helper_class.h"
mutex mt; // Mutex for thread safety

// Split a command string into individual arguments
vector<string> Helper_Class::SplitCommands(string cmd)
{
    vector<string> res; // Vector to hold the split arguments
    int indx = 0;       // Index for finding spaces
    do
    {
        // Find the index of the next space in the command string
        indx = cmd.find(' ');

        // Extract the argument from the start of the string to the found index
        string arg = cmd.substr(0, indx);
        res.push_back(arg); // Add the argument to the result vector

        // Update the command string to remove the processed argument
        cmd = cmd.substr(indx + 1);
    } while (indx != -1); // Continue until no spaces are found
    return res; // Return the vector of split arguments
}

// Compute the hash for a given key
lli Helper_Class::Get_hash(string key)
{
    unsigned char obuf[41];  // Output buffer for the SHA1 hash
    char finalHash[41];      // Buffer to store the final hash as a string
    string keyHash = "";     // String to accumulate the hash value
    int i;                   // Loop counter
    lli mod = pow(2, nodes); // Modulus for hash value

    // Convert the string key to an unsigned char array (needed for SHA1)
    unsigned char unsigned_key[key.length() + 1]; // Array size is key length + 1 for null terminator
    for (i = 0; i < key.length(); i++)
    {
        unsigned_key[i] = key[i]; // Copy each character to the unsigned array
    }
    unsigned_key[i] = '\0'; // Null terminate the array

    // Compute the SHA1 hash of the key
    SHA1(unsigned_key, sizeof(unsigned_key), obuf);

    // Convert the hash bytes to a string representation
    for (i = 0; i < nodes / 8; i++)
    {
        sprintf(finalHash, "%d", obuf[i]); // Convert each byte to a string
        keyHash += finalHash;              // Append to the keyHash string
    }

    // Compute the final hash by taking the modulus
    lli hash = stoll(keyHash) % mod;

    return hash; // Return the computed hash
}
// Parse a string containing an IP address and port number
psi Helper_Class::GET_IP_PORT(string ip_port)
{
    // Find the position of the colon separating the IP and port
    int pos = ip_port.find(':');
    // Extract the IP address from the start of the string to the colon position
    string ip = ip_port.substr(0, pos);
    // Extract the port number from the position after the colon to the end of the string
    string port = ip_port.substr(pos + 1);

    psi res;                         // Pair to hold the IP and port
    res.first = ip;                  // Set the first element of the pair to the IP
    res.second = atoi(port.c_str()); // Convert the port string to an integer and set it as the second element

    return res; // Return the pair containing IP and port
}

// Parse a string containing a key-value pair
pls Helper_Class::GET_KEY_VAL(string key_val)
{
    // Find the position of the colon separating the key and value
    int pos = key_val.find(':');
    // Extract the key from the start of the string to the colon position
    string key = key_val.substr(0, pos);
    // Extract the value from the position after the colon to the end of the string
    string val = key_val.substr(pos + 1);

    pls res;                // Pair to hold the key (as a long long int) and value (as a string)
    res.first = stoll(key); // Convert the key string to a long long int and set it as the first element
    res.second = val;       // Set the second element of the pair to the value

    return res; // Return the pair containing key and value
}

// Check if a string represents a valid key-value pair
bool Helper_Class::isKeyValue(string current_node_id)
{
    // Find the position of the colon in the string
    int pos = current_node_id.find(":");

    // If no colon is found, return false (not a key-value pair)
    if (pos == -1)
        return false;

    // Check if all characters before the colon are digits
    for (int i = 0; i < pos; i++)
    {
        // If any character is not a digit (ASCII values 48-57), return false
        if (!(current_node_id[i] >= 48 && current_node_id[i] <= 57))
            return false;
    }

    return true; // If all checks pass, return true (valid key-value format)
}

/* Will contact a node and get the value of a particular key from that node */
string Helper_Class::getKeyFromNode(ppsl node, string keyHash)
{
    // Extract IP address and port number from the node pair
    string ip = node.first.first;
    int port = node.first.second;

    struct sockaddr_in Connecting_Server;    // Structure to hold server address details
    socklen_t l = sizeof(Connecting_Server); // Length of the address structure

    // Set the details for the server to connect to
    setServerDetails(Connecting_Server, ip, port);

    // Create a UDP socket
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        perror("error"); // Print an error message if socket creation fails
        exit(-1);        // Exit the program on error
    }

    keyHash += "k"; // Append 'k' to the keyHash string to indicate a request

    char keyHashChar[40];                 // Buffer to hold the keyHash as a C-string
    strcpy(keyHashChar, keyHash.c_str()); // Copy the string into the char array

    // Send the keyHash to the specified node
    sendto(sock, keyHashChar, strlen(keyHashChar), 0, (struct sockaddr *)&Connecting_Server, l);

    char valChar[100]; // Buffer to receive the value from the node
    // Receive the response from the node
    int len = recvfrom(sock, valChar, 1024, 0, (struct sockaddr *)&Connecting_Server, &l);

    valChar[len] = '\0'; // Null terminate the received string

    string val = valChar; // Convert the char array to a string

    close(sock); // Close the socket to release the resource

    return val; // Return the retrieved value
}

/* Set details of the server to which you want to connect */
void Helper_Class::setServerDetails(struct sockaddr_in &server, string ip, int port)
{
    server.sin_family = AF_INET;                    // Set address family to IPv4
    server.sin_addr.s_addr = inet_addr(ip.c_str()); // Convert IP string to binary form
    server.sin_port = htons(port);                  // Convert port number to network byte order
}

// Function to send a key and its value to a specific node over UDP
void Helper_Class::Send_Key_to_Node(ppsl node, lli keyhash, string val)
{
    // Extract the IP address and port of the node from the 'node' parameter
    string ip = node.first.first;
    int port = node.first.second;

    // Initialize a sockaddr_in structure for the server (node) to connect to
    struct sockaddr_in Connecting_Server;
    socklen_t l = sizeof(Connecting_Server);

    // Set server details (IP and port) into the sockaddr_in structure
    setServerDetails(Connecting_Server, ip, port);

    // Create a UDP socket (SOCK_DGRAM indicates a UDP socket)
    int sock = socket(AF_INET, SOCK_DGRAM, 0);

    // Check if the socket creation was successful
    if (sock < 0)
    {
        // If socket creation fails, print the error and exit
        perror("error");
        exit(-1);
    }

    // Combine the key's hash and the value into a single string for sending
    string keyAndVal = combine_ip_port(to_string(keyhash), val);

    // Convert the combined string into a character array for sending via the socket
    char keyAndValChar[100];
    strcpy(keyAndValChar, keyAndVal.c_str());

    // Send the key-value pair to the specified node using sendto()
    sendto(sock, keyAndValChar, strlen(keyAndValChar), 0, (struct sockaddr *)&Connecting_Server, l);

    // Close the socket after sending the data to free resources
    close(sock);
}

// Function to request keys from a successor node in the Chord DHT
void Helper_Class::Get_Keys_from_successor(NODE_INFORMATION &nodeinfo, string ip, int port)
{
    // Initialize a sockaddr_in structure for the server (successor node)
    struct sockaddr_in Connecting_Server;
    socklen_t len = sizeof(Connecting_Server);

    // Set server details (IP and port) for connecting to the successor node
    setServerDetails(Connecting_Server, ip, port);

    // Create a UDP socket (SOCK_DGRAM indicates a UDP socket)
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        // If socket creation fails, print the error and exit
        perror("error");
        exit(-1);
    }

    // Prepare a message to request the keys, formatted as "getKeys:<current_node_id>"
    string current_node_id = to_string(nodeinfo.get_nodeid());
    string msg = "getKeys:" + current_node_id;

    // Convert the message to a character array for sending via the socket
    char msgChar[40];
    strcpy(msgChar, msg.c_str());

    // Send the message requesting keys to the successor node
    sendto(sock, msgChar, strlen(msgChar), 0, (struct sockaddr *)&Connecting_Server, len);

    // Buffer to receive the keys and values from the successor node
    char key_val_char[2000];

    // Receive the keys and values from the successor node
    int ln = recvfrom(sock, key_val_char, 2000, 0, (struct sockaddr *)&Connecting_Server, &len);

    // Null-terminate the received string
    key_val_char[ln] = '\0';

    // Close the socket after receiving the data
    close(sock);

    // Convert the received keys and values character array into a string
    string key_val = key_val_char;

    // Separate the keys and values into a vector of pairs
    vector<pls> key_val_vec = separate_KEY_VAL(key_val);

    // Store the received keys and their corresponding values in the node's storage
    for (int i = 0; i < key_val_vec.size(); i++)
    {
        nodeinfo.store_Key(key_val_vec[i].first, key_val_vec[i].second);
    }
}
/*
 * Function to separate key-value pairs from a formatted string.
 * The input string is formatted as "key1:val1;key2:val2;..."
 * This function splits the string into individual key-value pairs and stores them in a vector of pairs.
 */
vector<pls> Helper_Class::separate_KEY_VAL(string key_val)
{
    int size = key_val.size(); // Get the size of the input string
    int i = 0;                 // Initialize index to traverse the string
    vector<pls> res;           // Result vector to store key-value pairs

    // Traverse through the string to extract key-value pairs
    while (i < size)
    {
        string key = ""; // Initialize an empty string to store the key
        // Extract the key part until ':' is encountered
        while (i < size && key_val[i] != ':')
        {
            key += key_val[i];
            i++;
        }
        i++; // Skip the ':' character

        string val = ""; // Initialize an empty string to store the value
        // Extract the value part until ';' is encountered
        while (i < size && key_val[i] != ';')
        {
            val += key_val[i];
            i++;
        }
        i++; // Skip the ';' character

        // Convert the extracted key string to a long long integer (stoll) and store the pair
        res.push_back(make_pair(stoll(key), val));
    }

    return res; // Return the vector containing all key-value pairs
}
/*
 * Function to separate a list of successor nodes from a formatted string.
 * The input string is formatted as "ip1:port1;ip2:port2;..."
 * This function splits the string into individual (IP, port) pairs and stores them in a vector.
 */
vector<psi> Helper_Class::separate_SUCC_LIST(string succ_list)
{
    int n = succ_list.size(); // Get the size of the input string
    int i = 0;                // Initialize index to traverse the string
    vector<psi> res;          // Result vector to store (IP, port) pairs

    // Traverse through the string to extract IP-port pairs
    while (i < n)
    {
        string ip = ""; // Initialize an empty string to store the IP address
        // Extract the IP address part until ':' is encountered
        while (i < n && succ_list[i] != ':')
        {
            ip += succ_list[i];
            i++;
        }
        i++; // Skip the ':' character

        string port = ""; // Initialize an empty string to store the port number
        // Extract the port number part until ';' is encountered
        while (i < n && succ_list[i] != ';')
        {
            port += succ_list[i];
            i++;
        }
        i++; // Skip the ';' character

        // Convert the extracted port string to an integer (stoi) and store the (IP, port) pair
        res.push_back(make_pair(ip, stoi(port)));
    }

    return res; // Return the vector containing all (IP, port) pairs
}
/*
 * Function to combine an IP address and a port into a single string in the format "IP:port".
 */
string Helper_Class::combine_ip_port(string ip, string port)
{
    return ip + ":" + port; // Concatenate IP and port with a colon
}

/*
 * Function to store all keys in the node.
 * It parses the key-value pairs from the provided string and stores them using the node's store_Key function.
 */
void Helper_Class::store_all_keys(NODE_INFORMATION &nodeinfo, string key_val)
{
    int pos = key_val.find("storeKeys"); // Find the position of the "storeKeys" substring
    // Parse key-value pairs from the substring before "storeKeys"
    vector<pls> res = separate_KEY_VAL(key_val.substr(0, pos));
    // Store each key-value pair in the node's data storage
    for (auto c : res)
    {
        nodeinfo.store_Key(c.first, c.second);
    }
}

/*
 * Function to send necessary keys to the new predecessor node.
 * It gathers the keys that need to be transferred to the predecessor node and sends them in a specific format.
 */
void Helper_Class::sendNecessaryKeys(NODE_INFORMATION &nodeinfo, int new_socket, struct sockaddr_in client, string nodeIdString)
{
    socklen_t l = sizeof(client);                     // Get the length of the client's address structure
    int pos = nodeIdString.find(':');                 // Find the position of the ':' in the nodeIdString
    lli nodeid = stoll(nodeIdString.substr(pos + 1)); // Extract and convert the node ID to long long int

    // Get all the keys and values that need to be transferred to the new predecessor
    vector<pls> key_val_vec = nodeinfo.get_all_keys_for_predecessor(nodeid);

    string key_val = ""; // Initialize an empty string to store the concatenated keys and values

    /*
     * Arrange all keys and values in the format "key1:val1;key2:val2;..."
     */
    for (int i = 0; i < key_val_vec.size(); i++)
    {
        key_val += to_string(key_val_vec[i].first) + ":" + key_val_vec[i].second;
        key_val += ";"; // Add a semicolon after each key-value pair
    }

    char key_val_char[2000];               // Prepare a buffer to store the key-value string
    strcpy(key_val_char, key_val.c_str()); // Copy the string to the char array

    // Send the key-value string to the client (predecessor node)
    sendto(new_socket, key_val_char, strlen(key_val_char), 0, (struct sockaddr *)&client, l);
}

/*
 * Function to send a value associated with a key to a requesting node.
 * It retrieves the value for the requested key from the node's data and sends it to the client.
 */
void Helper_Class::send_val_to_node(NODE_INFORMATION &nodeinfo, int new_socket, struct sockaddr_in client, string node_id_string)
{
    node_id_string.pop_back(); // Remove the trailing character (assumed to be a newline or separator)

    lli key = stoll(node_id_string);    // Convert the node_id_string to a long long int (key)
    string val = nodeinfo.get_val(key); // Retrieve the value associated with the key from the node's storage
    socklen_t l = sizeof(client);       // Get the length of the client's address structure

    char valChar[100];            // Prepare a buffer to store the value
    strcpy(valChar, val.c_str()); // Copy the value string to the char array

    // Send the value to the client
    sendto(new_socket, valChar, strlen(valChar), 0, (struct sockaddr *)&client, l);
}
/*
 * Function to send the successor node ID to the requesting client.
 * It retrieves the successor's ID from the node information and sends it to the client.
 */
void Helper_Class::send_successorid(NODE_INFORMATION nodeinfo, int new_socket, struct sockaddr_in client)
{
    // Get the successor node from the current node's information
    ppsl succ = nodeinfo.get_Successor();
    // Convert the successor node ID to a string
    string succId = to_string(succ.second);
    char succ_id_char[40]; // Prepare a buffer to hold the successor ID

    socklen_t l = sizeof(client); // Get the length of the client's address structure

    strcpy(succ_id_char, succId.c_str()); // Copy the successor ID string to the char array

    // Send the successor ID to the client
    sendto(new_socket, succ_id_char, strlen(succ_id_char), 0, (struct sockaddr *)&client, l);
}

/*
 * Function to send the IP and port of the successor node to the joining node.
 * It finds the successor of the joining node, formats the IP and port as a string, and sends it to the node.
 */
void Helper_Class::send_successor(NODE_INFORMATION nodeinfo, string node_id_string, int newSock, struct sockaddr_in client)
{
    lli nd_id = stoll(node_id_string); // Convert the node ID string to a long long int
    socklen_t l = sizeof(client);      // Get the length of the client's address structure

    /* Find the successor of the joining node by using its node ID */
    ppsl succNode = nodeinfo.find_successor(nd_id);

    /* Prepare a char array to hold the IP and port of the successor */
    char ip_port[40];
    string succIp = succNode.first.first;               // Get the successor's IP address
    string succPort = to_string(succNode.first.second); // Get the successor's port number

    // Combine the IP address and port into a single string in the format "IP:port"
    strcpy(ip_port, combine_ip_port(succIp, succPort).c_str());

    // Send the IP and port of the successor to the client node
    sendto(newSock, ip_port, strlen(ip_port), 0, (struct sockaddr *)&client, l);
}
/*
 * Function to send the IP and port of the predecessor node to the requesting client.
 * If there is no predecessor, it sends an empty response.
 */
void Helper_Class::send_predecessor(NODE_INFORMATION nodeinfo, int new_socket, struct sockaddr_in client)
{
    // Get the predecessor node's IP and port from the node information
    ppsl pred = nodeinfo.get_Predecessor();
    string ip = pred.first.first, port = to_string(pred.first.second);
    socklen_t l = sizeof(client); // Get the length of the client's address structure

    /* If the predecessor is not set (i.e., the IP is empty), send an empty message */
    if (ip == "")
    {
        sendto(new_socket, "", 0, 0, (struct sockaddr *)&client, l);
    }
    else
    {
        // Combine the IP and port of the predecessor into a single string
        string ip_port = combine_ip_port(ip, port);

        char ip_port_char[40];                 // Buffer to store the combined IP and port
        strcpy(ip_port_char, ip_port.c_str()); // Copy the combined string to the char array

        // Send the IP and port of the predecessor to the client
        sendto(new_socket, ip_port_char, strlen(ip_port_char), 0, (struct sockaddr *)&client, l);
    }
}

/*
 * Function to retrieve the successor's ID by sending a request to the given IP and port.
 * This function sends a "finger" message to the node at the given IP and port and waits for the response.
 */
lli Helper_Class::get_succ_id(string ip, int port)
{
    struct sockaddr_in Connecting_Server;
    socklen_t l = sizeof(Connecting_Server);

    // Set the server details (IP and port) for the connection
    setServerDetails(Connecting_Server, ip, port);

    /* Set a timeout for the socket to avoid waiting indefinitely */
    struct timeval timer;
    set_timer(timer); // This sets a predefined timeout value for socket operations

    // Create a UDP socket for communication
    int sock = socket(AF_INET, SOCK_DGRAM, 0);

    if (sock < 0)
    {
        perror("Socket creation error");
        exit(-1); // Exit if socket creation fails
    }

    // Set the socket timeout option
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timer, sizeof(struct timeval));

    if (sock < -1)
    {
        perror("Socket option setting error");
        exit(-1);
    }

    // Message to request the successor's ID
    char msg[] = "finger";

    // Send the "finger" message to the given IP and port
    if (sendto(sock, msg, strlen(msg), 0, (struct sockaddr *)&Connecting_Server, l) == -1)
    {
        perror("Send error");
        exit(-1); // Exit if sending the message fails
    }

    char succ_id_char[40]; // Buffer to receive the successor's ID

    // Receive the successor's ID from the node
    int len = recvfrom(sock, succ_id_char, 1024, 0, (struct sockaddr *)&Connecting_Server, &l);

    close(sock); // Close the socket after communication

    // If no response is received or there's an error, return -1
    if (len < 0)
    {
        return -1;
    }

    succ_id_char[len] = '\0'; // Null-terminate the received ID

    // Convert the received successor ID from a string to a long long int and return it
    return atoll(succ_id_char);
}
/*
 * Function to set the socket timer.
 * This function sets the time for which the socket waits for a response before timing out.
 * tv_sec is set to 0, indicating no wait time in seconds, and tv_usec is set to 100000,
 * meaning the timeout is 100 milliseconds.
 */
void Helper_Class::set_timer(struct timeval &timer)
{
    timer.tv_sec = 0;       // Set the timeout seconds to 0
    timer.tv_usec = 100000; // Set the timeout microseconds to 100,000 (100ms)
}

/*
 * Function to retrieve the predecessor of a node by sending a request to the node's IP and port.
 * This function supports two modes:
 * - for_stabilize: Sends a message requesting the predecessor and also notifies the node.
 * - Otherwise, it only requests the predecessor without any notification.
 */
ppsl Helper_Class::get_predecessor_node(string ip, int port, string ip_client, int port_client, bool for_stabilize)
{
    struct sockaddr_in Connecting_Server;
    socklen_t l = sizeof(Connecting_Server);

    // Set the server details for connecting to the given IP and port
    setServerDetails(Connecting_Server, ip, port);

    /* Set a timer for the socket to avoid waiting indefinitely */
    struct timeval timer;
    set_timer(timer);

    // Create a UDP socket
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        perror("Socket creation error");
        exit(-1); // Exit if the socket fails to be created
    }

    // Set the socket's receive timeout using the timer
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timer, sizeof(struct timeval));

    string msg = "";

    /* p2 means just send the predecessor's IP and port without notifying the node.
     * p1 means this is for stabilization, so notify the node as well. */
    if (for_stabilize)
    {
        // Combine the client's IP and port, append "p1" to notify the node for stabilization
        msg = combine_ip_port(ip_client, to_string(port_client)) + "p1";
    }
    else
    {
        // If not stabilizing, send only "p2" to request the predecessor without notification
        msg = "p2";
    }

    // Prepare the message to send (IP and port along with the mode)
    char ip_port_char[40];
    strcpy(ip_port_char, msg.c_str());

    // Send the request to the server
    if (sendto(sock, ip_port_char, strlen(ip_port_char), 0, (struct sockaddr *)&Connecting_Server, l) < 0)
    {
        perror("Send error");
        exit(-1); // Exit if the message could not be sent
    }

    // Receive the predecessor's IP and port response
    int len = recvfrom(sock, ip_port_char, 1024, 0, (struct sockaddr *)&Connecting_Server, &l);
    close(sock); // Close the socket after communication

    // If no response is received, return a null predecessor (empty IP, -1 port, and -1 hash)
    if (len < 0)
    {
        ppsl node;
        node.first.first = "";
        node.first.second = -1;
        node.second = -1;
        return node;
    }

    ip_port_char[len] = '\0'; // Null-terminate the received message

    string ipAndPort = ip_port_char;
    ppsl node;

    // If the response is empty, return a null predecessor
    if (ipAndPort == "")
    {
        node.first.first = "";
        node.first.second = -1;
        node.second = -1;
    }
    else
    {
        // Parse the received IP and port from the response
        psi ipAndPortPair = GET_IP_PORT(ipAndPort);

        node.first.first = ipAndPortPair.first;   // Set the IP of the predecessor
        node.first.second = ipAndPortPair.second; // Set the port of the predecessor

        // Calculate the hash for the IP and port combination
        node.second = Get_hash(ipAndPort);
    }

    return node; // Return the predecessor node (IP, port, and hash)
}

// Function to retrieve the successor list from a node
vector<psi> Helper_Class::get_succ_list_from_node(string ip, int port)
{
    // Initialize the structure for the server address
    struct sockaddr_in Connecting_Server;
    socklen_t l = sizeof(Connecting_Server);

    // Set the server details (IP and port)
    setServerDetails(Connecting_Server, ip, port);

    // Set a timer for the socket to avoid blocking indefinitely
    struct timeval timer;
    set_timer(timer);

    // Create a UDP socket
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        perror("error creating socket");
        exit(-1);
    }

    // Set the receive timeout option for the socket
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timer, sizeof(struct timeval));

    // Message to request the successor list
    char msg[] = "sendSuccList";

    // Send the request to the server
    sendto(sock, msg, strlen(msg), 0, (struct sockaddr *)&Connecting_Server, l);

    // Buffer to receive the successor list
    char succListChar[1000];
    // Receive the successor list from the server
    int len = recvfrom(sock, succListChar, 1000, 0, (struct sockaddr *)&Connecting_Server, &l);

    // Close the socket after the operation is complete
    close(sock);

    // If no data is received, return an empty list
    if (len < 0)
    {
        vector<psi> list;
        return list; // Empty list
    }

    // Null-terminate the received string
    succListChar[len] = '\0';

    // Convert the received character array to a string
    string succList = succListChar;

    // Parse the successor list into a vector of pairs (psi)
    vector<psi> list = separate_SUCC_LIST(succList);

    return list; // Return the parsed successor list
}

// Function to send the successor list to a client
void Helper_Class::send_succ_list(NODE_INFORMATION &nodeinfo, int sock, struct sockaddr_in client)
{
    // Get the size of the client address structure
    socklen_t l = sizeof(client);

    // Retrieve the successor list from the node information
    vector<ppsl> list = nodeinfo.get_SuccList();

    // Convert the successor list into a string format
    string successorList = split_succ_list(list);

    // Prepare a character array to send the successor list
    char succ_list_char[1000];
    strcpy(succ_list_char, successorList.c_str());

    // Send the successor list to the client
    sendto(sock, succ_list_char, strlen(succ_list_char), 0, (struct sockaddr *)&client, l);
}

// Helper function to split a vector of successor pairs into a string
string Helper_Class::split_succ_list(vector<ppsl> aux)
{
    string res = "";
    // Iterate through the successor pairs and format them as "ip:port;"
    for (int i = 1; i <= succ_cnt; i++)
    {
        res = res + aux[i].first.first + ":" + to_string(aux[i].first.second) + ";";
    }
    return res; // Return the formatted string
}

void Helper_Class::send_acknowledgement(int new_socket, struct sockaddr_in client)
{
    socklen_t l = sizeof(client); // Get the size of the client address structure.

    // Send an acknowledgement message "1" to the client using UDP.
    sendto(new_socket, "1", 1, 0, (struct sockaddr *)&client, l);
}

// Function to check if a node is alive by sending a heartbeat message
bool Helper_Class::is_node_alive(string ip, int port)
{
    // Initialize the structure for the server address
    struct sockaddr_in Connecting_Server;
    socklen_t l = sizeof(Connecting_Server);

    // Set the server details (IP and port)
    setServerDetails(Connecting_Server, ip, port);

    // Set a timer for the socket to avoid blocking indefinitely
    struct timeval timer;
    set_timer(timer);

    // Create a UDP socket
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        perror("error creating socket");
        exit(-1);
    }

    // Set the receive timeout option for the socket
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timer, sizeof(struct timeval));

    // Message to check if the node is alive
    char msg[] = "alive";
    // Send the alive check message to the server
    sendto(sock, msg, strlen(msg), 0, (struct sockaddr *)&Connecting_Server, l);

    // Buffer to receive the response
    char response[5];
    // Receive the response from the server
    int len = recvfrom(sock, response, 2, 0, (struct sockaddr *)&Connecting_Server, &l);

    // Close the socket after the operation is complete
    close(sock);

    // Return true if the node is alive (response received), otherwise return false
    return len >= 0 ? true : false;
}
