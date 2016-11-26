#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "mail_server.h"
#include "UsersDB.h"


int showInbox(USER user,char* totalInbox) {
	int userSizeMail = user->mailsize;
	int showMailSize = MAX_SUBJECT + MAX_USERNAME + TEMP_ID;
	int totalResSize = userSizeMail * showMailSize;
	int flagFirst = 1;
	//char* totalInbox = (char*)malloc(sizeof(char)*totalResSize);
	char* mail = (char*)malloc(sizeof(char)*showMailSize);
	for (int i = 0; i < userSizeMail; i++) {
		mail = show_mail(user->mail[i]);
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

char* showMail(MAIL mail, int id) {
	int showMailSize = MAX_SUBJECT + MAX_USERNAME + TEMP_ID;
	if (mail->isTrash) {
		return NULL;
	}
	char* resMail = (char*)malloc(sizeof(char)*showMailSize);
//	sprintf(resMail, "%d", id);
	strcpy(resMail, id);
	strcat(resMail, " ");
	strcat(resMail, mail->from);
	strcat(resMail, " ");
	strcat(resMail, mail->subject);
	strcat(resMail, "\n");
	return resMail;
}

char* show_to(MAIL mail) {
	char* showTo = (char*)malloc(sizeof(char)*TOTAL_TO*100);
	for (int i = 0; i < mail->totalTo; i++) {
		if (i == 0)
			strcpy(showTo, mail->to[i]);
		else
			strcat(showTo, mail->to[i]);
		if (i != mail->totalTo - 1)
			strcat(showTo, ",");
	}
	return showTo;
}

int getMail(USER user, int id,char* stringMail) {
	int totalMailSize =MAX_SUBJECT + MAX_USERNAME + MAX_CONTENT + TOTAL_TO*MAX_USERNAME;
	if (user->mailsize <= id) {
		return -1;
	}
	MAIL mail = user->mail[id];
	if (mail->isTrash) {
		return -2;
	}
	if(stringMail == NULL){
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

char** str_split(char* a_str, const char a_delim,int* countStr) {
	char** result = 0;
	size_t count = 0;
	char* tmp = a_str;
	char* last_comma = 0;
	char delim[2];
	delim[0] = a_delim;
	delim[1] = 0;
	countStr=1;

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

int compose(TOTAL_MAILS mailsDB,USER* users,int size, MAIL mail) {
	if (mailsDB->size >= MAXMAILS) {
		return -1;
	}
	if(mail->totalTo > 20 ){
			return -2;
		}
	int newMailServerID = mailsDB->size;
	mailsDB->mails[newMailServerID] = mail;
	mailsDB->size++;

	for(int i=0;i<mail->totalTo;i++){
		for(int i=0;i<size;i++){
			if(strcmp(users[i]->user,mailsDB->mails[newMailServerID]->to[i])==0){
				users[i]->mail[users[i]->mailsize] = mailsDB->mails[newMailServerID];
				users[i]->mailsize++;
			}
		}
	}
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
int parseMail(char* unparseMail ,char* from, MAIL mail){
	int maxTotalToSize =TOTAL_TO*MAX_USERNAME;

	if (unparseMail == NULL) {
		return -1;
	}
	char* part1 = (char*) malloc(sizeof(char) * MAX_TEMP);
	char* part2 = (char*) malloc(sizeof(char) * MAX_TEMP);
	char* part3 = (char*) malloc(sizeof(char) * MAX_TEMP);
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
	char* toAll= (char*) malloc(sizeof(char) * maxTotalToSize);
	strcat(toAll, &part1[3]);
	char** To = str_split(toAll,',',&cnt);

	//Parse part 2
	char* Subject = (char*) malloc(sizeof(char) * MAX_SUBJECT);
	strcat(Subject, &part2[8]);

	//Parse part 3
	char* Content = (char*) malloc(sizeof(char) * MAX_CONTENT);
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
bool auth_user(char* client_message, char* username){
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
	char* password = (char*) malloc(sizeof(char) * MAX_PASSWORD);
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

