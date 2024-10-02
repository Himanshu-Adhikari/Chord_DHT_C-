all: do

do: main.o NodeInfo.o HelperClass.o init.o Port.o functions.o 
	g++ main.o init.o functions.o Port.o NodeInfo.o HelperClass.o -o prog -lcrypto -lpthread
	
main.o: main.cpp
		g++ -std=c++17 -c main.cpp
		
init.o: init.cpp
		g++ -std=c++17 -c init.cpp

Port.o: Port.cpp
		g++ -std=c++17 -c Port.cpp
		
functions.o: functions.cpp
				 g++ -std=c++17 -c functions.cpp
			 
NodeInfo.o: NodeInfo.cpp
				   g++ -std=c++17 -c NodeInfo.cpp
				   
HelperClass.o: HelperClass.cpp
			   g++ -std=c++17 -c HelperClass.cpp			


	