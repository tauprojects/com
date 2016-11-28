#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "mail_server.h"
#include "UsersDB.h"

int showInbox(USER user, char* totalInbox) {
	int userSizeMail = user->mailsize;
	int showMailSize = MAX_SUBJECT + MAX_USERNAME + TEMP_ID;
	int totalResSize = userSizeMail * showMailSize;
	int flagFirst = 1;
	//char* totalInbox = (char*)malloc(sizeof(char)*totalResSize);
	char* mail = (char*) malloc(sizeof(char) * showMailSize);
	for (int i = 0; i < userSizeMail; i++) {
		showMail(user->mail[i], i, mail);
		if (mail == NULL) {
			continue;
		}
		if (flagFirst == 1) {
			strcpy(totalInbox, mail);
			flagFirst = 0;
		} else {
			strcat(totalInbox, mail);
		}
	}
	free(mail);
	if (flagFirst == 1) {
		return -1;
	}
	return 0;
}

int showMail(MAIL mail, int id, char *resMail) {
	int showMailSize = MAX_SUBJECT + MAX_USERNAME + TEMP_ID;
	if (mail->isTrash) {
		return -1;
	}
//	char* resMail = (char*)malloc(sizeof(char)*showMailSize);
	sprintf(resMail, "%d", id);
//  strcpy(resMail, id);
	strcat(resMail, " ");
	strcat(resMail, mail->from);
	strcat(resMail, " ");
	strcat(resMail, mail->subject);
	strcat(resMail, "\n");
	return 0;
}

char* show_to(MAIL mail) {
//	char* showTo = (char*) malloc(sizeof(char) * TOTAL_TO * MAX_USERNAME);
//	for (int i = 0; i < mail->totalTo; i++) {
//		if (i == 0)
//			strcpy(showTo, mail->to[i]);
//		else
//			strcat(showTo, mail->to[i]);
//		if (i != mail->totalTo - 1)
//			strcat(showTo, ",");
//	}
	return mail->to;
}

int getMail(USER user, int id, char* stringMail) {
	int totalMailSize = MAX_SUBJECT + MAX_USERNAME + MAX_CONTENT
			+ TOTAL_TO * MAX_USERNAME;
	if (user->mailsize <= id) {
		return -1;
	}
	MAIL mail = user->mail[id];
	if (mail->isTrash) {
		return -2;
	}
	if (stringMail == NULL) {
		return -3;
	}
//	char* stringMail = (char*)malloc(sizeof(totalMailSize));
	strcpy(stringMail, "From:");
	strcat(stringMail, mail->from);
	strcat(stringMail, "\nTo: ");
	strcat(stringMail, show_to(mail));
	strcat(stringMail, "\nSubject: ");
	strcat(stringMail, mail->subject);
	strcat(stringMail, "\nText: ");
	strcat(stringMail, mail->content);
	strcat(stringMail, "\n");
	return 0;
}

int deleteMail(USER user, int id) {
	if (user->mailsize <= id) {
		return -1;
	}
	MAIL mail = user->mail[id];
	mail->isTrash = true;
	return 0;
}


int compose(TOTAL_MAILS mailsDB, USER* users, int size, MAIL mail) {
	int cnt;
	if (mailsDB->size >= MAXMAILS) {
		return -1;
	}
	if (mail->totalTo > 20) {
		return -2;
	}

	char** To = str_split(mail->to,',',&cnt);
	int newMailServerID = mailsDB->size;
	mailsDB->mails[newMailServerID] = mail;
	mailsDB->size++;

	for (int i = 0; i < mail->totalTo; i++) {
		for (int i = 0; i < size; i++) {
//			if (strcmp(users[i]->user, mailsDB->mails[newMailServerID]->to[i])
			if (strcmp(users[i]->user, To[i])
					== 0) {
				users[i]->mail[users[i]->mailsize] =
						mailsDB->mails[newMailServerID];
				users[i]->mailsize++;
			}
		}
	}
	return 0;
}
void printMail(MAIL mail) {
	printf("From: %s\n", mail->from);
	printf("To: %s\n", mail->to);
	printf("Subject: %s\n", mail->subject);
	printf("Text: %s\n", mail->content);
	printf("Total To: %d\n", mail->totalTo);
	printf("Is Trash: %d\n", mail->isTrash);
}
int parseMail(char* unparseMail, char* from, MAIL mail) {
	int maxTotalToSize = TOTAL_TO * MAX_USERNAME;

	if (unparseMail == NULL) {
		return -1;
	}
	char* part1 = (char*) malloc(sizeof(char) * MAX_TEMP);
	char* part2 = (char*) malloc(sizeof(char) * MAX_TEMP);
	char* part3 = (char*) malloc(sizeof(char) * MAX_TEMP);
	char* Subject = (char*) calloc(sizeof(char), MAX_SUBJECT);
	char* Content = (char*) calloc(sizeof(char), MAX_CONTENT);
	char* toAll = (char*) calloc(sizeof(char), maxTotalToSize);
	char* toAlltemp = (char*) calloc(sizeof(char), maxTotalToSize);
	if (!part1 || !part2 || !part3 || !Subject || !Content || !toAll || !toAlltemp){
		return -1;
	}
	char* temp;
	int cnt;

//Parse Mail to 3 parts
	temp = strtok(unparseMail, "\n");
	strcat(part1, temp);

	temp = strtok(NULL, "\n");
	strcat(part2, temp);

	temp = strtok(NULL, "\n");
	strcat(part3, temp);

	//Parse part 1

	strcat(toAll, &part1[4]);
	strcat(toAlltemp, &part1[4]);

	char** To = str_split(toAlltemp,',',&cnt);
//	char** To = str_split(toAll, ',');
	for(int j=0;j<cnt;j++)
		free(To[j]);
	free(To);
	free(toAlltemp);
	//Parse part 2
	strcpy(Subject, &part2[9]);
	printf("\nsub: %s", Subject);

	//Parse part 3
	strcat(Content, &part3[6]);

//	printf("\nsplitted: %s", To[0]);

	mail->isTrash = false;
	mail->totalTo=cnt;
	mail->from = from;
	mail->subject = Subject;
	mail->content = Content;
	mail->to = toAll;
	printf("\nmailsub: %s", mail->subject);


	free(part1);
	free(part2);
	free(part3);
	return 0;
}
bool auth_user(char* client_message, char* username, USER* users, int size) {
	int cnt;
	if (client_message == NULL) {
		return -1;
	}
	printf("MSG in auth: %s\n", client_message);
	char** temp = str_split(client_message, ' ', &cnt);
	strncpy(username, temp[0], strlen(temp[0]));
	char* password = (char*) malloc(sizeof(char) * MAX_PASSWORD);
	strncpy(password, temp[1], strlen(temp[1]));
	char* expected_pass = getUserPassword(users, size, username);
	if(expected_pass == NULL){
		return false;
	}
	removeSpaces(password);
	printf("user %s pass %s  exp pass %s \n", username,password,expected_pass);
	password[strlen(password) - 1] = '\0';

	if (strcmp(password, expected_pass) == 0) {
		fflush(NULL);
		puts("im true");
		return true;
	}
	return false;
}

