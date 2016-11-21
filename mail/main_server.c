/*
 * main_server.c
 *
 *  Created on: Nov 12, 2016
 *      Author: lirongazit
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <assert.h>
#include "mail_server.h"




#define NUM_OF_USERS 1
#define HELLO_MSG "‫!‪Welcome‬‬ ‫‪I‬‬ ‫‪am‬‬ ‫‪simple-mail-server.‬‬"


//char** str_split(char* a_str, const char a_delim,int* countStr);

int decInputState(char* client_message);

//char** str_split(char* a_str, const char a_delim,int* countStr);


typedef enum USER_COMMAND {
	AUTH,
	SHOW_INBOX,
	GET_MAIL,
	DELETE_MAIL,
	QUIT,
	COMPOSE
} USER_CMD;


int listenSd; // The listen socket, defined as global for the code that exit with failure

int main(int argc, char* argv[]) {
	//Checking argument state
	char *users_file;
	int port;
	if (argc == 2) {
		users_file = argv[1];
		port = DEFAULT_PORT;
	} else if (argc == 3) {
		users_file = argv[1];
		port = atoi(argv[2]);
	} else {
		puts("invalid number of arguments");
		return -1; //exit(1)
	}
//	UsersDB users = UsersDBCreate(users_file);

	listenSd = socket(AF_INET, SOCK_STREAM, 0); // For type=SOCK_STREAM protocol 0 is TCP
	if (listenSd == -1) {
		printf("Failed to create the listen socket: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	struct sockaddr_in myaddr, their_addr;
	int sin_size;
	myaddr.sin_family = AF_INET;
	myaddr.sin_port = htons(port);
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(listenSd, (struct sockaddr*) &myaddr, sizeof(myaddr))) { // Bind to the socket
		printf("Failed to bind to the listen socket: %s\n", strerror(errno));
		close(listenSd);
		exit(EXIT_FAILURE);
	}

	if (listen(listenSd, NUM_OF_USERS) == -1) {   // Listen to the socket
		printf("Failed to listen to the listen socket: %s\n", strerror(errno));
		close(listenSd);
		exit(EXIT_FAILURE);
	}

	int connSd = accept(listenSd, (struct sockaddr*) &their_addr,
			(socklen_t*) &sin_size);  // Accept a connection
	if (connSd == -1) {
		printf("Failed to accept a connection: %s\n", strerror(errno));
		close(listenSd);
		exit(EXIT_FAILURE);
	}
	write(connSd, HELLO_MSG, strlen(HELLO_MSG));
	int read_size;
	int totalMailSize =MAX_SUBJECT + MAX_CONTENT + TOTAL_TO*MAX_USERNAME;
	char client_message[totalMailSize];
//	char* message = (char*)malloc(sizeof(char)*128);

	//Receive a message from client
	while ((read_size = recv(connSd, client_message, totalMailSize, 0)) > 0) {
		//Send the message back to client
//		parse_input = str_split((char*)client_message," ",&cnt);
//		message = decInput(client_message);
		write(connSd, "Mati", strlen("Connected to server‬‬"));
	}

	if (read_size == 0) {
		puts("Client disconnected");
		fflush(stdout);
	} else if (read_size == -1) {
		perror("recv failed");
	}
	close(connSd);
	close(listenSd);
	return 0;
}


int decInputState(char* client_message) {
	char* temp = (char*) malloc(sizeof(char) * 5);
	strncpy(temp, client_message, 1);
	int opCode = atoi(temp);
	printf("\nstate is: %d", opCode);
	char* username = (char*) malloc(sizeof(char) * MAX_USERNAME);

	switch (opCode) {
	case 0:
//		0User: Mati    Password:correctS
		printf("Authenticate!\n");
		auth_user(&client_message[1],username); //T0DO
		break;
	case 1:
		printf("Show Inbox!\n");
//		showInbux(client_message); //T0DO
	case 2:
		printf("get mail\n");
//		getMail(client_message); //T0DO
		break;
	case 3:
		printf("delete mail\n");
//		deleteMail(client_message); //T0DO
		break;
	case 4:
		printf("\ncompose\n");
		MAIL mail = (MAIL) malloc(sizeof(mail));
		char* unpare  = (char*) malloc(sizeof(char) * 1024);
		strcat(unpare,&client_message[1]);
		parseMail(unpare,username,mail); //T0DO
		printMail(mail);
		break;
	case 5:
		printf("quit\n");
		break;
	default:
		printf("quit\n");
	}
//	free(temp);
	return opCode;
}


