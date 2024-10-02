#include <bits/stdc++.h>

#include "init.h"
#include "HelperClass.h"
#include "functions.h"
#include "Node_info.h"
#include "PortHeader.h"

using namespace std;

void initialise()
{
    Node_information nodeinfo = Node_information();

    /*open a socket to listen to other nodes*/
    nodeinfo.sp.specifyPortServer();

    cout << "Listening at port : " << nodeinfo.sp.getPortNumber() << endl;
    cout << "Type help to know all commands.\n";

    string cmd;
    while (true)
    {
        cout << "> ";
        getline(cin, cmd);
        /*Find space and separate arguments*/
        Helper_Functions helper = Helper_Functions();
        vector<string> args = helper.splitCommand(cmd);

        string current_arg = args[0];
        if (args.size() == 1)
        {
            /*creates*/
            if (current_arg == "create")
            {
                if (nodeinfo.getStatus())
                {
                    cout << "The node is already part of the ring .\n";
                }
                else
                {
                    thread first(create, ref(nodeinfo));
                    first.detach();
                }
            }
            else if (current_arg == "printstate")
            {
                if (nodeinfo.getStatus())
                {
                    printState(nodeinfo);
                }
                else
                {
                    cout << "The node is not in the ring .\n";
                }
            }
            else if (current_arg == "leave")
            {
                leave(nodeinfo);
                nodeinfo.sp.closeSocket();
                return;
            }
            else if (current_arg == "port")
            {
                cout << nodeinfo.sp.getPortNumber() << endl;
            }
            else if (current_arg == "print")
            {
                if (nodeinfo.getStatus())
                {
                    nodeinfo.printKeys();
                }
                else
                {
                    cout << "The node is not in the ring .\n";
                }
            }
            else if (current_arg == "help")
            {
                show_help();
            }
            else
            {
                cout << "Invalid commans\n";
            }
        }
        else if (args.size() == 2)
        {
            if (current_arg == "port")
            {
                if (nodeinfo.getStatus() == true)
                {
                    cout << "You can't change port number right now\n";
                }
                else
                {
                    int new_port = atoi(args[1].c_str());
                    nodeinfo.sp.changePortNumber(new_port);
                }
            }
            else if (current_arg == "get")
            {
                if (nodeinfo.getStatus())
                {
                    get(args[1], nodeinfo);
                }
                else
                {
                    cout << "The node is not in the ring .\n";
                }
            }
            else
            {
                cout << "Invalid commans\n";
            }
        }
        else if (args.size() == 3)
        {
            if (current_arg == "put")
            {
                if (nodeinfo.getStatus())
                {
                    put(args[1], args[2], nodeinfo);
                }
                else
                {
                    cout << "The node is not in the ring .\n";
                }
            }
            else if (current_arg == "join")
            {
                if (nodeinfo.getStatus())
                {
                    cout << "The node is already in the ring .\n";
                }
                else
                {
                    join(nodeinfo, args[1], args[2]);
                }
            }
        }

        else
        {
            cout << "Invalid commans\n";
        }
    }
}