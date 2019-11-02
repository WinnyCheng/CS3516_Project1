CC = g++
CFLAGS  = -g -Wall

all: server client

server:  tcp_server.o Log.o
	$(CC) $(CFLAGS) -o server tcp_server.o Log.o

tcp_server.o:  tcp_server.cpp Log.h
	$(CC) $(CFLAGS) -c tcp_server.cpp

client:  tcp_client.o
	$(CC) $(CFLAGS) -o client tcp_client.o

tcp_client.o:  tcp_client.cpp
	$(CC) $(CFLAGS) -c tcp_client.cpp


Log.o:  Log.cpp Log.h
	$(CC) $(CFLAGS) -c Log.cpp
