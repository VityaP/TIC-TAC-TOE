all: player server

player : player.cpp functions.o
	g++ -g3 -std=c++17 -Wall player.cpp functions.o -lzmq -o player -pthread

server: functions.o server.cpp
	g++ -g3 -std=c++17 -Wall server.cpp functions.o -lzmq -o server

functions.o : functions.cpp functions.h
	g++ -c -std=c++17 functions.cpp

clean:
	rm -f *.o player server