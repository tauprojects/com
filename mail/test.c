#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#define DEFAULT_PORT 6423
#define SHOW_MAIL_SIZE 300
#define TOTAL_MAIL_SIZE 4500
#define TOTAL_TO 20
#define MAXMAILS 1000
typedef enum USER_COMMAND {
	AUTH,
	SHOW_INBOX,
	GET_MAIL,
	DELETE_MAIL,
	QUIT,
	COMPOSE
} USER_CMD;

struct mail_data{
	char* from;  //check name size
	char** to; //check name size
	int totalTo;
	char* subject;
	char* content;
	bool isTrash;
};
typedef struct mail_data* MAIL;

struct total_mails{
	MAIL mails[MAXMAILS];
	int size;
};
typedef struct total_mails* TOTAL_MAILS;
int parseMail(char* unparseMail,char* from, MAIL mail);
int decInputState(char* client_message);
void removeSpaces(char* source);

char** str_split(char* a_str, const char a_delim,int* countStr);
bool auth_user(char* client_message,  char* username);

void printMail(MAIL mail);
int main(){
	char* unpare  = (char*) malloc(sizeof(char) * 1024);
	char* username = (char*) malloc(sizeof(char) * 100);
//	char username[100];

//	strcat(unpare,"4 To: Mati,Yuval,Lior \nSubject: Gizunterman \nContent: This is my content");
	strcat(unpare,"0User: Mati    Password: Gizunterman    ");
	bool state = auth_user(&unpare[1],username);
	printf("\nState: %d\n",state);
	printf("\nUsername: %s\n",username);
	printf("\nUnpare: %s\n",unpare);

//	DATA mail = (DATA)malloc(sizeof(mail));
//	char* unpare  = (char*) malloc(sizeof(char) * 1024);
//	int m = parseMail(unpare,"Liron",mail);

}
bool auth_user(char* client_message,  char* username){
	char* expected_pass = "Gizunterman";
	if (client_message == NULL) {
		return -1;
	}
	char* part1 = (char*) malloc(sizeof(char) * 1028);
	char* part2 = (char*) malloc(sizeof(char) * 1028);
	char* temp;
	temp = strtok(client_message, "\t");
	strcat(part1,temp);

	temp = strtok(NULL, "\n");
	strcat(part2,temp);

	printf("\nPart1:%s\n",part1);
	printf("\nPart2: %s\n",part2);

	//Parse part 1
	strcat(username, &part1[5]);

	//Parse part 2
	char* password = (char*) malloc(sizeof(char) * 1028);
	strcat(password, &part2[10]);

	//Handle Passowrd
	removeSpaces(password);
	password[strlen(password)-1]='\0';
	free(part1);
	free(part2);
	if(strcmp(password,expected_pass)==0){
		return true;
	}
	return false;
}

int decInputState(char* client_message) {
	char* temp = (char*) malloc(sizeof(char) * 5);
	strncpy(temp, client_message, 1);
	int state = atoi(temp);
	printf("\nstate is: %d", state);

	switch (state) {
	case 0:
		printf("Authenticate!\n");
//		auth(client_message); //T0DO
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
		parseMail(unpare,"Liron",mail); //T0DO
		printMail(mail);
		break;
	case 5:
		printf("quit\n");
		break;
	default:
		printf("quit\n");
	}
//	free(temp);
	return state;
}

//bool auth(char* client_message){
//	char* user;
//	char* pass;
//	getPassword(user);
//	if(strcmp(expected,acctual)==0)
//		return true;
//	return false;
//}

int parseMail(char* unparseMail ,char* from, MAIL mail){
	if (unparseMail == NULL) {
		return -1;
	}
	char* part1 = (char*) malloc(sizeof(char) * 1028);
	char* part2 = (char*) malloc(sizeof(char) * 1028);
	char* part3 = (char*) malloc(sizeof(char) * 1028);
	char* temp;
	int cnt;

	//Parse Mail to 3 parts
	temp = strtok(unparseMail, "\n");
	strcat(part1,temp);

	temp = strtok(NULL, "\n");
	strcat(part2,temp);

	temp = strtok(NULL, "\n");
	strcat(part3,temp);

	//Parse part 1
	char* toAll= (char*) malloc(sizeof(char) * 1028);
	strcat(toAll, &part1[3]);
	char** To = str_split(toAll,',',&cnt);

	//Parse part 2
	char* Subject = (char*) malloc(sizeof(char) * 1028);
	strcat(Subject, &part2[8]);

	//Parse part 3
	char* Content = (char*) malloc(sizeof(char) * 1028);
	strcat(Content, &part3[8]);

	mail->from=from;
	mail->content=Content;
	mail->subject = Subject;
	mail->to=To;
	mail->totalTo=cnt;
	mail->isTrash=false;

	free(part1);
	free(part2);
	free(part3);
	return 0;
}

void printMail(MAIL mail){
	printf("From: %s\n",mail->from);
	printf("To: %s\n",mail->to[1]);
	printf("subject: %s\n",mail->subject);
	printf("Content: %s\n",mail->content);
	printf("Total To: %d\n",mail->totalTo);
	printf("Is Trash: %d\n",mail->isTrash);
}
char** str_split(char* a_str, const char a_delim,int* countStr) {
	char** result = 0;
	size_t count = 0;
	char* tmp = a_str;
	char* last_comma = 0;
	char delim[2];
	delim[0] = a_delim;
	delim[1] = 0;
	*countStr=1;

	/* Count how many elements will be extracted. */
	while (*tmp) {
		if (a_delim == *tmp) {
			count++;
			countStr++;
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

void removeSpaces(char* source){
  char* i = source;
  char* j = source;
  while(*j != 0){
    *i = *j++;
    if(*i != ' ')
      i++;
  }
  *i = 0;
}
