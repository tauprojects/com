#include "UsersDB.h"
#include <assert.h>

//Fix for cahr* instead char[]
UsersDB UsersDBCreate(const char* filename){
	UsersDB users;
	users->size=0;
	if(filename==NULL){return NULL;}  //checking valid file name
	FILE* fp = fopen(filename, "r");
	if (fp == NULL) {return NULL;}    //checking file

	//temporary variables
	int lineNum=0;
	char* username = (char*)malloc(sizeof(char)*MAX_USERNAME);
	char* password =  (char*)malloc(sizeof(char)*MAX_PASSWORD);
	char tempLine[MAX_TEMP];
	char* temp;
	bool isValidLine=true;
	while (fgets(tempLine,MAX_TEMP, fp) != NULL) {
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
			fclose(fp);
			return NULL;
		}
		users->usersList[users->size]->user=username;
		users->usersList[users->size]->pass=password;
		users->size++;
		lineNum++;
	}
	fclose(fp);
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

USER getUser(UsersDB users,char* username){
	int size=users->size;
	if(username == NULL || users ==NULL){
		return NULL;
	}
	for(int i=0;i<size;i++){
		if(strcmp(users->usersList[i]->user,username)==0){
			return users->usersList[i];
		}
	}
	return NULL;
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

void prinUsersDb(UsersDB users){
	int size = users->size;
	printf("Total Users is: %d\n",size);
	for(int i=0;i<size;i++){
		printf("Username: %s, Password: %s" ,users->usersList[i]->user ,users->usersList[i]->pass);
	}
}
