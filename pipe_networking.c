#include "pipe_networking.h"


/*=========================
	server_handshake
	args: int * to_client

	Performs the client side pipe 3 way handshake.
	Sets *to_client to the file descriptor to the downstream pipe.

	returns the file descriptor for the upstream pipe.
	=========================*/
int server_handshake(int *to_client) {
	mkfifo(WKP, 0777);
	char privname[HANDSHAKE_BUFFER_SIZE];
	for (int i = 0; i < HANDSHAKE_BUFFER_SIZE; i++) privname[i]=0;
	int from_client = open(WKP, O_RDONLY, 0777);
	read(from_client, privname, HANDSHAKE_BUFFER_SIZE);
	// printf("Server: received [%s] from client\n", privname);

	remove(WKP);
	// printf("Server: removed WKP\n");

	*to_client = open(privname, O_WRONLY);
	write(*to_client, ACK, HANDSHAKE_BUFFER_SIZE);
	// printf("Server: wrote [%s] to client\n", ACK);

	char message[HANDSHAKE_BUFFER_SIZE];
	for (int i = 0; i < HANDSHAKE_BUFFER_SIZE; i++) message[i] = 0;
	read(from_client, message, HANDSHAKE_BUFFER_SIZE);
	if (strcmp(message, ACK)) {
		printf("Server: handshake failed - expected [%s], received [%s]\n", ACK, message);
		return 0;
	}
	// printf("Server: received [%s] from client\n", message);
	printf("3-way handshake established\n");
	return from_client;
}


/*=========================
	client_handshake
	args: int * to_server

	Performs the client side pipe 3 way handshake.
	Sets *to_server to the file descriptor for the upstream pipe.

	returns the file descriptor for the downstream pipe.
	=========================*/
int client_handshake(int *to_server) {
	char name[HANDSHAKE_BUFFER_SIZE];
	name[0]=0;
	sprintf(name, "%d", getpid());
	mkfifo(name, 0777);
	*to_server = open(WKP, O_WRONLY, 0777);
	write(*to_server, name, HANDSHAKE_BUFFER_SIZE);
	// printf("Client: wrote [%s] to server\n", name);

	int from_server = open(name, O_RDONLY, 0777);
	char message[HANDSHAKE_BUFFER_SIZE];
	for (int i = 0; i < HANDSHAKE_BUFFER_SIZE; i++) message[i] = 0;
	read(from_server, message, HANDSHAKE_BUFFER_SIZE);
	if (strcmp(message, ACK)) {
		printf("Client: Handshake failed - expected [%s], received [%s]\n", ACK, message);
		return 0;
	}
	// printf("Client: received [%s] from server\n", message);
	remove(name);
	// printf("Client: removed private FIFO\n");
	write(*to_server, message, HANDSHAKE_BUFFER_SIZE);
	// printf("Client: Wrote [%s] to server\n", message);
	printf("3-way handshake established\n");
	return from_server;
}
