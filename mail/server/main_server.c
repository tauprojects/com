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
#include <sys/socket.h>
#include <netinet/in.h>
#include "mail_server.h"

#define DEFAULT_PORT 6423
#define NUM_OF_USERS 1

int listenSd;   // The listen socket, defined as global for the code that exit with failure


int main(int argc, char* argv[]) {
	//Checking argument state
	char *users_file;
	int port;
	if (argc == 2) {
		users_file=argv[1];
		port=DEFAULT_PORT;
	} else if (argc == 3 ) {
		users_file=argv[1];
		port=atoi(argv[2]);
	} else {
		return -1; //exit(1)
	}
	UsersDB users = UsersDBCreate(users_file);

    listenSd = socket(AF_INET, SOCK_STREAM, 0); // For type=SOCK_STREAM protocol 0 is TCP
    if (listenSd == -1){
        printf("Failed to create the listen socket: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in myaddr,their_addr;
    int sin_size;
    myaddr.sin_family = AF_INET;
    myaddr.sin_port = htons( port );
    myaddr.sin_addr = htonl( INADDR_ANY );

    if(bind(listenSd, &myaddr, sizeof(myaddr))){   // Bind to the socket
    	printf("Failed to bind to the listen socket: %s\n", strerror(errno));
    	close(listenSd);
    	exit(EXIT_FAILURE);
    }

    if (listen(listenSd, NUM_OF_USERS) == -1) {   // Listen to the socket
        printf("Failed to listen to the listen socket: %s\n", strerror(errno));
        close(listenSd);
        exit(EXIT_FAILURE);
    }

    int connSd = accept(listenSd, (struct sockaddr*) &their_addr, &sin_size);  // Accept a connection
    if (connSd == -1){
        printf("Failed to accept a connection: %s\n", strerror(errno));
        close(listenSd);
        exit(EXIT_FAILURE);
    }
    int read_size;
        char *message , client_message[2000];
    //Receive a message from client
        while( (read_size = recv(connSd , client_message , 2000 , 0)) > 0 )
        {
            //Send the message back to client
            write(connSd , client_message , strlen(client_message));
        }

        if(read_size == 0)
        {
            puts("Client disconnected");
            fflush(stdout);
        }
        else if(read_size == -1)
        {
            perror("recv failed");
        }
    close(connSd);
    close(listenSd);
	return 0;
}




