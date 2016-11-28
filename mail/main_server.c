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
#include <unistd.h>
#include <fcntl.h>
#include "mail_server.h"

#define NUM_OF_USERS 1
#define CHUNK_SIZE 512

#define HELLO_MSG "Welcome! I am simple-mail-server"
#define SUC_AUTH "Connected to server‬‬"
#define FAIL_AUTH "failed to connect to server"

//Functions Deceleration
int getOpcode(char* client_message);
int parseId(char* message);

//Enum Message Operation Code Declarations
typedef enum USER_COMMAND {
	AUTH, SHOW_INBOX, GET_MAIL, DELETE_MAIL, COMPOSE, QUIT

} USER_CMD;

int listenSd; // The listen socket, defined as global for the code that exit with failure
//bool sendall(int socket, void *buffer, size_t length);
int main(int argc, char* argv[]) {
	char *users_file;
	int port;
	int opCode;
	int read_size;
	int totalMailSize = 4500;
	int sizeOfUsers;
	TOTAL_MAILS mailsDB=(TOTAL_MAILS)malloc(sizeof(*mailsDB));
	mailsDB->mails=(MAIL*)malloc(sizeof(MAIL)*MAXMAILS);
	mailsDB->size=0;
	//= MAX_SUBJECT + MAX_CONTENT + TOTAL_TO * MAX_USERNAME;
	char client_message[totalMailSize];
	char* username = (char*) malloc(sizeof(char) * MAX_USERNAME);
	char* message = (char*) calloc(2000, sizeof(char));
	USER user;
	USER *users = (USER*) malloc(sizeof(USER) * NUM_OF_CLIENTS);

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
//	int sizeOfUsers = UsersDBCreate(users_file, users);
//	prinUsersDb(users, sizeOfUsers);


	//Create Users DataBase
	if( access( users_file, F_OK ) != -1 ) {
		sizeOfUsers = UsersDBCreate(users_file, users);
		prinUsersDb(users, sizeOfUsers);
	} else {
	   printf("Error While Accessing Users Data File: %s\n",users_file);
		exit(1);
	}

	if (sizeOfUsers < 0) {
		printf("Error While Parsing %s File", users_file);
		exit(1);
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
	int thereAreUsers = 1;
	while (thereAreUsers) {
		int connSd = accept(listenSd, (struct sockaddr*) &their_addr,
				(socklen_t*) &sin_size);  // Accept a connection
		if (connSd == -1) {
			printf("Failed to accept a connection: %s\n", strerror(errno));
			close(listenSd);
			exit(EXIT_FAILURE);
		}
		strncpy(message, HELLO_MSG, strlen(HELLO_MSG));
//	write(connSd, HELLO_MSG, strlen(HELLO_MSG));
//	memset(message,0,strlen(message));
		write(connSd, message, strlen(message));
		int authFlag = 1, totalRcv;
		while (authFlag) {
			memset(client_message, 0, 4500);
			printf("Ready For USER\n");
			if (recv(connSd, client_message, 4500, 0) < 0) {
				printf("Failed to receive from server: %s\n", strerror(errno));
				exit(EXIT_FAILURE);
			} else {
				printf("MSG: %s\n", client_message);
				if (auth_user(&client_message[5], username, users,
						sizeOfUsers)) {
					write(connSd, SUC_AUTH, strlen(SUC_AUTH));
					authFlag = 0;
					break;
				} else {
					write(connSd, FAIL_AUTH, strlen(FAIL_AUTH));
				}
			}
		}
		int quitFlag = 1;
		while (quitFlag) {
			int id, ack;
			memset(client_message, 0, 4500);
			if (recv(connSd, client_message, 4500, 0) < 0) {
				printf("Failed to receive from server: %s\n", strerror(errno));
				exit(EXIT_FAILURE);
			} else {
				opCode = getOpcode(client_message);
				printf("OC: %d\n", opCode);
				switch (opCode) {
				//Case 1 - Show Inbox Of the Authenticated Username
				//client_message = "1\t"
				case SHOW_INBOX:
//				//1
//				//Handle Case when no mails in DB!
//				ack = showInbox(user, message);
//				if (ack != 0) {
//					printf(
//							"Error While Getting Inbox for Username: %s  - Internal Server Error",
//							username);
//					break;
//				}
					write(connSd, "SHOW_INBOX\n", 4500);
					break;  //T0DO

					//Case 2 - Get Mail Relative to ID number
					//client_message = "2id\t" - Handle the Messeage
				case GET_MAIL:
				id = parseId(&client_message[5]);
				fflush(NULL);
				printf("id : %d\n", id);
				ack = getMail(user,mailsDB, id, message); //Parse mail ID
				fflush(NULL);
				printf("ack : %d\n", ack);
				switch (ack) {
				case -1:
					printf(
							"Error While Getting Mail ID Number: %d - Not Found in DB",
							id);
					break;
				case -2:
					printf(
							"Error While Getting Mail ID Number: %d  - Mail Marked as Trash",
							id);
					break;
				case -3:
					printf(
							"Error While Getting Mail ID Number: %d  - Internal Server Error",
							id);
					break;
				}
					write(connSd, "GET_MAIL\n", 4500);

					break;

					//Case 3 - Delete Mail Relative to ID number
					//client_message = "3id\t" - Handle the retuen Messeage
				case DELETE_MAIL:
//				id = parseId(&client_message[5]);
//				ack = deleteMail(user, id); //T0DO
//				if (ack != 0) {
//					printf(
//							"Error While Deleting Mail ID Number: %d - Not Found in DB",
//							id);
//				}
					write(connSd, "DELETE_MAIL\n", 4500);
					break;

					//Case 4 - Compose Mail
					//client_message = "4content\0"
				case COMPOSE:
					; //Empty statement due to Unallowed declaration (stackoverflow)
					MAIL mail = (MAIL) malloc(sizeof(mail));
					char* unParsedMail = (char*) malloc(sizeof(char) * 4500);
					strcat(unParsedMail, &client_message[5]);
					parseMail(unParsedMail, username, mail);
					printMail(mail);
	//				write(connSd, "COMPOSE\n", 4500);

					break;

					//Case 5 - Compose Mail
					//client_message = "5\t"
				case QUIT:
					quitFlag = 0;
					break;
					printf(
							"Error In Client Protocol Implementation-Server Expected Operation Message");

				}
			}
		}
		close(connSd);
	}
//		memset(client_message,0,strlen(client_message));
//		read_size = recv(connSd, client_message, 1, 0);
//		printf("MSG: %s\n",client_message);
//		fflush(NULL);
//		char a[1];
//		strncpy(a,client_message,1);
//		opCode = atoi(a);
//		memset(client_message,0,strlen(client_message));
//		read_size =recv(connSd, client_message, 4, 0);
//		printf("MSG: %s\n",client_message);
//		totalRcv = atoi(client_message);
//		memset(client_message,0,strlen(client_message));
//		read_size = recv(connSd, client_message, totalRcv, 0);
//		printf("MSG: %s\n",client_message);
//		printf("%d %d",totalRcv,read_size);
//		if (opCode != AUTH) {
//			printf(
//					"Error In Client Protocol Implementation - Server Expected Auth Message");
//		//	break;
//		}
//		//Handle Cases if user is authenticated
//		else if (auth_user(&client_message[1], username, users, sizeOfUsers)) {
//			puts("im ok");
//			//user = getUser(users,sizeOfUsers,username);
//			authFlag = 0;
//			write(connSd, SUC_AUTH, strlen(SUC_AUTH));
//			puts("123");
//			fflush(NULL);
//		} else {
//			write(connSd, FAIL_AUTH, strlen(FAIL_AUTH));
//		}
//		//Receive a message from client
//		while ((read_size = recv(connSd, client_message, totalMailSize, 0)) > 0
//				&& authFlag == 0) {
//
//		opCode = getOpcode(client_message);
//		int id,ack;
//Get Opcode of the first message in order to authenticate
//			while (1) {
//				switch (opCode) {
//
//				//Case 1 - Show Inbox Of the Authenticated Username
//				//client_message = "1\t"
//				case SHOW_INBOX:
//					//1
//					//Handle Case when no mails in DB!
//					ack = showInbox(user,message);
//					if(ack!=0){
//						printf("Error While Getting Inbox for Username: %s  - Internal Server Error",username);
//						break;
//					}
//					break;  //T0DO
//
//
//				//Case 2 - Get Mail Relative to ID number
//				//client_message = "2id\t" - Handle the Messeage
//				case GET_MAIL:
//						id = parseId(&client_message[1]);
//					    ack = getMail(user,id,message); //Parse mail ID
//						switch (ack) {
//						case -1:
//							printf("Error While Getting Mail ID Number: %d - Not Found in DB",id);
//							break;
//						case -2:
//							printf("Error While Getting Mail ID Number: %d  - Mail Marked as Trash",id);
//							break;
//						case -3:
//							printf("Error While Getting Mail ID Number: %d  - Internal Server Error",id);
//							break;
//						}
//						break;
//
//				//Case 3 - Delete Mail Relative to ID number
//				//client_message = "3id\t" - Handle the retuen Messeage
//				case DELETE_MAIL:
//					id = parseId(&client_message[1]);
//					ack = deleteMail(user, id); //T0DO
//					if(ack!=0){
//						printf("Error While Deleting Mail ID Number: %d - Not Found in DB",id);
//					}
//					break;
//
//				//Case 4 - Compose Mail
//				//client_message = "4content\0"
//				case COMPOSE:;  //Empty statement due to Unallowed declaration (stackoverflow)
//					MAIL mail = (MAIL) malloc(sizeof(mail));
//					char* unParsedMail = (char*) malloc(sizeof(char) * 1024);
//					strcat(unParsedMail, &client_message[1]);
//					parseMail(unParsedMail, username, mail);
//					printMail(mail);
//					break;
//
//				//Case 5 - Compose Mail
//				//client_message = "5\t"
//				case QUIT:
//					break;
//				printf("Error In Client Protocol Implementation-Server Expected Operation Message");
//				}
//			}
//		}
//		write(connSd, message, strlen("Connected to server‬‬"));
//	printf("%s\n", client_message);
//	write(connSd, client_message, strlen(client_message));
//
//	//	}
////	}
//	if (read_size == 0) {
//		puts("Client disconnected");
//		fflush(stdout);
//	} else if (read_size == -1) {
//		perror("recv failed");
//	}
	free(username);
	free(message);
	freeMailDB(mailsDB);
	close(listenSd);
	for(int j=0;j<sizeOfUsers;j++){
		freeUser(users[j]);
	}
	free(users);
	freeUser(user);
	return 0;
}

int getOpcode(char* client_message) {
	char* temp = (char*) malloc(sizeof(char) * 5);
	strncpy(temp, client_message, 1);
	int opCode = atoi(temp);
//	printf("\nstate is: %d", opCode);
	free(temp);
	return opCode;
}

int parseId(char* message) {
	char* temp = strchr(message, '\t');
	int id = atoi(message);
	return id;
}

//bool sendall(int socket, void *buffer, size_t length)
//{
//    char *ptr = (char*) buffer;
//    while (length > 0)
//    {
//        int i = send(socket, ptr, length);
//        if (i < 1) return false;
//        ptr += i;
//        length -= i;
//    }
//    return true;
//}

//int recv_timeout(int s , int timeout)
//{
//    int size_recv , total_size= 0;
//    struct timeval begin , now;
//    char chunk[CHUNK_SIZE];
//    double timediff;
//
//    //make socket non blocking
//    fcntl(s, F_SETFL, O_NONBLOCK);
//
//    //beginning time
//    gettimeofday(&begin , NULL);
//
//    while(1)
//    {
//        gettimeofday(&now , NULL);
//
//        //time elapsed in seconds
//        timediff = (now.tv_sec - begin.tv_sec) + 1e-6 * (now.tv_usec - begin.tv_usec);
//
//        //if you got some data, then break after timeout
//        if( total_size > 0 && timediff > timeout )
//        {
//            break;
//        }
//
//        //if you got no data at all, wait a little longer, twice the timeout
//        else if( timediff > timeout*2)
//        {
//            break;
//        }
//
//        memset(chunk ,0 , CHUNK_SIZE);  //clear the variable
//        if((size_recv =  recv(s , chunk , CHUNK_SIZE , 0) ) < 0)
//        {
//            //if nothing was received then we want to wait a little before trying again, 0.1 seconds
//            usleep(100000);
//        }
//        else
//        {
//            total_size += size_recv;
//            printf("%s" , chunk);
//            //reset beginning time
//            gettimeofday(&begin , NULL);
//        }
//    }
//
//    return total_size;
//}
//
