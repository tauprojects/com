#include "UsersDB.h"
#include <assert.h>

struct user_struct {
	char user[MAXLEN];
	char pass[MAXLEN];
	MAIL mail[MAXMAILS];
	int mailsize;
};


struct users_db {
	USER usersList[20];
	int size;
};

struct user_mail{
	int id;
	DATA data;
	bool isTrash;
};

UsersDB UsersDBCreate(const char* filename){
	UsersDB users;
	users->size=0;
	if(filename==NULL){return NULL;}  //checking valid file name
	FILE* fp = fopen(filename, "r");
	if (fp == NULL) {return NULL;}    //checking file

	//temporary variables
	int lineNum=0;
	char username[MAXLEN];
	char password[MAXLEN];
	char tempLine[MAXLEN];
	char* temp;
	bool isValidLine=true;
	while (fgets(tempLine,MAXLEN, fp) != NULL) {
		if(isCommentLine(tempLine)){
			lineNum++;
			continue;
		}
		tempLine[strlen(tempLine)-1]='\0';
		temp = strchr(tempLine, '\t');
		if(temp==NULL){ isValidLine=false;}
		else{
			*temp='\0';
			strcpy(username,tempLine);
			strcpy(password,temp+1);
			isValidLine= removeSpaces(username) && removeSpaces(password);
		}
		if(!isValidLine){  //checking line
			return NULL;
		}
		users->usersList[users->size]->user=username;
		users->usersList[users->size]->pass=password;
		users->size++;
		lineNum++;
	}
	return users;
}

bool removeSpaces(char* source){
  char* i = source;
  char* j = source;
  while(*j != 0){
    *i = *j++;
    if(*i != ' ')
      i++;
  }
  *i = 0;
  return true;
}

bool isCommentLine(char* tempLine){
	while (isspace(*tempLine))
		tempLine++;
	if (*tempLine == '#' || *tempLine == '\0')
		return true;
	return false;
}

bool hasSpace(char* source){
	while (!isspace(*source) && *source != '\0')
		source++;
	if(*source == '\0')
		return false;
	return true;
}

bool isUserExists(UsersDB users,char* username){
	int size=users->size;
	if(username == NULL || users ==NULL){
		return false;
	}
	for(int i=0;i<size;i++){
		if(strcmp(users->usersList[i]->user,username)==0){
			return true;
		}
	}
	return false;
}

char* getUserPassword(UsersDB users,char* username){
	int size=users->size;
	if(username == NULL || users ==NULL){
		return NULL;
	}
	for(int i=0;i<size;i++){
		if(strcmp(users->usersList[i]->user,username)==0){
			return users->usersList[i]->pass;
		}
	}
	return NULL;
}



bool isNum(char* str) {
	while (isdigit(*str))
		str++;
	if (*str != '\0')
		return false;
	return true;
}

