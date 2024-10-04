
#include "Node_info.h"
#include "task_functions.h"
#include "macros_and_headers.h"
#include "Helper_class.h"

NODE_INFORMATION::NODE_INFORMATION()
{
    finger_table = vector<ppsl>(nodes + 1);      // Initialize finger table
    successor_list = vector<ppsl>(succ_cnt + 1); // Initialize successor list
    node_present_in_ring = false;                // Initially, the node is not present in the ring
}

// Set the status indicating the node is present in the ring
void NODE_INFORMATION::set_status()
{
    node_present_in_ring = true;
}

// Get the current status of the node
bool NODE_INFORMATION::get_status()
{
    return node_present_in_ring;
}

// Set the successor node information
void NODE_INFORMATION::set_Successor(string ip, int port, lli node_hash_key)
{
    successor.first.first = ip;       // Set the IP address of the successor
    successor.first.second = port;    // Set the port of the successor
    successor.second = node_hash_key; // Set the hash key of the successor
}

// Get the successor node information
ppsl NODE_INFORMATION::get_Successor()
{
    return successor; // Return the successor node
}

// Set the predecessor node information
void NODE_INFORMATION::set_Predecessor(string ip, int port, lli node_hash_key)
{
    predecessor.first.first = ip;       // Set the IP address of the predecessor
    predecessor.first.second = port;    // Set the port of the predecessor
    predecessor.second = node_hash_key; // Set the hash key of the predecessor
}

// Get the predecessor node information
ppsl NODE_INFORMATION::get_Predecessor()
{
    return predecessor; // Return the predecessor node
}

// Set the successor list information
void NODE_INFORMATION::set_SuccList(string ip, int port, lli node_hash_key)
{
    for (int i = 1; i <= succ_cnt; i++)
    {
        // Populate the successor list with the same IP, port, and hash key
        successor_list[i] = make_pair(make_pair(ip, port), node_hash_key);
    }
}

// Get the successor list
vector<ppsl> NODE_INFORMATION::get_SuccList()
{
    return successor_list; // Return the list of successors
}
// Set the node ID
void NODE_INFORMATION::set_nodeid(lli nodeid)
{
    current_node_id = nodeid; // Assign the provided node ID to the member variable
}

// Get the node ID
lli NODE_INFORMATION::get_nodeid()
{
    return current_node_id; // Return the current node ID
}

// Set the finger table with the provided IP, port, and node hash key
void NODE_INFORMATION::set_Finger_table(string ip, int port, lli node_hash_key)
{
    for (int i = 1; i <= nodes; i++)
    {
        // Populate the finger table with the same IP, port, and hash key for all entries
        finger_table[i] = make_pair(make_pair(ip, port), node_hash_key);
    }
}

// Get the finger table
vector<ppsl> NODE_INFORMATION::get_Finger_table()
{
    return finger_table; // Return the current finger table
}

// Store a key-value pair in the data data_dict
void NODE_INFORMATION::store_Key(lli key, string val)
{
    data_dict[key] = val; // Add the key and its associated value to the data_dict
}

// Retrieve a value associated with a key from the data data_dict
string NODE_INFORMATION::get_val(lli key)
{
    // Check if the key exists in the data_dict
    if (data_dict.count(key))
        return data_dict[key]; // Return the value if the key is found
    else
        return ""; // Return an empty string if the key is not found
}

// Print all keys and their corresponding values in the data data_dict
void NODE_INFORMATION::print_keys()
{
    // Check if the data_dict is empty
    if (data_dict.size() == 0)
    {
        cout << "No keys present in the node_dictionary \n"; // Notify if no keys are present
        return;                                              // Exit the function
    }
    cout << "\n Keys Present in the data_dict are " << endl;
    // Iterate through the data_dict and print each key-value pair
    for (auto &[key, val] : data_dict)
    {
        cout << "key: " << key << " val: " << val << endl; // Display the key and value
    }
}
// Function to update the successor list for the current node
void NODE_INFORMATION::update_successor_list()
{
    Helper_Class help; // Create an instance of the Helper_Class
    // Retrieve the current successor list from the successor node
    vector<psi> list = help.get_succ_list_from_node(successor.first.first, successor.first.second);

    // Check if the retrieved list size matches the expected count of successors
    if (list.size() != succ_cnt)
    {
        return; // Exit if the size does not match
    }

    // Update the successor list with the current successor
    successor_list[1] = successor;

    // Populate the successor list with the newly retrieved successors
    for (int i = 2; i <= succ_cnt; i++)
    {
        successor_list[i] = make_pair(
            make_pair(list[i - 2].first, list[i - 2].second),                      // Store the IP and port of the successor
            help.Get_hash(list[i - 2].first + ":" + to_string(list[i - 2].second)) // Get the hash of the successor address
        );
    }
}

// Function to retrieve all keys for the current successor
vector<pls> NODE_INFORMATION::get_all_keys_for_successor()
{
    vector<pls> res; // Vector to store the keys and values for the successor

    // Iterate through the data dictionary and retrieve all key-value pairs
    for (auto [key, val] : data_dict)
    {
        res.push_back(make_pair(key, val)); // Add the key-value pair to the result vector
        data_dict.erase(key);               // Remove the key from the dictionary after retrieval
    }

    return res; // Return the vector containing all keys and values
}

// Function to get keys for a specific predecessor based on its node ID
vector<pls> NODE_INFORMATION::get_all_keys_for_predecessor(lli nodeid)
{
    // Iterator for traversing the data dictionary
    map<lli, string>::iterator it;

    vector<pair<lli, string>> res; // Vector to store keys and values for the predecessor

    // Iterate through the data dictionary
    for (it = data_dict.begin(); it != data_dict.end(); it++)
    {
        lli keyId = it->first; // Get the current key ID

        // Check if the predecessor's ID is greater than the current node's ID
        if (current_node_id < nodeid)
        {
            // Check if the key ID falls between the current node ID and the predecessor ID
            if (keyId > current_node_id && keyId <= nodeid)
            {
                res.push_back(make_pair(keyId, it->second)); // Add to the result vector
                data_dict.erase(it);                         // Remove the key from the dictionary
            }
        }
        // If the predecessor's ID is less than the current node's ID
        else
        {
            // Check if the key ID is either less than or equal to the predecessor ID or greater than the current node ID
            if (keyId <= nodeid || keyId > current_node_id)
            {
                res.push_back(make_pair(keyId, it->second)); // Add to the result vector
                data_dict.erase(it);                         // Remove the key from the dictionary
            }
        }
    }

    return res; // Return the vector containing keys for the predecessor
}

// Function to find the successor of a given node ID in the distributed system
ppsl NODE_INFORMATION::find_successor(lli nodeid)
{
    ppsl self;                         // Create a pair to represent the current node
    self.first.first = sp.GET_IP();    // Get the IP address of the current node
    self.first.second = sp.GET_PORT(); // Get the port of the current node
    self.second = current_node_id;     // Set the current node's ID

    // Check if the target node ID is between the current node ID and the successor's ID
    if (nodeid > current_node_id && nodeid <= successor.second)
    {
        return successor; // Return the successor if the condition is met
    }
    // Check if the current node is the successor or if the node ID is the same as the current node's ID
    else if (current_node_id == successor.second || nodeid == current_node_id)
    {
        return self; // Return the current node
    }
    // Handle case where the successor is the same as the predecessor
    else if (successor.second == predecessor.second)
    {
        // Check if the successor ID is greater than or equal to the current node ID
        if (successor.second >= current_node_id)
        {
            // If the node ID is outside the range, return the current node
            if (nodeid > successor.second || nodeid < current_node_id)
                return self;
        }
        else // If the successor ID is less than the current node ID
        {
            // Check if the node ID is outside the range of current and successor
            if ((nodeid > current_node_id && nodeid > successor.second) || (nodeid < current_node_id && nodeid < successor.second))
                return successor; // Return the successor if conditions are met
            else
                return self; // Otherwise, return the current node
        }
    }
    else // General case when there are distinct predecessor and successor
    {
        // Find the closest preceding node for the given node ID
        ppsl node = closest_preceding_node(nodeid);

        // Check if the closest preceding node is the current node
        if (node.second == current_node_id)
        {
            return successor; // Return the successor if current node is the closest
        }
        else
        {
            // Prepare to connect to the closest preceding node to find the successor
            struct sockaddr_in serverToConnectTo;      // Socket address structure for the target node
            socklen_t len = sizeof(serverToConnectTo); // Length of the address structure

            // If no closest preceding node was found, ask the successor to do so
            if (node.second == -1)
            {
                node = successor; // Fallback to the successor
            }

            Helper_Class help;                                                             // Create a helper instance
            help.setServerDetails(serverToConnectTo, node.first.first, node.first.second); // Set server details for connection

            // Set timer for the socket to prevent blocking
            struct timeval timer;
            help.set_timer(timer);

            // Create a UDP socket
            int sockT = socket(AF_INET, SOCK_DGRAM, 0);

            // Set socket options for receiving timeout
            setsockopt(sockT, SOL_SOCKET, SO_RCVTIMEO, (char *)&timer, sizeof(struct timeval));

            // Check for socket creation error
            if (sockT < 0)
            {
                cout << "socket creation error"; // Error message
                perror("error");
                exit(-1); // Exit if error occurs
            }

            // Send the target node ID to the connected node
            char nodeIdChar[40];
            strcpy(nodeIdChar, to_string(nodeid).c_str());                                                // Convert node ID to string
            sendto(sockT, nodeIdChar, strlen(nodeIdChar), 0, (struct sockaddr *)&serverToConnectTo, len); // Send the node ID

            // Buffer to receive the IP and port of the node's successor
            char ipAndPort[40];

            // Receive the IP and port of the successor
            int l = recvfrom(sockT, ipAndPort, 1024, 0, (struct sockaddr *)&serverToConnectTo, &len);

            close(sockT); // Close the socket after use

            // Check for reception error
            if (l < 0)
            {
                ppsl node;              // Create an empty node to return on error
                node.first.first = "";  // Set IP to an empty string
                node.second = -1;       // Set node ID to -1
                node.first.second = -1; // Set port to -1
                return node;            // Return the empty node
            }

            ipAndPort[l] = '\0'; // Null-terminate the received string

            // Set IP, port, and hash for the found successor node
            string key = ipAndPort;                                  // Convert received data to string
            lli hash = help.Get_hash(ipAndPort);                     // Get the hash of the IP and port
            pair<string, int> ipAndPortPair = help.GET_IP_PORT(key); // Extract IP and port
            node.first.first = ipAndPortPair.first;                  // Set the IP
            node.first.second = ipAndPortPair.second;                // Set the port
            node.second = hash;                                      // Set the hash

            return node; // Return the found successor node
        }
    }
    return {{"", -1}, -1}; // Default return if no valid node is found
}
// Function to find the closest preceding node for a given node ID
ppsl NODE_INFORMATION::closest_preceding_node(lli nodeid)
{
    Helper_Class help; // Create an instance of Helper_Class for utility functions

    // Iterate through the finger table in reverse order
    for (int i = nodes; i >= 1; i--)
    {
        // Check if the finger table entry is valid
        if (finger_table[i].first.first == "" || finger_table[i].first.second == -1 || finger_table[i].second == -1)
        {
            continue; // Skip invalid entries
        }

        // Check if the finger table entry is greater than the current node and less than the target node ID
        if (finger_table[i].second > current_node_id && finger_table[i].second < nodeid)
        {
            return finger_table[i]; // Return the finger table entry if it meets the criteria
        }
        else
        {
            // Get the successor ID of the current finger table entry
            lli successorId = help.get_succ_id(finger_table[i].first.first, finger_table[i].first.second);

            // Skip if the successor ID could not be retrieved
            if (successorId == -1)
                continue;

            // Check if the current finger table entry is greater than its successor
            if (finger_table[i].second > successorId)
            {
                // Check if the target node ID is within the range of the current entry and its successor
                if ((nodeid <= finger_table[i].second && nodeid <= successorId) || (nodeid >= finger_table[i].second && nodeid >= successorId))
                {
                    return finger_table[i]; // Return the current entry if conditions are met
                }
            }
            // Check if the successor is greater than the current entry and the target node ID falls between them
            else if (finger_table[i].second < successorId && nodeid > finger_table[i].second && nodeid < successorId)
            {
                return finger_table[i]; // Return the current entry
            }

            // Get the predecessor node of the current finger table entry
            ppsl predNode = help.get_predecessor_node(finger_table[i].first.first, finger_table[i].first.second, "", -1, false);
            lli predecessorId = predNode.second; // Get the ID of the predecessor node

            // Check if the predecessor ID is valid and if the current entry is less than the predecessor
            if (predecessorId != -1 && finger_table[i].second < predecessorId)
            {
                // Check if the target node ID falls within the range of the current entry and the predecessor
                if ((nodeid <= finger_table[i].second && nodeid <= predecessorId) || (nodeid >= finger_table[i].second && nodeid >= predecessorId))
                {
                    return predNode; // Return the predecessor node if conditions are met
                }
            }
            // Check if the predecessor ID is valid and if the target node ID is within the bounds of the predecessor and current entry
            if (predecessorId != -1 && finger_table[i].second > predecessorId && nodeid >= predecessorId && nodeid <= finger_table[i].second)
            {
                return predNode; // Return the predecessor node
            }
        }
    }

    // Return an empty node if no valid preceding node is found
    ppsl node;
    node.first.first = "";  // Set IP to an empty string
    node.first.second = -1; // Set port to -1
    node.second = -1;       // Set node ID to -1
    return node;            // Return the empty node
}

void NODE_INFORMATION::stabilise()
{
    Helper_Class help;           // Instantiate Helper_Class to access helper functions
    string ownIp = sp.GET_IP();  // Get the IP address of the current node
    int ownPort = sp.GET_PORT(); // Get the port number of the current node

    // Check if the successor node is alive; if not, exit the function
    if (help.is_node_alive(successor.first.first, successor.first.second) == false)
        return;

    // Retrieve the predecessor of the current node's successor
    ppsl predNode = help.get_predecessor_node(successor.first.first, successor.first.second, ownIp, ownPort, true);

    lli predecessorHash = predNode.second; // Get the hash of the predecessor node

    // If the predecessor's hash is invalid or the current predecessor is also invalid, exit the function
    if (predecessorHash == -1 || predecessor.second == -1)
        return;

    // Check if the predecessor is in the correct position in the ring
    // Update the successor if the predecessor is greater than the current node and less than the successor,
    // or if the predecessor is less than the current node and less than the successor
    if (predecessorHash > current_node_id ||
        (predecessorHash > current_node_id && predecessorHash < successor.second) ||
        (predecessorHash < current_node_id && predecessorHash < successor.second))
    {
        successor = predNode; // Update the successor to be the predecessor node
    }
}
void NODE_INFORMATION::check_predecessor()
{
    // If the predecessor is invalid (indicated by -1), exit the function.
    if (predecessor.second == -1)
        return;

    Helper_Class help;
    string ip = predecessor.first.first; // Get the IP address of the predecessor.
    int port = predecessor.first.second; // Get the port of the predecessor.

    // Check if the predecessor node is alive.
    if (help.is_node_alive(ip, port) == false)
    {
        // If the predecessor is not alive and it is the same as the successor,
        // update the successor information to point to this node itself.
        if (predecessor.second == successor.second)
        {
            successor.first.first = sp.GET_IP();                                          // Update successor IP to the current node's IP.
            successor.first.second = sp.GET_PORT();                                       // Update successor port to the current node's port.
            successor.second = current_node_id;                                           // Set successor ID to the current node's ID.
            set_SuccList(successor.first.first, successor.first.second, current_node_id); // Update the successor list.
        }
        // Mark the predecessor as invalid by clearing its IP and setting its ID to -1.
        predecessor.first.first = "";  // Clear the IP address.
        predecessor.first.second = -1; // Set the port to -1 (invalid).
        predecessor.second = -1;       // Set the predecessor ID to -1 (invalid).
    }
}

// Function to check the status of the successor node
void NODE_INFORMATION::check_successor()
{
    // Return if the successor is the current node
    if (successor.second == current_node_id)
        return;

    Helper_Class help;                 // Create an instance of Helper_Class for utility functions
    string ip = successor.first.first; // Retrieve the IP address of the successor
    int port = successor.first.second; // Retrieve the port number of the successor

    // Check if the successor node is alive
    if (help.is_node_alive(ip, port) == false)
    {
        // If the successor is not alive, update the successor to the next in the successor list
        successor = successor_list[2]; // Update to the next successor
        update_successor_list();       // Refresh the successor list
    }
}
// Function to notify the current node of a new predecessor
void NODE_INFORMATION::notify(ppsl node)
{
    lli pred_hash = predecessor.second; // Store the hash of the current predecessor
    lli node_hash = node.second;        // Get the hash of the new node being notified

    predecessor = node; // Update the predecessor to the new node

    // If the current node is the only node (i.e., its successor points to itself)
    if (successor.second == current_node_id)
    {
        successor = node; // Set the successor to the new node as well
    }
}

void NODE_INFORMATION::fix_finger_table()
{
    int next = 1;
    lli mod = pow(2, nodes);

    Helper_Class help;
    while (next <= nodes)
    {
        if (help.is_node_alive(successor.first.first, successor.first.second) == false)
            return;

        lli newId = current_node_id + pow(2, next - 1);
        newId = newId % mod;
        ppsl node = find_successor(newId);
        if (node.first.first == "" || node.second == -1 || node.first.second == -1)
            break;
        finger_table[next] = node;
        next++;
    }
}
