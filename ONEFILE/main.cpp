#include "macros_and_headers.h"

class socket_and_port
{
private:
    int serving_port;
    int serving_socket;
    struct sockaddr_in current;

public:
    /* Generate a port number for the server */
    void specifyPortServer()
    {

        /* Seed the random number generator with the current time */
        srand(time(0));

        /* Generate a random port number between 0 and 65535 */
        serving_port = rand() % 65536;

        /* Ensure the port number is at least 1024 (valid range for non-privileged ports) */
        if (serving_port < 1024)
            serving_port += 1024; // Shift the number up to be within the valid range

        /* Define the length of the address structure */
        socklen_t len = sizeof(current);

        /* Create a UDP socket */
        serving_socket = socket(AF_INET, SOCK_DGRAM, 0);

        /* Specify the address family (IPv4) */
        current.sin_family = AF_INET;

        /* Set the port number, converting from host byte order to network byte order */
        current.sin_port = htons(serving_port);

        /* Set the IP address to localhost */
        current.sin_addr.s_addr = inet_addr("127.0.0.1");

        /* Bind the socket to the specified address and port */
        if (bind(serving_socket, (struct sockaddr *)&current, len) < 0)
        {
            perror("error"); // Print an error message if binding fails
            exit(-1);        // Exit the program with an error status
        }
    }

    /* Change the port number for the server */
    void Change_port_no_to(int new_port)
    {
        // Check if the new port number is within the valid range (1024 - 65535)
        if (new_port < 1024 || new_port > 65535)
        {
            cout << "Please enter a valid port number\n";
        }
        else
        {
            // Check if the new port number is already in use
            if (IF_CURRENT_PORT_IN_USE(new_port))
            {
                cout << "Sorry, but the port number is already in use\n";
            }
            else
            {
                // Close the existing socket before changing the port
                close(serving_socket);

                // Define the length of the address structure
                socklen_t addressLength = sizeof(current);

                // Create a new UDP socket
                serving_socket = socket(AF_INET, SOCK_DGRAM, 0);

                // Set the new port number in network byte order
                current.sin_port = htons(new_port);

                // Attempt to bind the new socket to the specified address and port
                if (bind(serving_socket, (struct sockaddr *)&current, addressLength) < 0)
                {
                    perror("error"); // Print an error message if binding fails
                    // Revert the port number back to the previous one if binding fails
                    current.sin_port = htons(serving_port);
                }
                else
                {
                    // Update the current port number to the new port
                    serving_port = new_port;
                    cout << "Port number changed to: " << serving_port << endl;
                }
            }
        }
    }
    /* Check if a port number is already in use */
    bool IF_CURRENT_PORT_IN_USE(int port_no)
    {
        // Create a new socket for checking the port status
        int checkSock = socket(AF_INET, SOCK_DGRAM, 0);

        // Prepare the address structure for the specified port
        struct sockaddr_in address;
        socklen_t addressLength = sizeof(address);
        address.sin_port = htons(port_no);                // Set the port number in network byte order
        address.sin_family = AF_INET;                     // Specify the address family (IPv4)
        address.sin_addr.s_addr = inet_addr("127.0.0.1"); // Use localhost

        // Attempt to bind the socket to the specified address and port
        if (bind(checkSock, (struct sockaddr *)&address, addressLength) < 0)
        {
            perror("error"); // Print an error message if binding fails
            return true;     // Port is in use
        }
        else
        {
            close(checkSock); // Close the socket if binding succeeded
            return false;     // Port is available
        }
    }
    /* Get the IP address of the server */
    string GET_IP()
    {
        // Convert the IP address from network byte order to a human-readable string
        string ipAddress = inet_ntoa(current.sin_addr);
        return ipAddress; // Return the IP address
    }

    /* Get the port number on which the server is listening */
    int GET_PORT()
    {
        return serving_port; // Return the currently set port number
    }

    /* Get the socket file descriptor */
    int getSocketFd()
    {
        return serving_socket; // Return the socket file descriptor
    }

    /* Close the socket */
    void closeSocket()
    {
        close(serving_socket); // Close the socket to release the resource
    }
};

class NODE_INFORMATION
{
private:
    lli node_id;                 // Unique identifier for the node
    ppsl successor;              // Successor node (pair of IP, port, and node hash key)
    ppsl predecessor;            // Predecessor node (pair of IP, port, and node hash key)
    vector<ppsl> finger_table;   // Finger table for efficient lookups
    vector<ppsl> successor_list; // List of successor nodes
    map<lli, string> data_dict;  // Dictionary to store data associated with node IDs
    bool node_present_in_ring;   // Status indicating if the node is present in the ring

public:
    // Constructor to initialize the node information
    NODE_INFORMATION()
    {
        finger_table = vector<ppsl>(nodes + 1);      // Initialize finger table
        successor_list = vector<ppsl>(succ_cnt + 1); // Initialize successor list
        node_present_in_ring = false;                // Initially, the node is not present in the ring
    }

    socket_and_port sp; // Socket and port information for the node

    // Set the status indicating the node is present in the ring
    void set_status()
    {
        node_present_in_ring = true;
    }

    // Get the current status of the node
    bool get_status()
    {
        return node_present_in_ring;
    }

    // Set the successor node information
    void set_Successor(string ip, int port, lli node_hash_key)
    {
        successor.first.first = ip;       // Set the IP address of the successor
        successor.first.second = port;    // Set the port of the successor
        successor.second = node_hash_key; // Set the hash key of the successor
    }

    // Get the successor node information
    ppsl get_Successor()
    {
        return successor; // Return the successor node
    }

    // Set the predecessor node information
    void set_Predecessor(string ip, int port, lli node_hash_key)
    {
        predecessor.first.first = ip;       // Set the IP address of the predecessor
        predecessor.first.second = port;    // Set the port of the predecessor
        predecessor.second = node_hash_key; // Set the hash key of the predecessor
    }

    // Get the predecessor node information
    ppsl get_Predecessor()
    {
        return predecessor; // Return the predecessor node
    }

    // Set the successor list information
    void set_SuccList(string ip, int port, lli node_hash_key)
    {
        for (int i = 1; i <= succ_cnt; i++)
        {
            // Populate the successor list with the same IP, port, and hash key
            successor_list[i] = make_pair(make_pair(ip, port), node_hash_key);
        }
    }

    // Get the successor list
    vector<ppsl> get_SuccList()
    {
        return successor_list; // Return the list of successors
    }
    // Set the node ID
    void set_nodeid(lli nodeid)
    {
        node_id = nodeid; // Assign the provided node ID to the member variable
    }

    // Get the node ID
    lli get_nodeid()
    {
        return node_id; // Return the current node ID
    }

    // Set the finger table with the provided IP, port, and node hash key
    void set_Finger_table(string ip, int port, lli node_hash_key)
    {
        for (int i = 1; i <= nodes; i++)
        {
            // Populate the finger table with the same IP, port, and hash key for all entries
            finger_table[i] = make_pair(make_pair(ip, port), node_hash_key);
        }
    }

    // Get the finger table
    vector<ppsl> get_Finger_table()
    {
        return finger_table; // Return the current finger table
    }

    // Store a key-value pair in the data dictionary
    void store_Key(lli key, string val)
    {
        data_dict[key] = val; // Add the key and its associated value to the dictionary
    }

    // Retrieve a value associated with a key from the data dictionary
    string get_val(lli key)
    {
        // Check if the key exists in the dictionary
        if (data_dict.count(key))
            return data_dict[key]; // Return the value if the key is found
        else
            return ""; // Return an empty string if the key is not found
    }

    // Print all keys and their corresponding values in the data dictionary
    void print_keys()
    {
        // Check if the dictionary is empty
        if (data_dict.size() == 0)
        {
            cout << "No keys present in the node_dictionary \n"; // Notify if no keys are present
            return;                                              // Exit the function
        }
        cout << "\n Keys Present in the dictionary are " << endl;
        // Iterate through the dictionary and print each key-value pair
        for (auto &[key, val] : data_dict)
        {
            cout << "key: " << key << " val: " << val << endl; // Display the key and value
        }
    }
    // void update_successor_list(){
    //     Helper_Class help;
    //     vector<psi>list=help.get
    // }
};
class Helper_Class
{
public:
    mutex mt; // Mutex for thread safety

    // Split a command string into individual arguments
    vector<string> SplitCommands(string cmd)
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
    lli Get_hash(string key)
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
    psi GET_IP_PORT(string ip_port)
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
    pls GET_KEY_VAL(string key_val)
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
    bool isKeyValue(string id)
    {
        // Find the position of the colon in the string
        int pos = id.find(":");

        // If no colon is found, return false (not a key-value pair)
        if (pos == -1)
            return false;

        // Check if all characters before the colon are digits
        for (int i = 0; i < pos; i++)
        {
            // If any character is not a digit (ASCII values 48-57), return false
            if (!(id[i] >= 48 && id[i] <= 57))
                return false;
        }

        return true; // If all checks pass, return true (valid key-value format)
    }

    /* Will contact a node and get the value of a particular key from that node */
    string getKeyFromNode(ppsl node, string keyHash)
    {
        // Extract IP address and port number from the node pair
        string ip = node.first.first;
        int port = node.first.second;

        struct sockaddr_in serverToConnectTo;    // Structure to hold server address details
        socklen_t l = sizeof(serverToConnectTo); // Length of the address structure

        // Set the details for the server to connect to
        setServerDetails(serverToConnectTo, ip, port);

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
        sendto(sock, keyHashChar, strlen(keyHashChar), 0, (struct sockaddr *)&serverToConnectTo, l);

        char valChar[100]; // Buffer to receive the value from the node
        // Receive the response from the node
        int len = recvfrom(sock, valChar, 1024, 0, (struct sockaddr *)&serverToConnectTo, &l);

        valChar[len] = '\0'; // Null terminate the received string

        string val = valChar; // Convert the char array to a string

        close(sock); // Close the socket to release the resource

        return val; // Return the retrieved value
    }

    /* Set details of the server to which you want to connect */
    void setServerDetails(struct sockaddr_in &server, string ip, int port)
    {
        server.sin_family = AF_INET;                    // Set address family to IPv4
        server.sin_addr.s_addr = inet_addr(ip.c_str()); // Convert IP string to binary form
        server.sin_port = htons(port);                  // Convert port number to network byte order
    }

    
};

int main()
{
    return 0;
}