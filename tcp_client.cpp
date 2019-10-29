#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>

/**
 * Sends an image from client to server
 *
 * @param imgURL - The path to the image
 * @param socket - The file descriptor of the socket
 *
 * SOURCE: https://stackoverflow.com/questions/5638831/c-char-array
 * Author Username: @Cubbi
 */
void sendImageToSocket(const char* imgURL, int socket) {
	FILE *image = fopen(imgURL, "r");
	fseek(image, 0, SEEK_END);
	int size = ftell(image);
	fseek(image, 0, SEEK_SET);

	//Send Picture Size
	printf("Sending Picture Size\n");
	write(socket, &size, sizeof(size));

	//Send Picture as Byte Array
	printf("Sending Picture as Byte Array\n");
	char send_buffer[size];
	while(!feof(image)) {
		fread(send_buffer, 1, sizeof(send_buffer), image);
		write(socket, send_buffer, sizeof(send_buffer));
		bzero(send_buffer, sizeof(send_buffer));
	}
}

int main() {

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
	if(connection_status == -1){
		printf("There was an error making a connection to the remote socket");
	}

	//send image
	sendImageToSocket("qr_example.png", network_socket);

	// receive data from the server
	char server_response[256];
	recv(network_socket, &server_response, sizeof(server_response), 0);

	// print out the server's response
	printf("The server sent the data. %s", server_response);

	// and the close the socket
	close(network_socket);

	return 0;
}
