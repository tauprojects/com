#include "UsersDB.h"
#include <assert.h>

//Fix for cahr* instead char[]
int UsersDBCreate(const char* filename, USER* users){
	int size =0;
	if(filename==NULL){return -1;}  //checking valid file name
	FILE* fp = fopen(filename, "r");
	if (fp == NULL) {return -1;}    //checking file
	//temporary variables
	int lineNum=0;
	char tempLine[1024];
	char* temp;
	bool isValidLine=true;
	USER ustemp;
	char* username = (char*)malloc(sizeof(char)*MAX_USERNAME);
	char* password =  (char*)malloc(sizeof(char)*MAX_PASSWORD);
	while (fgets(tempLine,1024, fp) != NULL ) {
		if(tempLine[strlen(tempLine)-1]=='\n'){
			tempLine[strlen(tempLine)-1]='\0';
		}
		else{
			tempLine[strlen(tempLine)]='\0';
		}
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
			return -1;
		}
		users[size]=createUser(username,password);
		printf("\nUsername: %s",users[size]->user);
		printf("\nPassword: %s",users[size]->pass);
		size++;
		lineNum++;
	}
	printf("\nLines number: %d ",size);
	free(username);
	free(password);
	fclose(fp);
	return size;
}

USER createUser(char* user,char* pass){
	USER u=(USER)malloc(sizeof(USER));
	u->mailsize=0;
	u->user=(char*)malloc(sizeof(char)*MAX_USERNAME);
	u->pass =  (char*)malloc(sizeof(char)*MAX_PASSWORD);
	strncpy(u->user,user,strlen(user));
	strncpy(u->pass,pass,strlen(pass));
	return u;
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

bool isUserExists(USER* users, int size,char* username){
	if(username == NULL || users ==NULL){
		return false;
	}
	for(int i=0;i<size;i++){
		if(strcmp(users[i]->user,username)==0){
			return true;
		}
	}
	return false;
}

USER getUser(USER* users, int size,char* username){
	if(username == NULL || users ==NULL){
		return NULL;
	}
	for(int i=0;i<size;i++){
		if(strcmp(users[i]->user,username)==0){
			return users[i];
		}
	}
	return NULL;
}

char* getUserPassword(USER* users, int size,char* username){
	if(username == NULL || users ==NULL){
		return NULL;
	}
	for(int i=0;i<size;i++){
		if(strcmp(users[i]->user,username)==0){
			return users[i]->pass;
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

void prinUsersDb(USER* users, int size){
	printf("Total Users is: %d\n",size);
	for(int i=0;i<size;i++){
		printf("Username: %s, Password: %s" ,users[i]->user ,users[i]->pass);
	}
}
