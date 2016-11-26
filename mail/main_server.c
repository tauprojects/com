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


//Functions Deceleration
int getOpcode(char* client_message);
int parseId(char* message);


//Enum Message Operation Code Declarations
typedef enum USER_COMMAND {
	AUTH,
	SHOW_INBOX,
	GET_MAIL,
	DELETE_MAIL,
	COMPOSE,
	QUIT

} USER_CMD;


int listenSd; // The listen socket, defined as global for the code that exit with failure

int main(int argc, char* argv[]) {
	char *users_file;
	int port;
	int opCode;
	int read_size;
	int totalMailSize =MAX_SUBJECT + MAX_CONTENT + TOTAL_TO*MAX_USERNAME;
	char client_message[totalMailSize];
	char* username = (char*) malloc(sizeof(char) * MAX_USERNAME);
	char* message;(char*) malloc(sizeof(char) * 2000);
	USER user;
	USER *users=(USER*)malloc(sizeof(USER)*NUM_OF_CLIENTS);



	//Checking argument state
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



	//Create Users DataBase
	int sizeOfUsers = UsersDBCreate(users_file,users);
	if(sizeOfUsers<0){
		printf("Error While Parsing user.tx File");
	}



	//Listening Flow....
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


	//Receive a message from client
	while ((read_size = recv(connSd, client_message, totalMailSize, 0)) > 0) {

		//Get Opcode of the first message in order to authenticate
		opCode = getOpcode(client_message);

		if(opCode!=AUTH){
			printf("Error In Client Protocol Implementation - Server Expected Auth Message");
			break;
		}

		//Handle Cases if user is authenticated
		else if(auth_user(&client_message[1],username)){
			user=  getUser(users,sizeOfUsers,username);
			int id,ack;
			while (1) {
				switch (opCode) {

				//Case 1 - Show Inbox Of the Authenticated Username
				//client_message = "1\t"
				case SHOW_INBOX:
					//1
					//Handle Case when no mails in DB!
					ack = showInbox(user,message);
					if(ack!=0){
						printf("Error While Getting Inbox for Username: %s  - Internal Server Error",username);
						break;
					}
					break;  //T0DO


				//Case 2 - Get Mail Relative to ID number
				//client_message = "2id\t" - Handle the Messeage
				case GET_MAIL:
						id = parseId(&client_message[1]);
					    ack = getMail(user,id,message); //Parse mail ID
						switch (ack) {
						case -1:
							printf("Error While Getting Mail ID Number: %d - Not Found in DB",id);
							break;
						case -2:
							printf("Error While Getting Mail ID Number: %d  - Mail Marked as Trash",id);
							break;
						case -3:
							printf("Error While Getting Mail ID Number: %d  - Internal Server Error",id);
							break;
						}
						break;

				//Case 3 - Delete Mail Relative to ID number
				//client_message = "3id\t" - Handle the retuen Messeage
				case DELETE_MAIL:
					id = parseId(&client_message[1]);
					ack = deleteMail(user, id); //T0DO
					if(ack!=0){
						printf("Error While Deleting Mail ID Number: %d - Not Found in DB",id);
					}
					break;

				//Case 4 - Compose Mail
				//client_message = "4content\0"
				case COMPOSE:;  //Empty statement due to Unallowed declaration (stackoverflow)
					MAIL mail = (MAIL) malloc(sizeof(mail));
					char* unParsedMail = (char*) malloc(sizeof(char) * 1024);
					strcat(unParsedMail, &client_message[1]);
					parseMail(unParsedMail, username, mail);
					printMail(mail);
					break;

				//Case 5 - Compose Mail
				//client_message = "5\t"
				case QUIT:
					break;
				printf("Error In Client Protocol Implementation-Server Expected Operation Message");
				}
			}
		}
		write(connSd, message, strlen("Connected to server‬‬"));
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

int getOpcode(char* client_message){
	char* temp = (char*) malloc(sizeof(char) * 5);
	strncpy(temp, client_message, 1);
	int opCode = atoi(temp);
	printf("\nstate is: %d", opCode);
//	free(temp);
	return opCode;
}
int parseId(char* message){
	char* temp = strchr(message,'\t');
	int id = atoi(message);
	return id;
}


