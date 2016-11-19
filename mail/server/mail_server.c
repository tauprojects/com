#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mail_server.h"

#define DEFAULT_PORT 6423
#define SHOW_MAIL_SIZE 300
#define TOTAL_MAIL_SIZE 4500
#define TOTAL_TO 20

char* show_inbox(USER user) {
	int userSizeMail = user->mailsize;
	int totalResSize = userSizeMail * SHOW_MAIL_SIZE;
	int flagFirst = 1;
	char totalInbox[totalResSize];
	char mail[SHOW_MAIL_SIZE];
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
	if (flagFirst == 1) {
		return NULL;
	}
	return totalInbox;
}

char* show_mail(MAIL mail) {
	if (mail->data->isTrash) {
		return NULL;
	}
	char resMail[SHOW_MAIL_SIZE];
	strcpy(resMail, mail->id);
	strcat(resMail, " ");
	strcat(resMail, mail->data->from);
	strcat(resMail, " ");
	strcat(resMail, mail->data->subject);
	strcat(resMail, "\n");
	return resMail;
}

char* show_to(MAIL mail) {
	char showTo[TOTAL_TO * 100];
	for (int i = 0; i < mail->data->totalTo; i++) {
		if (i == 0)
			strcpy(showTo, mail->data->to[i]);
		else
			strcat(showTo, mail->data->to[i]);
		if (i != mail->data->totalTo - 1)
			strcat(showTo, ",");
	}
	return showTo;
}

char* get_mail(USER user, int id) {
	if (user->mailsize <= id) {
		return NULL;
	}
	MAIL mail = user->mail[id];
	if (mail->data->isTrash) {
		return NULL;
	}
	char stringMail[TOTAL_MAIL_SIZE];
	strcpy(stringMail, "From:");
	strcat(stringMail, mail->data->from);
	strcat(stringMail, "\nTo: ");
	strcat(stringMail, show_to(mail));
	strcat(stringMail, "\nSubject: ");
	strcat(stringMail, mail->data->subject);
	strcat(stringMail, "\nText: ");
	strcat(stringMail, mail->data->content);
	strcat(stringMail, "\n");
	return stringMail;
}

int delete_mail(USER user, int id) {
	if (user->mailsize <= id) {
		return -1;
	}
	MAIL mail = user->mail[id];
	mail->data->isTrash = true;
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

int compose(MAILS mailsDB,UsersDB users, char* from, char* to, char* subject, char* content) {
	if (mailsDB->size >= MAXMAILS) {
		return -1;
	}
	int newMailServerID = mailsDB->size;
	int countStr;
	mailsDB->size++;
	strcpy(mailsDB->mails[newMailServerID]->from, from);
	strcpy(mailsDB->mails[newMailServerID]->subject, subject);
	strcpy(mailsDB->mails[newMailServerID]->content, content);
	mailsDB->mails[newMailServerID]->isTrash = false;
	char** mailTo = str_split(to, ",", &countStr);
	if(countStr > 20 ){
		return -2;
	}
	mailsDB->mails[newMailServerID]->totalTo=countStr;
	for(int i=0;i<countStr;i++){
		strcpy(mailsDB->mails[newMailServerID]->to[i],mailTo[i]);
		free(mailTo[i]);
		int size=users->size;
		for(int i=0;i<size;i++){
			if(strcmp(users->usersList[i]->user,mailsDB->mails[newMailServerID]->to[i])==0){
				users->usersList[i]->mail[users->usersList[i]->mailsize]=mailsDB->mails[newMailServerID];
				users->usersList[i]->mailsize++;
			}
		}
	}
	return 0;
}

