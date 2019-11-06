#define _BSD_SOURCE

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <iostream>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <ctype.h>
#include <getopt.h>
#include "Log.h"

char *capitalize(char *str1) {
	int i = 0;
	while (str1[i]) {
		str1[i] = toupper(str1[i]);
		i++;
	}
	return str1;
}

/**
 * Sends an image from client to server
 *
 * @param imgURL - The path to the image
 * @param socket - The file descriptor of the socket
 *
 * @return -1 on disconnected socket, 1 on invalid request, 0 on success, 2 on timeout
 *
 * SOURCE: https://stackoverflow.com/questions/5638831/c-char-array
 * Author Username: @Cubbi
 */
int readImageFromClient(const char *outputURL, int socket) {
	//Read Picture Size
	int size;
	const int MAX_SIZE = 50000; // 50kb
	int validity = read(socket, &size, sizeof(int));
	if (validity == 0){
		return -1;
	} else if (validity < 0) {
		return  2;
	}

	//Read Picture Byte Array
	char p_array[size];
	validity = read(socket, p_array, size);
	if (validity <= 0) {
		return -1;
	}
	if (MAX_SIZE < size) { // secure server, clear buffer, return error
		return 1;
	}
	// Convert it Back into Picture
	FILE *image;
	image = fopen(outputURL, "w");
	fwrite(p_array, 1, sizeof(p_array), image);
	fclose(image);
	return 0;
}

string parseURL(char* rawOutput) {
	const char delim2[3] = "\n";
	strtok(rawOutput, delim2); // not concerned with tokens before
	strtok(NULL, delim2);
	strtok(NULL, delim2);
	strtok(NULL, delim2);
	return strtok(NULL, delim2);
}

/**
 * Executes command in unix shell and then reads standard out and returns the value
 *
 * SOURCE: https://stackoverflow.com/questions/478898/how-do-i-execute-a-command-and-get-the-output-of-the-command-within-c-using-po
 * AUTHOR USERNAME: gregpaton08
 */
std::string exec(const char *cmd) {
	std::array<char, 128> buffer;
	std::string result;
	std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
	if (!pipe) {
		throw std::runtime_error("popen() failed!");
	}
	while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
		result += buffer.data();
	}
	return result;
}

std::string convertQRToURL(const char *url) {
	char command[100];
	strcpy(command, "java -cp \"javase.jar;core.jar\" com.google.zxing.client.j2se.CommandLineRunner ");
	strcat(command, url);
	return exec(command);
}

int main(int argc, char *argv[]) {

	// Defaults
	int port = 2012;
	int rateReq = 3; // x amount of requests
	int rateSec = 60; // per x amount of seconds per user
	int maxUsers = 3;
	int timeout = 80; // seconds


	static struct option long_options[] = {
			{"PORT",      optional_argument, 0, 'a'},
			{"RATE_MSGS", optional_argument, 0, 'm'},
			{"RATE_TIME", optional_argument, 0, 'r'},
			{"MAX_USERS", optional_argument, 0, 'u'},
			{"TIMEOUT",   optional_argument, 0, 't'},
			{0,           0,                 0, 0}
	};

	int long_index = 0;
	int opt;
	int tempArg;
	while ((opt = getopt_long(argc, argv, "amrut::", long_options, &long_index)) != -1) {

		switch (opt) {
			case 'a' :
				tempArg = atoi(optarg);
				if (tempArg != 0 && 2000 <= tempArg && tempArg <= 3000) {
					port = tempArg;
				} else {
					std::cout << "invalid option [" << optarg << "] defaulting to [" << port << "]" << std::endl;
				}
				break;
			case 'm' :
				tempArg = atoi(optarg);
				if (tempArg != 0) {
					rateReq = tempArg;
				} else {
					std::cout << "invalid option [" << optarg << "] defaulting to [" << rateReq << "]" << std::endl;
				}
				break;
			case 'r' :
				tempArg = atoi(optarg);
				if (tempArg != 0) {
					rateSec = tempArg;
				} else {
					std::cout << "invalid option [" << optarg << "] defaulting to [" << rateSec << "]" << std::endl;
				}
				break;
			case 'u' :
				tempArg = atoi(optarg);
				if (tempArg != 0) {
					maxUsers = tempArg;
				} else {
					std::cout << "invalid option [" << optarg << "] defaulting to [" << maxUsers << "]" << std::endl;
				}
				break;
			case 't':
				tempArg = atoi(optarg);
				if (tempArg != 0) {
					timeout = tempArg;
				} else {
					std::cout << "invalid option [" << optarg << "] defaulting to [" << timeout << "]" << std::endl;
				}
				break;
		}
	}

	Log logger = Log(port, rateReq, rateSec, maxUsers, timeout); // output hardcoded to Log.txt
	std::cout << logger.serverStarted();

	// create the server socket
	int server_socket;
	server_socket = socket(AF_INET, SOCK_STREAM, 0);

	// define the server address
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(port);
	server_address.sin_addr.s_addr = INADDR_ANY;

	// bind the socket to our specified IP and port
	bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address));

	listen(server_socket, 5);
	struct sockaddr_in newAddr;
	int client_socket;
	int childpid = 1;
	socklen_t addr_size;

	// timeout
	struct timeval timeoutStruct;
	timeoutStruct.tv_sec = timeout;
	timeoutStruct.tv_usec = 0;

	while (true) {
		if (childpid > 0) { // only parent
			client_socket = accept(server_socket, (struct sockaddr *) &newAddr, &addr_size);
			if (client_socket < 0) {
				std::cout << logger.userDisconnected(inet_ntoa(newAddr.sin_addr), newAddr.sin_port);
				break;
			} else {
				std::cout << logger.successfulConnection(inet_ntoa(newAddr.sin_addr), newAddr.sin_port);
			}
			childpid = fork();
		}

		if (childpid == 0) {
			close(server_socket);

			setsockopt (client_socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeoutStruct, sizeof(timeoutStruct));

			int result = readImageFromClient("test.png", client_socket); // should provide flow control

            if (result == -1) {
	            std::cout << logger.userDisconnected(inet_ntoa(newAddr.sin_addr), newAddr.sin_port);
                break;
            } else if (result == 2) { // timeout
	            send(client_socket, &result, sizeof(int), 0);
            	std::cout << logger.userTimeout(inet_ntoa(newAddr.sin_addr), newAddr.sin_port);
	            close(client_socket);
            	break;
            }
            else{
                // convert QR code image to url
                std::string server_message = convertQRToURL("test.png");

                // if send image not a vaild QR code
                if(strstr(server_message.c_str(), "No barcode found") != NULL){
                    result = 1;
                }

                //send result code
                send(client_socket, &result, sizeof(int), 0);

                if (result == 1) { // invalid QR request
                    // log invalid QR request
	                std::cout << logger.invalidQRRequest(inet_ntoa(newAddr.sin_addr), newAddr.sin_port);
                } else if (result == 0) { //success
                    // parse message to only contain url
                    server_message = parseURL(const_cast<char*>(server_message.c_str()));
                    // length of message
                    int length = server_message.length();
                    // send message length
                    send(client_socket, &length, sizeof(int), 0);
                    // send message
                    send(client_socket, server_message.c_str(), server_message.size(), 0);
                    // log vaild QR request
	                std::cout << logger.validQRRequest(inet_ntoa(newAddr.sin_addr), newAddr.sin_port);
                }
            }
		}
	}
	close(client_socket);
	return 0;
}

