CC = g++
CFLAGS  = -g -Wall

default: server

# To create the executable file count we need the object files
# countwords.o, counter.o, and scanner.o:
server:  tcp_server.o Log.o
	$(CC) $(CFLAGS) -o server tcp_server.o Log.o

# To create the object file countwords.o, we need the source
# files countwords.c, scanner.h, and counter.h:
#
tcp_server.o:  tcp_server.cpp Log.h
	$(CC) $(CFLAGS) -c tcp_server.cpp

# To create the object file counter.o, we need the source files
# counter.c and counter.h:
#
Log.o:  Log.cpp Log.h
	$(CC) $(CFLAGS) -c Log.cpp

# To create the object file scanner.o, we need the source files
# scanner.c and scanner.h:
#

clean:
	$(RM) count *.o *~