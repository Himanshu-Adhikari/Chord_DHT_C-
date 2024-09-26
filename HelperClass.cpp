#include "all_header.h"
#include "HelperClass.h"
mutex mt;

/* Split command into separate arguments using stringstream */
vector<string>Helper_Functions::splitCommand(string cmd){
    vector<string>arg;
    stringstream ss(cmd);
    string s;
    while(ss>>s)arg.push_back(s);
    return arg;
}

/* Use SHA1 for hashing a key */
lli Helper_Functions::getHash(string key){
    unsigned char output_buffer[41];
    char FinalHash[41];
    string Hashed_key="";

    lli mod= pow(2,nodes_count);

     /* convert string to an unsigned char array because SHA1 takes unsigned char array as parameter */
    unsigned char unsigned_key[key.length()+1];
    for(int i=0;i<key.length();i++){
        unsigned_key[i] = key[i];
    }

    SHA1(unsigned_key,sizeof(unsigned_key),output_buffer);

    for(int i=0;i<nodes_count/8;i++){
        sprintf(FinalHash,"%d",output_buffer[i]);
        Hashed_key+=FinalHash;
    }
    return stoll(Hashed_key)%mod;

}


pair<string,int> Helper_Functions::Get_Ip_and_Port(string key){
    int id=key.find(':');
    string ip=key.substr(0,id),port=key.substr(id+1);

    pair<string,int>ip_and_port = {ip,atoi(port.c_str())};
    
    return ip_and_port;
}