#ifndef node_info_h
#define node_info_h

#include "macros_and_headers.h"
#include "socket_port.h"

class NODE_INFORMATION
{
private:
    lli current_node_id;         // Unique identifier for the node
    ppsl successor;              // Successor node (pair of IP, port, and node hash key)
    ppsl predecessor;            // Predecessor node (pair of IP, port, and node hash key)
    vector<ppsl> finger_table;   // Finger table for efficient lookups
    vector<ppsl> successor_list; // List of successor nodes
    map<lli, string> data_dict;  // Dictionary to store data associated with node IDs
    bool node_present_in_ring;   // Status indicating if the node is present in the ring

public:
    // Constructor to initialize the node information
    NODE_INFORMATION();
    socket_and_port sp; // Socket and port information for the node
    void set_status();
    bool get_status();
    void set_Successor(string ip, int port, lli node_hash_key);
    ppsl get_Successor();
    void set_Predecessor(string ip, int port, lli node_hash_key);
    ppsl get_Predecessor();
    void set_SuccList(string ip, int port, lli node_hash_key);
    vector<ppsl> get_SuccList();
    void set_nodeid(lli nodeid);
    lli get_nodeid();
    void set_Finger_table(string ip, int port, lli node_hash_key);
    vector<ppsl> get_Finger_table();
    void store_Key(lli key, string val);
    string get_val(lli key);
    void print_keys();
    void update_successor_list();
    vector<pls> get_all_keys_for_successor();
    vector<pls> get_all_keys_for_predecessor(lli nodeid);
    ppsl find_successor(lli nodeid);
    ppsl closest_preceding_node(lli nodeid);

    void stabilise();
    void check_predecessor();
    void check_successor();
    void notify(ppsl node);
    void fix_finger_table();
};

#endif
