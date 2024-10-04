all: do

do: main.o 
	g++ main.o  -o server -lcrypto -lpthread
	
main.o: main.cpp
		g++ -std=c++17 -c main.cpp
