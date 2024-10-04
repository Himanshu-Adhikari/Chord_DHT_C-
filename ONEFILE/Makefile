all: do

do: main.o Node_info.o Helper_class.o init.o socket_port.o task_functions.o 
	g++ main.o init.o task_functions.o socket_port.o Node_info.o Helper_class.o -o prog -lcrypto -lpthread
	
main.o: main.cpp
		g++ -std=c++17 -c main.cpp
		
init.o: init.cpp
		g++ -std=c++17 -c init.cpp

socket_port.o: socket_port.cpp
		g++ -std=c++17 -c socket_port.cpp
		
task_functions.o: task_functions.cpp
				 g++ -std=c++17 -c task_functions.cpp
			 
Node_info.o: Node_info.cpp
				   g++ -std=c++17 -c Node_info.cpp
				   
Helper_class.o: Helper_class.cpp
			   g++ -std=c++17 -c Helper_class.cpp			


	
