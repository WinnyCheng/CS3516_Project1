#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>

/**
 * Sends an image from client to server
 *
 * @param imgURL - The path to the image
 * @param socket - The file descriptor of the socket
 *
 * SOURCE: https://stackoverflow.com/questions/5638831/c-char-array
 * Author Username: @Cubbi
 */
void sendImageToSocket(const char *imgURL, int socket) {
	FILE *image = fopen(imgURL, "r");
	fseek(image, 0, SEEK_END);
	int size = ftell(image);
	fseek(image, 0, SEEK_SET);

	//Send Picture Size
	printf("Sending Picture Size %d\n", size);
	write(socket, &size, sizeof(size));

	//Send Picture as Byte Array
	printf("Sending Picture as Byte Array\n");
	char send_buffer[size];
	fread(send_buffer, 1, sizeof(send_buffer), image);
	write(socket, send_buffer, sizeof(send_buffer));
	bzero(send_buffer, sizeof(send_buffer));
}

int main(int argc, char *argv[]) {
	int port = 2012;
	std::string address = "127.0.0.1";
	std::string file = "";

	static struct option long_options[] = {
			{"PORT",    optional_argument, 0, 'p'},
			{"ADDRESS", optional_argument, 0, 'a'},
			{"FILE",    optional_argument, 0, 'f'},
			{0, 0, 0, 0}
	};

	int long_index = 0;
	int opt;
	int tempArg;
	while ((opt = getopt_long(argc, argv, "paf::", long_options, &long_index)) != -1) {

		switch (opt) {
			case 'p' :
				tempArg = atoi(optarg);
				if (tempArg != 0 && 2000 <= tempArg && tempArg <= 3000) {
					port = tempArg;
				} else {
					std::cout << "invalid option [" << optarg << "] defaulting to [" << port << "]" << std::endl;
				}
				break;
			case 'a' :
				address = optarg;
				break;
			case 'f' :
				file = optarg;
				break;
		}
	}

	if (strcmp(file.c_str(), "") == 0) {
		std::cout << "No file specified! Use --FILE=[filePath]" << std::endl;
		return -1;
	}

	// create a socket
	int network_socket;
	network_socket = socket(AF_INET, SOCK_STREAM, 0);

	// specify an address for the socket
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(9002);
	server_address.sin_addr.s_addr = INADDR_ANY;

	int connection_status = connect(network_socket, (struct sockaddr *) &server_address, sizeof(server_address));
	// check for error with the connection
	if (connection_status == -1) {
		printf("There was an error making a connection to the remote socket");
	}
	while (strcmp(file.c_str(), "q") != 0) {
		//send image
		sendImageToSocket(file.c_str(), network_socket);

		int code;
		recv(network_socket, &code, sizeof(int), 0);

		if(code == 0){
			int length;
			recv(network_socket, &length, sizeof(int), 0);

			// receive data from the server
			char server_response[length];
			recv(network_socket, &server_response, sizeof(server_response), 0);

			// print out the server's response
			printf("Parsed URL: %s\n", server_response);
		}
		else if(code == 1){
			printf("Invalid request or violated network security\n");
		}
		else if(code == 2){
			printf("Max connection time without interaction exceeded. Connection has been closed\n");
			break;
		}
		else if(code == 3){
			printf("Too many request! The rate limit was exceed. Please try again later.\n");
		}
		std::cout << "\nEnter a new image path or 'q' to exit: ";
		std::cin >> file;
	}

	// and the close the socket
	close(network_socket);

	return 0;
}
