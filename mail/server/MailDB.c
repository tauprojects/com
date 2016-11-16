/*
 * MailDB.c
 *
 *  Created on: Nov 12, 2016
 *      Author: lirongazit
 */


#include "MailDB.h"
#include <stdlib.h>ß


struct mail_data{
	char from[100];  //check name size
	char to[20][100]; //check name size
	int totalTo;
	char subject[100];
	char content[2000];
};


struct total_mails{
	DATA mails[MAXMAILS];
	int size;
};

bool isMailDBFull(MAILS mails){
	if(mails->size >= MAXMAILS){
		return true;
	}
	return false;
}

UsersDB UsersDBCreate(const char* filename){
	UsersDB users;
	users->size=0;
	if(filename==NULL){return NULL;}  //checking valid file name
	FILE* fp = fopen(filename, "r");
	if (fp == NULL) {return NULL;}    //checking file

	//temporary variables

	return users;
}


int insertMail(char* unparseMail, MAILS mails,char* from){
	if(unparseMail==NULL || mails==NULL){
		return -1;
	}
	char* tempLine;
	char* temp;
	char* key;
	temp = strchr(key, '\n');
	*temp='\0';
	tempLine = strchr(key, ':');
	*tempLine='\0';
	if(strcmp(key,"To") == 0){

		strcpy(mails->mails[mails->size]->to,tempLine);
	}
	else
		return -1;
	temp = strchr(key, '\n');
	*temp='\0';
	tempLine = strchr(key, ':');
	*tempLine='\0';
	if(strcmp(key,"Subject") == 0){
		strcpy(mails->mails[mails->size]->subject,tempLine);
	}
	else
		return -1;
	temp = strchr(key, '\n');
	*temp='\0';
	tempLine = strchr(key, ':');
	*tempLine='\0';
	if(strcmp(key,"Text") == 0){
		strcpy(mails->mails[mails->size]->content,tempLine);
	}
	else
		return -1;
	strcpy(mails->mails[mails->size]->from,from);
	return 0;
}

