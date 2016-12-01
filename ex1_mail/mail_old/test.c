#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#define MAXMAILS 32000
#define MAX_USERNAME 100
#define MAX_PASSWORD 100
#define MAX_SUBJECT 100
#define MAX_CONTENT 2000
#define NUM_OF_CLIENTS 20
#define MAX_TEMP 1024
#define TEMP_ID 6;
typedef enum USER_COMMAND {
	AUTH,
	SHOW_INBOX,
	GET_MAIL,
	DELETE_MAIL,
	QUIT,
	COMPOSE
} USER_CMD;

struct mail_data{
	bool isTrash;
	int totalTo;
	char* from;  //check name size
	char* subject;
	char* content;
	char** to; //check name size

};
typedef struct user_mail* MAIL_PER_USER;
typedef struct user_struct* USER;
typedef struct users_db* UsersDB;
typedef struct mail_data* MAIL;
typedef struct total_mails* TOTAL_MAILS;

struct total_mails{
	MAIL mails[MAXMAILS];
	int size;
};

 struct user_mail{
	int id;
	MAIL data;
};
 struct user_struct {
 	char* user;
 	char* pass;
 	MAIL_PER_USER* mail;
 	int mailsize;
 };
struct users_db {
	USER usersList[NUM_OF_CLIENTS];
	int size;
};
int removeSpaces(char* source);
void prinUsersDb(USER* users,int size);
int UsersDBCreate(const char* filename, USER* users);

int main(){
	int n =5;
//	char* msg = (char*)calloc(sizeof(char),5);
//	sprintf(msg,"%4d",n);
	printf("\n%05d\n", n);

}

USER createUser(char* user,char* pass);

//int UsersDBCreate(const char* filename, USER* users){
//	int size =0;
//	if(filename==NULL){return -1;}  //checking valid file name
//	FILE* fp = fopen(filename, "r");
//	if (fp == NULL) {return -1;}    //checking file
//	//temporary variables
//	int lineNum=0;
//	char tempLine[1024];
//	char* temp;
//	bool isValidLine=true;
//	USER ustemp;
//	char* username = (char*)malloc(sizeof(char)*MAX_USERNAME);
//	char* password =  (char*)malloc(sizeof(char)*MAX_PASSWORD);
//	while (fgets(tempLine,1024, fp) != NULL ) {
//		if(tempLine[strlen(tempLine)-1]=='\n'){
//			tempLine[strlen(tempLine)-1]='\0';
//		}
//		else{
//			tempLine[strlen(tempLine)]='\0';
//		}
//		temp = strchr(tempLine, '\t');
//		if(temp==NULL){ isValidLine=false;}
//		else{
//			*temp='\0';
//			strcpy(username,tempLine);
//			strcpy(password,temp+1);
//			isValidLine= removeSpaces(username) && removeSpaces(password);
//		}
//		if(!isValidLine){  //checking line
//			fclose(fp);
//			return -1;
//		}
//		users[size]=createUser(username,password);
//		printf("\nUsername: %s",users[size]->user);
//		printf("\nPassword: %s",users[size]->pass);
//		size++;
//		lineNum++;
//	}
//	printf("\nLines number: %d ",size);
//	free(username);
//	free(password);
//	return size;
//}


//void prinUsersDb(USER* users,int size){
//	printf("\nTotal Users is: %d\n",size);
//	for(int i=0;i<size;i++){
//		printf("Username: %s, Password: %s" ,users[i]->user ,users[i]->pass);
//	}
//}

//
//USER createUser(char* user,char* pass){
//	USER u=(USER)malloc(sizeof(USER));
//	u->mailsize=0;
//	u->user=(char*)malloc(sizeof(char)*MAX_USERNAME);
//	u->pass =  (char*)malloc(sizeof(char)*MAX_PASSWORD);
//	strncpy(u->user,user,strlen(user));
//	strncpy(u->pass,pass,strlen(pass));
//	return u;
//}
//
//int removeSpaces(char* source){
//  char* i = source;
//  char* j = source;
//  while(*j != 0){
//    *i = *j++;
//    if(*i != ' ')
//      i++;
//  }
//  *i = 0;
//  return 1;
//}
