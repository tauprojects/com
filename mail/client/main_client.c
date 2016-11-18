/*
 * mail_client.c
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
#include <arpa/inet.h>
#include <assert.h>

#define DEFAULT_PORT 6423
#define TOTAL_MSG 4500
#define HOST_REPLY 100
#define DEFAULT_HOST "localhost"



char** str_split(char* a_str, const char a_delim)
{
    char** result    = 0;
    size_t count     = 0;
    char* tmp        = a_str;
    char* last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;

    /* Count how many elements will be extracted. */
    while (*tmp)
    {
        if (a_delim == *tmp)
        {
            count++;
            last_comma = tmp;
        }
        tmp++;
    }

    /* Add space for trailing token. */
    count += last_comma < (a_str + strlen(a_str) - 1);

    /* Add space for terminating null string so caller
       knows where the list of returned strings ends. */
    count++;

    result = malloc(sizeof(char*) * count);

    if (result)
    {
        size_t idx  = 0;
        char* token = strtok(a_str, delim);

        while (token)
        {
            assert(idx < count);
            *(result + idx++) = strdup(token);
            token = strtok(0, delim);
        }
        assert(idx == count - 1);
        *(result + idx) = 0;
    }

    return result;
}

//int main()
//{
//    char months[] = "JAN,FEB,MAR,APR,MAY,JUN,JUL,AUG,SEP,OCT,NOV,DEC";
//    char** tokens;
//
//    printf("months=[%s]\n\n", months);
//
//    tokens = str_split(months, ',');
//
//    if (tokens)
//    {
//        int i;
//        for (i = 0; *(tokens + i); i++)
//        {
//            printf("month=[%s]\n", *(tokens + i));
//            free(*(tokens + i));
//        }
//        printf("\n");
//        free(tokens);
//    }
//
//    return 0;
//}

int main(int argc, char* argv[]) {
	//Checking argument state
	char *hostname;
	int port;
	if (argc == 1) {
		hostname = DEFAULT_HOST;
		port = DEFAULT_PORT;
	} else if (argc == 2) {
		hostname = argv[1];
		port = DEFAULT_PORT;
	} else if (argc == 3) {
		hostname = argv[1];
		port = atoi(argv[2]);
	} else {
		return -1; //exit(1)
	}

	int sock;
	struct sockaddr_in server;
	char message[TOTAL_MSG], server_reply[HOST_REPLY];
	//Create socket
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1) {
		printf("Failed to create the listen socket: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_family = AF_INET;
	server.sin_port = htons(port);

	//Connect to remote server
	if (connect(sock, (struct sockaddr *) &server, sizeof(server)) < 0) {
		printf("Failed to connect to server: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
		close(sock);
	}
	//keep communicating with server
	while (1) {
		printf("Enter Command : ");
		fgets(message, TOTAL_MSG, stdin);
		char** splitArgs=str_split(message,' ');
		printf("%s      ", splitArgs[0]);
		printf("%s=      ",splitArgs[1]);
		printf("END     ");
		if(strcmp(message,"QUIT")==0){
			send(sock, message, strlen(message), 0);
			break;
		}
		//Send some data
		if (send(sock, message, strlen(message), 0) < 0) {
			printf("Failed to send to server: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
			close(sock);
		}

		//Receive a reply from the server
		if (recv(sock, server_reply, 2000, 0) < 0) {
			printf("Failed to received to server: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
			close(sock);
		}

		puts("Server reply :");
		puts(server_reply);
	}

	close(sock);
	return 0;

// after he get an "welcome" messages he will send user&password

}

