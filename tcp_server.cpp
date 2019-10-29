#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <iostream>
#include <ctype.h>
#include "Log.h"

char* capitalize(char* str1) {
	int i = 0;
	while (str1[i]) {
		str1[i] = toupper(str1[i]);
		i++;
	}
	return str1;
}

int main(int argc, char *argv[]) {

	// Defaults
	int port = 2012;
	int rateReq = 3; // x amount of requests
	int rateSec = 60; // per x amount of seconds per user
	int maxUsers = 3;
	int timeout = 80; // seconds

	Log logger = Log(); // output hardcoded to Log.txt

	for (int i = 1; i < argc; i++) { // can we assume we are given valid input
		if (strcmp(capitalize(argv[i]), "PORT") == 0 && i + 1 < argc) {
			int temporaryPort = atoi(argv[++i]); // cast to int
			if (2000 <= temporaryPort && temporaryPort <= 3000) {
				port = temporaryPort;
			} else {
				std::cout << "Invalid port, will use default" << std::endl;
				i--;
			}
		} else if (strcmp(argv[i], "RATE_MSGS") == 0 && i + 1 < argc) {
			int tempRateReq = atoi(argv[++i]);
			if (tempRateReq > 0) {
				rateReq = tempRateReq;
			} else {
				std::cout << "Invalid rate messages, will use default" << std::endl;
				i--;
			}
		} else if (strcmp(argv[i], "RATE_TIME") == 0 && i + 1 < argc) {
			int tempRateTime = atoi(argv[++i]);
			if (tempRateTime > 0) {
				rateSec = tempRateTime;
			} else {
				std::cout << "Invalid rate time, will use default" << std::endl;
				i--;
			}
		} else if (strcmp(argv[i], "MAX_USERS") == 0 && i + 1 < argc) {
			int tempMaxUsers = atoi(argv[++i]);
			if (tempMaxUsers > 0) {
				maxUsers = tempMaxUsers;
			} else {
				std::cout << "Invalid max users, will use default" << std::endl;
				i--;
			}
		} else if (strcmp(argv[i], "TIME_OUT") == 0 && i + 1 < argc) {
			int tempTimeout = atoi(argv[++i]);
			if (tempTimeout > 0) {
				timeout = tempTimeout;
			} else {
				std::cout << "Invalid timeout, will use default" << std::endl;
				i--;
			}
		}
	}

	logger.serverStarted();

	char server_message[256] = "You have reached the server";

	// create the server socket
	int server_socket;
	server_socket = socket(AF_INET, SOCK_STREAM, 0);

	// define the server address
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(9002);
	server_address.sin_addr.s_addr = INADDR_ANY;

	// bind the socket to our specified IP and port
	bind(server_socket, (struct sockaddr*) &server_address, sizeof(server_address));

	listen(server_socket, 5);

	int client_socket;
	client_socket = accept(server_socket, NULL, NULL);

	// send the message
	send(client_socket, server_message, sizeof(server_message), 0);

	// close the socket
	close(server_socket);

	return 0;
}
