#include "UsersDB.h"
#include <assert.h>

char** str_split(char* a_str, const char a_delim, int* cnt) {
	char** result = 0;
	size_t count = 0;
	char* tmp = a_str;
	char* last_comma = 0;
	char delim[2];
	delim[0] = a_delim;
	delim[1] = 0;
	int i=0;

	/* Count how many elements will be extracted. */
	while (*tmp) {
		if (a_delim == *tmp) {
			count++;
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
		i++;
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
	*cnt = count -1;
	return result;
}


//Fix for cahr* instead char[]
int UsersDBCreate(const char* filename, USER* users){
	int size =0;
	if(filename==NULL){return -1;}  //checking valid file name
	FILE* fp = fopen(filename, "r");
	if (fp == NULL) {return -1;}    //checking file
	//temporary variables
	int lineNum=0;
//	char** splitArgs;
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
//		printf("%s\n", tempLine);
//		splitArgs = str_split(tempLine, ' ');
//		printf("user: %s pass: %s\n", tempLine);

		temp = strchr(tempLine, '\t');
//		temp = strtok(tempLine, "    ");
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
//		users[size]=createUser(splitArgs[0],splitArgs[1]);
//		free(splitArgs);
//		printf("\nUsername: %s",users[size]->user);
//		printf("\nPassword: %s",users[size]->pass);
		size++;
		lineNum++;
	}
//	printf("\nLines number: %d ",size);
	free(username);
	free(password);
	fclose(fp);
	return size;
}

USER createUser(char* user,char* pass){
	USER u=(USER)malloc(sizeof(USER));
	u->user=(char*)malloc(sizeof(char)*MAX_USERNAME);
	u->pass =  (char*)malloc(sizeof(char)*MAX_PASSWORD);
	u->mailsize=0;
	strncpy(u->user,user,strlen(user));
	strncpy(u->pass,pass,strlen(pass));
//	u->mail = (MAIL*)malloc(sizeof(MAIL)*MAXMAILS);
	return u;
}

void freeUser(USER u){
	free(u->user);
	free(u->pass);
	free(u->user);
	free(u);
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
		fflush(NULL);
		printf("Username: %s, Password: %s\n" ,users[i]->user ,users[i]->pass);
	}
}
void freeMailDB(TOTAL_MAILS mailsDB){
	freeMails(mailsDB->mails,mailsDB->size);
}

void freeMails(MAIL* mails,int size){
	for(int i=0;i<size;i++){
		free(mails[i]->from);
		free(mails[i]->subject);
		free(mails[i]->content);
		free(mails[i]->to);
	}
}
