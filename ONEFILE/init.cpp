#include "init.h"
#include "socket_port.h"
#include "task_functions.h"
#include "Helper_class.h"
#include "Node_info.h"

void initialize()
{
    NODE_INFORMATION nodeInfo = NODE_INFORMATION();

    /* open a socket to listen to other nodes */
    nodeInfo.sp.specifyPortServer();

    cout << "Now listening at port number " << nodeInfo.sp.GET_PORT() << endl;

    cout << "Type help to know more\n";

    string command;

    while (1)
    {
        cout << "> ";
        getline(cin, command);

        /* find space in command and seperate arguments*/
        Helper_Class help;
        vector<string> arguments = help.SplitCommands(command);

        string arg = arguments[0];
        if (arguments.size() == 1)
        {

            /* creates */
            if (arg == "create")
            {
                if (nodeInfo.get_status() == true)
                {
                    cout << "Sorry but this node is already on the ring\n";
                }
                else
                {
                    thread first(create, ref(nodeInfo));
                    first.detach();
                }
            }

            /* prints */
            else if (arg == "printstate")
            {
                if (nodeInfo.get_status() == false)
                {
                    cout << "Sorry this node is not in the ring\n";
                }
                else
                    printState(nodeInfo);
            }

            /* leaves */
            else if (arg == "leave")
            {
                leave(nodeInfo);
                nodeInfo.sp.closeSocket();
                return;
            }

            /* print current port number */
            else if (arg == "port")
            {
                cout << nodeInfo.sp.GET_PORT() << endl;
            }

            /* print keys present in this node */
            else if (arg == "print")
            {
                if (nodeInfo.get_status() == false)
                {
                    cout << "Sorry this node is not in the ring\n";
                }
                else
                    nodeInfo.print_keys();
            }

            else if (arg == "help")
            {
                showHelp();
            }

            else
            {
                cout << "Invalid Command\n";
            }
        }

        else if (arguments.size() == 2)
        {

            /* */
            if (arg == "port")
            {
                if (nodeInfo.get_status() == true)
                {
                    cout << "Sorry you can't change port number now\n";
                }
                else
                {
                    int newPortNo = atoi(arguments[1].c_str());
                    nodeInfo.sp.Change_port_no_to(newPortNo);
                }
            }

            /* */
            else if (arg == "get")
            {
                if (nodeInfo.get_status() == false)
                {
                    cout << "Sorry this node is not in the ring\n";
                }
                else
                    get(arguments[1], nodeInfo);
            }

            else
            {
                cout << "Invalid Command\n";
            }
        }

        else if (arguments.size() == 3)
        {

            /* */
            if (arg == "join")
            {
                if (nodeInfo.get_status() == true)
                {
                    cout << "Sorry but this node is already on the ring\n";
                }
                else
                    join(nodeInfo, arguments[1], arguments[2]);
            }

            /* puts the entered key and it's value to the necessary node*/
            else if (arg == "put")
            {
                if (nodeInfo.get_status() == false)
                {
                    cout << "Sorry this node is not in the ring\n";
                }
                else
                    put(arguments[1], arguments[2], nodeInfo);
            }

            else
            {
                cout << "Invalid Command\n";
            }
        }

        else
        {
            cout << "Invalid Command\n";
        }
    }
}