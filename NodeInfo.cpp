#include<iostream>
#include "all_header.h"
#include "M.h"
#include "Node_info.h"
#include "HelperClass.h"

using namespace std;

Node_information::Node_information(){
    finger_table=vector<ppsl>(nodes_count+1);
    successor_list=vector<ppsl>(successors+1);
    isInRing=false;
}

void Node_information::setStatus(){
    isInRing=true;
}

void Node_information::setSuccessor(string ip,int port,lli hashed_key){
    for(int i=1;i<=successors;i++){
        successor_list[i]={{ip,port},hashed_key};
    }
}

void Node_information::setId(lli nodeid){
    id=nodeid;
}

