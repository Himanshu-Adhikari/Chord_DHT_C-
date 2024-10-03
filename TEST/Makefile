all: do

do: main.o nodeInformation.o helperClass.o init.o port.o functions.o 
	g++ main.o init.o functions.o port.o nodeInformation.o helperClass.o -o prog -lcrypto -lpthread
	
main.o: main.cpp
		g++ -std=c++17 -c main.cpp
		
init.o: init.cpp
		g++ -std=c++17 -c init.cpp

port.o: port.cpp
		g++ -std=c++17 -c port.cpp
		
functions.o: functions.cpp
				 g++ -std=c++17 -c functions.cpp
			 
nodeInformation.o: nodeInformation.cpp
				   g++ -std=c++17 -c nodeInformation.cpp
				   
helperClass.o: helperClass.cpp
			   g++ -std=c++17 -c helperClass.cpp			


	
