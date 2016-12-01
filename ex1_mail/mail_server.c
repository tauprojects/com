/*
 * try.c
 *
 *  Created on: Nov 28, 2016
 *      Author: lirongazit
 */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>

#define DEFAULT_PORT 6423

#define MAXMAILS 32000
#define MAX_USERNAME 100
#define MAX_PASSWORD 100
#define MAX_SUBJECT 100
#define MAX_CONTENT 2000
#define NUM_OF_CLIENTS 20
#define MAX_TEMP 1024
#define TEMP_ID 6

#define HELLO_MSG "Welcome! I am simple-mail-server"
#define SUC_AUTH "Connected to server‬‬"
#define FAIL_AUTH "failed to connect to server"

//Enum Message Operation Code Declarations
typedef enum USER_COMMAND {
	AUTH, SHOW_INBOX, GET_MAIL, DELETE_MAIL, COMPOSE, QUIT, QUIT_SERVER
} USER_CMD;

typedef struct MAIL {
	int isTrash;
	int totalTo;
	char from[MAX_USERNAME];
	char subject[MAX_SUBJECT];
	char text[MAX_CONTENT];
	char to[NUM_OF_CLIENTS][MAX_USERNAME];
} MAIL;

typedef struct USER {
	char user[MAX_USERNAME];
	char pass[MAX_PASSWORD];
	int mailAmount;
	int mailIdInDB[MAXMAILS];
} USER;

int createUser(char* username, char* password, USER* user) {
	user->mailAmount = 0;
	strncpy(user->user, username, strlen(username));
	strncpy(user->pass, password, strlen(password));
	return 0;
}

char** str_split(char* a_str, const char a_delim, int* cnt) {
	char** result = 0;
	size_t count = 0;
	char* tmp = a_str;
	char* last_comma = 0;
	char delim[2];
	delim[0] = a_delim;
	delim[1] = 0;
	int i = 0;

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
		i++;
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
	*cnt = count - 1;
	return result;
}

void freeSplit(int cnt, char** splitArgs) {
	int j;
	for (j = 0; j < cnt; j++)
		free(splitArgs[j]);
	free(splitArgs);
}

bool removeSpaces(char* source) {
	char* i = source;
	char* j = source;
	while (*j != 0) {
		*i = *j++;
		if (*i != ' ')
			i++;
	}
	*i = 0;
	return true;
}

int UsersDBCreate(const char* filename, USER* users) {
	int size = 0, cnt;
	char tempLine[1024];
	char** splitArgs;
	if (filename == NULL) {
		return -1;
	}  //checking valid file name
	FILE* fp = fopen(filename, "r");
	if (fp == NULL) {
		return -1;
	}

	while (fgets(tempLine, 1024, fp) != NULL) {
		char* username = (char*) calloc(sizeof(char),MAX_USERNAME);
		char* password = (char*) calloc(sizeof(char),MAX_PASSWORD);
		splitArgs = str_split(tempLine, '\t', &cnt);
		if(splitArgs){
			strncpy(username, splitArgs[0], strlen(splitArgs[0]));
			strncpy(password, splitArgs[1], (strlen(splitArgs[1])-1));
		}
		createUser(username, password, &users[size]);
		size++;
		freeSplit(cnt, splitArgs);
		free(username);
		free(password);
	}
	fclose(fp);
	return size;
}

char* show_to(MAIL *mail) {
	int i;
	char* showTo = (char*) calloc(sizeof(char), MAX_USERNAME * NUM_OF_CLIENTS);
	if (!showTo) {
		return NULL;
	}
	for (i = 0; i < mail->totalTo; i++) {
		if (i == 0)
			strncpy(showTo, mail->to[i], strlen(mail->to[i]));
		else
			strncat(showTo, mail->to[i], strlen(mail->to[i]));
		if (i != mail->totalTo - 1)
			strncat(showTo, ",", 1);
	}
	return showTo;
}

char* build_mail(MAIL *mail) {
	int total = MAX_USERNAME * (NUM_OF_CLIENTS + 1) + MAX_SUBJECT + MAX_CONTENT
			+ 100;
	char* res = (char*) calloc(sizeof(char), total);
	if (!res) {
		return NULL;
	}
	char* to = show_to(mail);
	if (!to)
		return NULL;
	sprintf(res, "From: %s\nTo: %s\nSubject: %s\n%s\n", mail->from, to, mail->subject,
			mail->text);
	free(to);
	return res;
}

int split_mail(char* msg,char * username, MAIL *mail) {
	int cnt, total, i;
	char** splitArgs;
	char** to;
	splitArgs = str_split(msg, '\n', &cnt);
	if (!splitArgs)
		return -1;
	to = str_split(&splitArgs[0][4], ',', &total);
	if (!to)
		return -1;
	mail->isTrash = 0;
	mail->totalTo = total;
	strncpy(mail->from, username, strlen(username));
	strncpy(mail->subject, &splitArgs[1][9], strlen(&splitArgs[1][9]));
	strncpy(mail->text, splitArgs[2], strlen(splitArgs[2]));
	for (i = 0; i < total; i++)
		strncpy(mail->to[i], to[i], strlen(to[i]));
//	printf("after split_mail, the mail is:\nfrom: %s\nsubject: %s\n", mail->from, mail->subject);
	freeSplit(cnt, splitArgs);
	freeSplit(total, to);
	return 0;
}

int authenticate_user(USER* users, int num_of_users, char* username, char* password){
	password[strlen(password)]='\0';
	int i;
	for(i=0; i<num_of_users; i++){
//		if(strncmp(users[i].user, username, strlen(users[i].user))==0 ){
		if(strncmp(users[i].user, username, strlen(users[i].user))==0 && strlen(users[i].user) == strlen(username)){
			if(strncmp(users[i].pass, password, strlen(users[i].pass))==0 && strlen(users[i].pass) == strlen(password)){
//			if(strncmp(users[i].pass, password, strlen(users[i].pass))==0 ){
			return i;
			}
		}
	}
	return -1;
}

int parseId(char* message) {
	char* temp = strchr(message, '\t');
	int id = atoi(message);
	free(temp);
	return id;
}

int delete_mail(MAIL *mail){
	if (!mail){
		return 1;
	}
	mail->isTrash = 1;
	return 0;
}

char* get_mail(MAIL *mail){
	if(mail->isTrash == 0){
		return build_mail(mail);
	}
	return "mail deleted";
}

int update_users(USER* users, int num_users, MAIL *mail, int mail_id){
	int i, j;
	for(i = 0; i < mail->totalTo; i++){
		for(j = 0; j < num_users; j++){
//			printf("to: %s    mail: %s\n",users[j].user, mail->to[i]);
			if(strncmp(mail->to[i], users[j].user, strlen(mail->to[i]))== 0 && strlen(mail->to[i]) == strlen(users[j].user)){
				users[j].mailIdInDB[users[j].mailAmount] = mail_id;
				users[j].mailAmount++;
//				printf("user: %s\nmail id:%d\n",users[j].user,users[j].mailAmount);
			}
		}
	}
	return 0;
}

int compose_mail(char* msg,char* username, MAIL *mail, USER* users, int NumberOfUsers, int* mailsInServer){
//	printf("in compose, this is the msg that was received from: %s\n%s\n",username,msg);
	int indication;
	if(!msg){
		return -1;
	}
	if(!mail){
		return -2;
	}
	indication = split_mail(msg,username, mail);
	update_users(users, NumberOfUsers, mail, *mailsInServer);
	(*mailsInServer)++;
	return indication;
}

void print_mail(MAIL mail){
	printf("isTrash:%d\ntotalTo:%d\nfrom:%s\nsubject:%s\ntext:%s\n",mail.isTrash,mail.totalTo,mail.from, mail.subject, mail.text);
}

void show_mailDB(MAIL* mails, int mailsInServer){
	int i;
	for(i=0; i<mailsInServer; i++){
		print_mail(mails[i]);
	}
}

char* show_inbox(USER user, MAIL *mails){
	int i;
	int total = MAX_USERNAME * (NUM_OF_CLIENTS + 1) + MAX_SUBJECT + MAX_CONTENT
				+ 100;
	char* result = (char*)calloc(sizeof(char), total*user.mailAmount);
	if (!result){
		return NULL;
	}
	for(i = 0; i< user.mailAmount; i++){
		if(!mails[user.mailIdInDB[i]].isTrash){
			char mailInbox[total];
//			printf("testing attributes: from: %s\nsubject: %s\n", mails[user.mailIdInDB[i]].from,mails[user.mailIdInDB[i]].subject);
			sprintf(mailInbox,"%d %s %s\n",i,mails[user.mailIdInDB[i]].from,mails[user.mailIdInDB[i]].subject);
//			printf("\ntext to be concat %s\n", mailInbox);
			strncat(result,mailInbox,strlen(mailInbox));
//			printf("text from show_inbox phase: %d\n%s\n", i, result);
		}
	}
	return result;
}

int getOpcode(char* client_message) {
	char* temp = (char*) malloc(sizeof(char) * 5);
	strncpy(temp, client_message, 1);
	int opCode = atoi(temp);
	free(temp);
	return opCode;
}


int listenSd; // The listen socket, defined as global for the code that exit with failure


 int main(int argc, char* argv[]) {
	int mailsInServer = 0, NumberOfUsers;
	int ack, i, port, cnt = 0,opCode,user_id,id_in_db, id;
	char *build;
	char* filename;
	char* inboxUser;
	char client_message[4500];
	char curr_username[MAX_USERNAME];
	char** splitArgs;
	if (argc == 2) {
		filename = argv[1];
		port = DEFAULT_PORT;
	} else if (argc == 3) {
		filename = argv[1];
		port = atoi(argv[2]);
	} else {
		puts("invalid number of arguments");
		return -1; //exit(1)
	}

	MAIL *totalMails = (MAIL*) malloc(sizeof(MAIL) * MAXMAILS); //mail DB - here all the mails are going to be.
	USER *users = (USER*) malloc(sizeof(USER) * NUM_OF_CLIENTS);

	if( access(filename, F_OK) != -1 ) {
		NumberOfUsers = UsersDBCreate(filename, users);
	} else {
		printf("Error While Accessing Users Data File: %s\n",filename);
		exit(EXIT_FAILURE);
	}

	if (NumberOfUsers < 0) {
		printf("Error While Parsing %s File", filename);
		exit(EXIT_FAILURE);
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

	if (listen(listenSd, NUM_OF_CLIENTS) == -1) {   // Listen to the socket
		printf("Failed to listen to the listen socket: %s\n", strerror(errno));
		close(listenSd);
		exit(EXIT_FAILURE);
	}
	// loops as long as there are users appending
	int thereAreUsers = 1;
	while (thereAreUsers) {
		int connSd = accept(listenSd, (struct sockaddr*) &their_addr, (socklen_t*) &sin_size);  // Accept a connection
		if (connSd == -1) {
			printf("Failed to accept a connection: %s\n", strerror(errno));
			close(listenSd);
			exit(EXIT_FAILURE);
		}
		write(connSd, HELLO_MSG, strlen(HELLO_MSG));

		// autentication phase
		int authFlag = 1;
		while (authFlag) {
			memset(client_message, 0, 4500);
			printf("Ready For USER\n");
			if (recv(connSd, client_message, 4500, 0) < 0) {
				printf("Failed to receive from server: %s\n", strerror(errno));
				exit(EXIT_FAILURE);
			} else {
				splitArgs = str_split(&client_message[5],'\t', &cnt);
				if(splitArgs){
					strncpy(curr_username, splitArgs[0], strlen(splitArgs[0]));
					user_id = authenticate_user(users, NumberOfUsers,curr_username, splitArgs[1]);
					if (user_id >= 0) {
						write(connSd, SUC_AUTH, strlen(SUC_AUTH));
						authFlag = 0;
						freeSplit(cnt,splitArgs);
						break;
					} else {
						freeSplit(cnt,splitArgs);
						write(connSd, FAIL_AUTH, strlen(FAIL_AUTH));
					}
				}
			}
		}
		// waiting for user cmds
		int quitFlag = 1;
		while (quitFlag) {
			memset(client_message, 0, 4500);
			if (recv(connSd, client_message, 4500, 0) < 0) {
				printf("Failed to receive from server: %s\n", strerror(errno));
				exit(EXIT_FAILURE);
			} else {
				opCode = getOpcode(client_message);
				switch (opCode) {
				case SHOW_INBOX:
//					show_mailDB(totalMails, mailsInServer);
					inboxUser=show_inbox(users[user_id],totalMails);
					write(connSd, inboxUser, strlen(inboxUser));
					free(inboxUser);
					break;
				case GET_MAIL:
					id = parseId(&client_message[5]);
//					printf("id: %d   user id: %d\n",id,users[user_id].mailAmount);
					if(id < users[user_id].mailAmount){
						id_in_db = users[user_id].mailIdInDB[id];
						if(id_in_db < mailsInServer){
							build = get_mail(&totalMails[id_in_db]);
							write(connSd, build, strlen(build));
//							if(build){
//								free(build);
//							}
						}
						else{
							write(connSd, "mail does not exist", strlen("mail does not exist"));
						}
					}
					else{
						write(connSd, "mail does not exist", strlen("mail does not exist"));
					}
					break;
				case DELETE_MAIL:
					id = parseId(&client_message[5]);
					id_in_db = users[user_id].mailIdInDB[id];
					delete_mail(&totalMails[id_in_db]);
					break;
				case COMPOSE:
					ack = compose_mail(&client_message[5],curr_username, &totalMails[mailsInServer], users, NumberOfUsers, &mailsInServer);
					printf("ack: %d   mailinServer: %d\n",ack, mailsInServer);
					break;
				case QUIT:
					quitFlag = 0;
					break;
				case QUIT_SERVER:
					close(connSd);
					close(listenSd);
					if (build) {
						free(build);
					}
					for (i = 0; i < mailsInServer; i++) {
						free(&totalMails[i]);
					}
					free(totalMails);
//					for (i = 0; i < NumberOfUsers; i++) {
//						free(&users[i]);
//					}
					free(users);
					if (inboxUser) {
						free(inboxUser);
					}
					return 0;
				}
			}
		}
		close(connSd);
	}
//	// checking build and split mail
//	char msg[] =
//			"From: li\nTo: lironemilyg,gizunterman\nSubject: hello\nText: hi whatsuppppp???";
//	char msg2[] =
//			"From: big\nTo: lironemilyg,gizunterman,schneider2\nSubject: bye bye\nText: it is late now";
////	ack = split_mail(msg, &totalMails[0]);
////	build = build_mail(&totalMails[0]);
////	printf("%s", build);
////	ack = split_mail(msg2, &totalMails[1]);
////	build = build_mail(&totalMails[1]);
////	printf("%s", build);
////	build = get_mail(&totalMails[0]);
////	printf("mail: \n%s", build);
////	delete_mail(&totalMails[0]);
////	build = get_mail(&totalMails[0]);
////	printf("mail: \n%s", build);
//	ack = compose_mail(msg, &totalMails[0], users, NumberOfUsers, &mailsInServer);
////	printf("%d\n",ack);
//	build = get_mail(&totalMails[0]);
////	printf("mail: \n%s", build);
//	ack = compose_mail(msg2, &totalMails[1], users, NumberOfUsers, &mailsInServer);
//	for(i=0;i<NumberOfUsers;i++){
////		printf("user: %s total mails: %d, mail[0]: %d\n",users[i].user,users[i].mailAmount,users[i].mailIdInDB[0]);
//		inboxUser=show_inbox(users[i],totalMails);
//		printf("user: %s showInbox: %s\n",users[i].user,inboxUser);
//		memset(inboxUser, 0, strlen(inboxUser));
//	}
//
////	  checking authentication
////	char username1[] = "schneider2";
////	char password1[] = "302814355";
////	char username2[] = "schneider2456";
////	char username3[] = "gizunterman";
////	char password2[] = "303157804";
////	success = authenticate_user(users, NumberOfUsers, username1, password1);
////	printf("auth success = %d\n", success);
////	success = authenticate_user(users, NumberOfUsers, username2, password1);
////	printf("auth not success = %d\n", success);
////	success = authenticate_user(users, NumberOfUsers, username1, password2);
////	printf("auth not success = %d\n", success);
////	success = authenticate_user(users, NumberOfUsers, username3, password2);
////	printf("auth success = %d\n", success);
//
//

	// free everything
	if(build){
		free(build);
	}

	for (i = 0; i < mailsInServer; i++) {
		free(&totalMails[i]);
	}
	free(totalMails);
	for (i = 0; i < NumberOfUsers; i++) {
		free(&users[i]);
	}
	free(users);
	if(inboxUser){
		free(inboxUser);
	}
	close(listenSd);
	return 0;
}

