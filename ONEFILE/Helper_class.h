
#ifndef helper_class_h
#define helper_class_h

#include <iostream>
#include "macros_and_headers.h"
#include "Node_info.h"

using namespace std;

class Helper_Class
{
public:
       vector<string> SplitCommands(string cmd);
    lli Get_hash(string key);
    psi GET_IP_PORT(string ip_port);
    pls GET_KEY_VAL(string key_val);
    bool isKeyValue(string current_node_id);
    string getKeyFromNode(ppsl node, string keyHash);
    void setServerDetails(struct sockaddr_in &server, string ip, int port);
    void Send_Key_to_Node(ppsl node, lli keyhash, string val);
    void Get_Keys_from_successor(NODE_INFORMATION &nodeinfo, string ip, int port);
    vector<pls> separate_KEY_VAL(string key_val);
    vector<psi> separate_SUCC_LIST(string succ_list);
    string combine_ip_port(string ip, string port);
    void store_all_keys(NODE_INFORMATION &nodeinfo, string key_val);
    void sendNecessaryKeys(NODE_INFORMATION &nodeinfo, int new_socket, struct sockaddr_in client, string nodeIdString);
    void send_val_to_node(NODE_INFORMATION &nodeinfo, int new_socket, struct sockaddr_in client, string node_id_string);
    void send_successorid(NODE_INFORMATION nodeinfo, int new_socket, struct sockaddr_in client);
    void send_successor(NODE_INFORMATION nodeinfo, string node_id_string, int newSock, struct sockaddr_in client);
    void send_predecessor(NODE_INFORMATION nodeinfo, int new_socket, struct sockaddr_in client);
    lli get_succ_id(string ip, int port);
    void set_timer(struct timeval &timer);
    ppsl get_predecessor_node(string ip, int port, string ip_client, int port_client, bool for_stabilize);
    vector<psi> get_succ_list_from_node(string ip, int port);
    void send_succ_list(NODE_INFORMATION &nodeinfo, int sock, struct sockaddr_in client);
    string split_succ_list(vector<ppsl> aux);
    void send_acknowledgement(int new_socket, struct sockaddr_in client);
    bool is_node_alive(string ip, int port);
};

#endif