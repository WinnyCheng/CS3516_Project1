#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
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
 * SOURCE: https://stackoverflow.com/questions/5638831/c-char-array
 * Author Username: @Cubbi
 */
void readImageFromClient(const char* outputURL, int socket) {
	//Read Picture Size
	printf("Reading Picture Size\n");
	int size;
	read(socket, &size, sizeof(int));

	//Read Picture Byte Array
	printf("Reading Picture Byte Array\n");
	char p_array[size];
	read(socket, p_array, size);

	//Convert it Back into Picture
	printf("Converting Byte Array to Picture\n");
	FILE *image;
	image = fopen(outputURL, "w");
	fwrite(p_array, 1, sizeof(p_array), image);
	fclose(image);
}

/**
 * Executes command in unix shell and then reads standard out and returns the value
 *
 * SOURCE: https://stackoverflow.com/questions/478898/how-do-i-execute-a-command-and-get-the-output-of-the-command-within-c-using-po
 * AUTHOR USERNAME: gregpaton08
 */
std::string exec(const char* cmd) {
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

std::string convertQRToURL(const char* url) {
	char command[100];
	strcpy(command, "java -cp \"javase.jar;core.jar\" com.google.zxing.client.j2se.CommandLineRunner ");
	strcat(command, url);
	return(exec(command));
}

int main(int argc, char *argv[]) {

	// Defaults
	int port = 2012;
	int rateReq = 3; // x amount of requests
	int rateSec = 60; // per x amount of seconds per user
	int maxUsers = 3;
	int timeout = 80; // seconds


	static struct option long_options[] = {
			{"PORT", optional_argument, 0,  'a'},
			{"RATE_MSGS", optional_argument, 0,  'm'},
			{"RATE_TIME", optional_argument, 0,  'r'},
			{"MAX_USERS", optional_argument, 0,  'u'},
			{"TIMEOUT", optional_argument, 0,  't'},
			{0, 0, 0, 0}
	};

	int long_index =0;
	int opt;
	int tempArg;
	while ((opt = getopt_long(argc, argv,"amrut::",long_options, &long_index )) != -1) {

		switch (opt) {
			case 'a' :
				tempArg = atoi(optarg);
				if (tempArg != 0 && 2000 <= tempArg && tempArg <= 3000) {
					port = tempArg;
				} else {
					std::cout << "invalid option [" << optarg <<"] defaulting to [" << port << "]" << std::endl;
				}
				break;
			case 'm' :
				tempArg = atoi(optarg);
				if (tempArg != 0) {
					rateReq = tempArg;
				} else {
					std::cout << "invalid option [" << optarg <<"] defaulting to [" << rateReq << "]" << std::endl;
				}
				break;
			case 'r' :
				tempArg = atoi(optarg);
				if (tempArg != 0) {
					rateSec = tempArg;
				} else {
					std::cout << "invalid option [" << optarg <<"] defaulting to [" << rateSec << "]" << std::endl;
				}
				break;
			case 'u' :
				tempArg = atoi(optarg);
				if (tempArg != 0) {
					maxUsers = tempArg;
				} else {
					std::cout << "invalid option [" << optarg <<"] defaulting to [" << maxUsers << "]" << std::endl;
				}
				break;
			case 't':
				tempArg = atoi(optarg);
				if (tempArg != 0) {
					timeout = tempArg;
				} else {
					std::cout << "invalid option [" << optarg <<"] defaulting to [" << timeout << "]" << std::endl;
				}
				break;
		}
	}

	std::cout << std::to_string(port) << " " << std::to_string(rateReq) << " " << std::to_string(rateSec) << " " <<
	std::to_string(maxUsers) << " " << std::to_string(timeout) << std::endl;

	Log logger = Log(port, rateReq, rateSec, maxUsers, timeout); // output hardcoded to Log.txt
	logger.serverStarted();

	// create the server socket
	int server_socket;
	server_socket = socket(AF_INET, SOCK_STREAM, 0);

	// define the server address
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(9002);
	server_address.sin_addr.s_addr = INADDR_ANY;

	// bind the socket to our specified IP and port
	bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address));

	listen(server_socket, 5);

	int client_socket = accept(server_socket, NULL, NULL);

	readImageFromClient("test.png", client_socket);

	// send the message
	std::string server_message = convertQRToURL("test.png");
	std::cout << "!!!!!! " << server_message << "@@@@@";
	send(client_socket, server_message.c_str(), server_message.size(), 0);

	// close the socket
	close(server_socket);

	return 0;
}

