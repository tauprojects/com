/*
 * mail_client.c
 * Created By: Liron Gazit, Lior Schneider, Matan Gizunterman
 * TAU Username: lironemilyg , gizunterman , schneider2
 * ID number: 305774382, 302814355, 303157804
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
#define HOST_REPLY 4500
#define AUTH_LEN 100
#define SUBJECT 100
#define TOT_TEXT 2350
#define STD_IN 0
#define DEFAULT_HOST "127.0.0.1"
#define QUIT_MSG "QUIT"
#define SQUIT_MSG "S_QUIT"
#define MSG "MSG"
#define SHOW_ONLINE "SHOW_ONLINE_USERS"
#define HELLO_MSG "Welcome! I am simple-mail-server"
#define AUTH_USR_ERR "Error-wrong format or too long authenticity key, the format should be: User: <10chars> try again or print \"QUIT\" to exit\n"
#define AUTH_PWD_ERR "Error-wrong format or too long authenticity key, the format should be: Password: <10chars> try again or print \"QUIT\" to exit\n"
#define USER_ATT "User:"
#define PASS_ATT "Password:"
#define TO_ATT "To:"
#define SUB_ATT "Subject:"
#define TXT_ATT "Text:"
#define SUC_AUTH "Connected to server‬‬"
#define BAD_AUTH "Bad Authentication parameters. try again or print \"QUIT\" to exit\n"
#define SHOW "SHOW_INBOX"
#define GET "GET_MAIL"
#define DEL "DELETE_MAIL"
#define COMP "COMPOSE"
#define WRNG_CMD "Wrong command. the options are: \"SHOW_INBOX\", \"GET_MAIL <id>\", \"DELETE_MAIL <id>\", \"COMPOSE\", \"SHOW_ONLINE_USERS\", \"MSG\" \n"
#define TO_ERR "Wrong format! Type \"To: <user1,user2, ..> \"\n"
#define SUB_ERR "Wrong format! Type \"Subject: <subject> \"\n"
#define TXT_ERR "Wrong format! Type \"Text: <text> \"\n"

int sock;

int inttostr(int i, char* str);
char** str_split(char* a_str, const char a_delim) {
	char** result = 0;
	size_t count = 0;
	char* tmp = a_str;
	char* last_comma = 0;
	char delim[2];
	delim[0] = a_delim;
	delim[1] = 0;

	/* Count how many elements will be extracted. */
	while (*tmp) {
		if (a_delim == *tmp) {
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

	if (result) {
		size_t idx = 0;
		char* token = strtok(a_str, delim);
		while (token) {
			assert(idx < count);
			*(result + idx++) = strdup(token);
			token = strtok(0, delim);
		}
		assert(idx == count - 1);
		*(result + idx) = 0;
	}
	return result;
}

void send_data(char* msg) {
	if (send(sock, msg, strlen(msg), 0) < 0) {
		printf("Failed to send to server: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
		close(sock);
	}
}

void check_quit(char* msg) {
	if (strncmp(msg, QUIT_MSG, strlen(QUIT_MSG)) == 0) {
		send_data("5");
		close(sock);
		exit(EXIT_FAILURE);
	}
	if (strncmp(msg, SQUIT_MSG, strlen(SQUIT_MSG)) == 0) {
		send_data("6");
		close(sock);
		exit(EXIT_FAILURE);
	}
}

int recv_data(char* msg) {
	int result;
	memset(msg, 0, strlen(msg));
	if ((result = recv(sock, msg, HOST_REPLY, 0)) < 0) {
		printf("Failed to receive from server: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
		close(sock);
	}
	return result;
}

int enter_test_auth(char* message, char* user, char* password, char** splitArgs,
		char* auth, char* server_reply, char* temp) {

	strcpy(user, "lironemilyg");
	strcpy(password, "305774382");
	int size = 2 + strlen(user) + strlen(password);
	size = inttostr(size, temp);
	sprintf(auth, "0%s%s	%s", temp, user, password);
	send_data(auth);
	recv_data(server_reply);
	if (strncmp(SUC_AUTH, server_reply, strlen(SUC_AUTH)) == 0) {
		puts(server_reply);
		free(splitArgs);
		return 1;
	}
	return 0;
}

int compose_to_server(char* message, char* temp, char** splitArgs,
		char* send_list, char* subject, char* text, int size, char* comp_txt,
		char* msgSize, char* server_reply) {
	int to_flag = 0, sub_flag = 0, text_flag = 0;
	while (!(to_flag && sub_flag && text_flag)) {
		fgets(message, TOTAL_MSG, stdin);
		strcpy(temp, message);
		check_quit(message);
		splitArgs = str_split(message, ' ');
		if (to_flag == 0) {
			if (strncmp(splitArgs[0], TO_ATT, strlen(TO_ATT)) == 0
					&& splitArgs[1]) {
				to_flag = 1;
				strcpy(send_list, temp);
			} else {
				printf("%s\n", TO_ERR);
			}
		} else if (to_flag == 1 && sub_flag == 0) {
			if (strncmp(splitArgs[0], SUB_ATT, strlen(SUB_ATT)) == 0
					&& splitArgs[1]) {
				sub_flag = 1;
				strcpy(subject, temp);
			} else {
				printf("%s\n", SUB_ERR);
			}
		} else if (to_flag == 1 && sub_flag == 1) {
			if (strncmp(splitArgs[0], TXT_ATT, strlen(TXT_ATT)) == 0
					&& splitArgs[1]) {
				text_flag = 1;
				strcpy(text, temp);
			} else {
				printf("%s\n", TXT_ERR);
			}
		}
	}
	size = strlen(send_list) + strlen(subject) + strlen(text);
	sprintf(comp_txt, "4%s%s%s%s", msgSize, send_list, subject, text);
	send_data(comp_txt);
	return 0;
}

int compose_to_server_test(char* message, char* temp, char** splitArgs,
		char* send_list, char* subject, char* text, int size, char* comp_txt,
		char* msgSize, char* server_reply) {
	int to_flag = 0, sub_flag = 0, text_flag = 0;
	strcpy(send_list, "To: lironemilyg,gizunterman,schneider2\n");
	strcpy(subject, "Subject: This is my subject\n");
	strcpy(text, "Text: This is my Content\n");
	size = strlen(send_list) + strlen(subject) + strlen(text);
	sprintf(comp_txt, "4%s%s%s%s", msgSize, send_list, subject, text);
	send_data(comp_txt);
	return 0;
}

int enter_auth(char* message, char* user, char* password, char** splitArgs,
		char* auth, char* server_reply, char* temp) {
	while (1) {
		fgets(message, TOTAL_MSG, stdin);
		check_quit(message);
		splitArgs = str_split(message, ' ');
		if (strncmp(splitArgs[0], USER_ATT, strlen(USER_ATT))
				!= 0|| !splitArgs[1] || strlen(splitArgs[1])>AUTH_LEN) {
			printf("%s\n", AUTH_USR_ERR);
		} else {
			strtok(splitArgs[1], "\n");
			strcpy(user, splitArgs[1]);
			break;
		}
	}
	while (1) {
		fgets(message, TOTAL_MSG, stdin);
		check_quit(message);
		splitArgs = str_split(message, ' ');
		if (strncmp(splitArgs[0], PASS_ATT, strlen(PASS_ATT))
				!= 0|| !splitArgs[1] ||strlen(splitArgs[1])>AUTH_LEN) {
			printf("%s\n", AUTH_PWD_ERR);
		} else {
			strcpy(password, splitArgs[1]);
			int size = 2 + strlen(user) + strlen(password);
			size = inttostr(size, temp);
			sprintf(auth, "0%s%s	%s", temp, user, password);
			send_data(auth);
			recv_data(server_reply);
			if (strncmp(SUC_AUTH, server_reply, strlen(SUC_AUTH)) == 0) {
				puts(server_reply);
				free(splitArgs);
				return 1;
			} else {
				printf("%s\n", BAD_AUTH);
				free(splitArgs);
				return 0;
			}
		}
	}
	free(splitArgs);
	return 0;
}

void get_cmd_and_execute(char *message, char *temp, char ** splitArgs,
		char* server_reply, char *send_list, char *subject, char *text,
		char* comp_txt, char* msgSize) {
	fgets(message, TOTAL_MSG, stdin);
	strcpy(temp, message);
	check_quit(message);
	splitArgs = str_split(message, ' ');
	int size;
	if (strncmp(splitArgs[0], SHOW, strlen(SHOW)) == 0) {
		send_data("1");
	} else if (strncmp(splitArgs[0], GET, strlen(GET)) == 0 && splitArgs[1]) {
		size = 1 + strlen(splitArgs[1]);
		size = inttostr(size, msgSize);
		sprintf(temp, "2%s%s", msgSize, splitArgs[1]);
		send_data(temp);
	} else if (strncmp(splitArgs[0], DEL, strlen(DEL)) == 0 && splitArgs[1]) {
		size = 1 + strlen(splitArgs[1]);
		size = inttostr(size, msgSize);
		sprintf(temp, "3%s%s", msgSize, splitArgs[1]);
		send_data(temp);
	} else if (strncmp(splitArgs[0], SHOW_ONLINE, strlen(SHOW_ONLINE)) == 0) {
		send_data("8");
	} else if (strncmp(splitArgs[0], COMP, strlen(COMP)) == 0) {
		compose_to_server(message, temp, splitArgs, send_list, subject, text,
				size, comp_txt, msgSize, server_reply);
	} else if (strncmp(splitArgs[0], MSG, strlen(MSG)) == 0) {
		sprintf(comp_txt, "7%s%s", msgSize, temp);
		send_data(comp_txt);
	} else {
		printf(WRNG_CMD);
	}
	free(splitArgs);
}

int inttostr(int i, char* str) {
	sprintf(str, "%04d", i);
	return 0;
}

int main(int argc, char* argv[]) {
	char msgSize[4];
	// var declaration
	char *hostname;
	char user[AUTH_LEN], password[AUTH_LEN], auth[3 * AUTH_LEN], send_list[21
			* AUTH_LEN], subject[SUBJECT], text[2010], comp_txt[TOT_TEXT];
	char message[TOTAL_MSG], server_reply[HOST_REPLY], temp[TOTAL_MSG];
	char** splitArgs;
	int port, max_sd;
	struct sockaddr_in server;
	fd_set readfds, writefds;

	// Checking argument state
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

	// Create socket
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1) {
		printf("Failed to create the listen socket: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	server.sin_addr.s_addr = inet_addr(DEFAULT_HOST);
	server.sin_family = AF_INET;
	server.sin_port = htons(port);

	//Connect to remote server
	if (connect(sock, (struct sockaddr *) &server, sizeof(server)) < 0) {
		printf("Failed to connect to server: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
		close(sock);
	}

	// authentication process can get out of the loop only by QUIT or good auth
	recv_data(server_reply);
	puts(server_reply);

	// entering user
	int flag = 0;
	while (flag == 0) {
		flag = enter_auth(message, user, password, splitArgs, auth,
				server_reply, msgSize);
	}
	while (1) {
		//clear the socket set
		FD_ZERO(&readfds);
		FD_ZERO(&writefds);
		//add master socket to set
		FD_SET(sock, &writefds);
		FD_SET(STDIN_FILENO, &writefds);
		max_sd = sock;
		//wait for an activity on one of the sockets , timeout is NULL , so wait indefinitely
		int activity = select(max_sd + 1, &writefds, NULL, NULL, NULL);

		if ((activity < 0) && (errno != EINTR)) {
			printf("select error");
		}

		if (FD_ISSET(STDIN_FILENO, &writefds)) {
			get_cmd_and_execute(message, temp, splitArgs, server_reply,
					send_list, subject, text, comp_txt, msgSize);
		}
		if (FD_ISSET(sock, &writefds)) {
			int bytesRecieved;
			// cmds to mail server
			bytesRecieved = recv_data(server_reply);
			server_reply[bytesRecieved] = '\0';
			printf("%s", server_reply);

		}
	}
	close(sock);
	return 0;
}

