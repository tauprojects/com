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
	char* from;  //check name size
	char** to; //check name size
	int totalTo;
	char* subject;
	char* content;
	bool isTrash;
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

typedef struct user_mail* MAIL_PER_USER;
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
	USER* usersList;
	int size;
};
int removeSpaces(char* source);
void prinUsersDb(UsersDB users);
UsersDB UsersDBCreate(const char* filename);

int main(){
	const char* filename = "./users.txt";
	UsersDB users = UsersDBCreate(filename);
	prinUsersDb(users);

}

UsersDB UsersDBCreate(const char* filename){
	UsersDB users = (UsersDB)malloc(sizeof(users));
	users->usersList = (USER*)malloc(sizeof(USER)*1024);
	users->size=0;
	if(filename==NULL){return NULL;}  //checking valid file name
	FILE* fp = fopen(filename, "r");
	if (fp == NULL) {return NULL;}    //checking file

	//temporary variables
	int lineNum=0;
	char* username = (char*)malloc(sizeof(char)*100);
	char* password =  (char*)malloc(sizeof(char)*100);
	char tempLine[1024];
	char* temp;
	bool isValidLine=true;
	while (fgets(tempLine,1024, fp) != NULL) {
		tempLine[strlen(tempLine)-1]='\0';
		printf("\nLine: %s",tempLine);

		temp = strchr(tempLine, '\t');
		printf("\nuser: %s",temp);
		if(temp==NULL){ isValidLine=false;}
		else{
			*temp='\0';
			strcpy(username,tempLine);
			strcpy(password,temp+1);
			isValidLine= removeSpaces(username) && removeSpaces(password);
		}
//		if(!isValidLine){  //checking line
//			fclose(fp);
//			return NULL;
//		}
//		users->usersList[users->size]->user=username;
//		users->usersList[users->size]->pass=password;
//		users->size++;
//		lineNum++;
	}
//	fclose(fp);
	return users;
}


void prinUsersDb(UsersDB users){
	int size = users->size;
	printf("Total Users is: %d\n",size);
	for(int i=0;i<size;i++){
		printf("Username: %s, Password: %s" ,users->usersList[i]->user ,users->usersList[i]->pass);
	}
}

int removeSpaces(char* source){
  char* i = source;
  char* j = source;
  while(*j != 0){
    *i = *j++;
    if(*i != ' ')
      i++;
  }
  *i = 0;
  return 0;
}
