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

 struct user_mail{
	int id;
	MAIL data;
};
struct user_struct {
	char user[100];
	char pass[100];
//	MAIL_PER_USER* mail;
	int mailsize;
};
struct users_db {
	USER* usersList;
	int size;
};
int removeSpaces(char* source);
void prinUsersDb(UsersDB users);
UsersDB UsersDBCreate(const char* filename, UsersDB users);

int main(){
	const char* filename = "./users.txt";
	UsersDB users =  (UsersDB)malloc(sizeof(UsersDB));
	users->usersList = (USER*)malloc(sizeof(users->usersList)*10);
	for(int i=0;i<10;i++){
		strcat(users->usersList[i]->user,"aaa");
//		users->usersList[i]->pass = (char*)malloc(sizeof(char)*100);

	}
	UsersDBCreate(filename,users);
	prinUsersDb(users);

}

UsersDB UsersDBCreate(const char* filename, UsersDB users){
	users->size=0;
	if(filename==NULL){return NULL;}  //checking valid file name
	FILE* fp = fopen(filename, "r");
	if (fp == NULL) {return NULL;}    //checking file

	//temporary variables
	int lineNum=0;

	char tempLine[1024];
	char* temp;
	bool isValidLine=true;
	while (fgets(tempLine,1024, fp) != NULL) {
		char* username = (char*)malloc(sizeof(char)*100);
		char* password =  (char*)malloc(sizeof(char)*100);
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
//		users->usersList[users->size]->user = (char*)malloc(sizeof(char)*2);
		printf("\nUsername: %s",username);
		printf("\nPassword: %s",password);

//		users->usersList[users->size]->user=(char*)malloc(sizeof(char)*2);
//		strncpy(users->usersList[users->size]->user,username,strlen(username));
//		strcat(users->usersList[users->size]->pass,password);
//		users->size++;
		lineNum++;
	}
	printf("\nLines number: %d ",users->size);
//	fclose(fp);
	return users;
}


void prinUsersDb(UsersDB users){
	int size = users->size;
	printf("\nTotal Users is: %d\n",size);
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
  return 1;
}
