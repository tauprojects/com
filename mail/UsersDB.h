#ifndef USERSDB_H_
#define USERSDB_H_

#include <stdbool.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>


//Constants Declarations
#define DEFAULT_PORT 6423

#define TOTAL_TO 20

#define MAXMAILS 32000
#define MAX_USERNAME 100
#define MAX_PASSWORD 100
#define MAX_SUBJECT 100
#define MAX_CONTENT 2000
#define NUM_OF_CLIENTS 20
#define MAX_TEMP 1024
#define TEMP_ID 6
/**
 * A data-structure which is used for configuring the system.
 */


typedef struct user_mail* MAIL_PER_USER;
typedef struct user_struct* USER;
typedef struct users_db* UsersDB;
typedef struct mail_data* MAIL;
typedef struct total_mails* TOTAL_MAILS;


//struct user_mail{
//	int id;
//	MAIL data;
//};

struct user_struct {
	char* user;
	char* pass;
	MAIL mail[MAXMAILS];
	int mailsize;
};

struct mail_data{
	char* from;  //check name size
	char** to; //check name size
	int totalTo;
	char* subject;
	char* content;
	bool isTrash;
};

struct total_mails{
	MAIL mails[MAXMAILS];
	int size;
};

/**
 *
 *
 */
int UsersDBCreate(const char* filename, USER* users);

USER createUser(char* user,char* pass);

void prinUsersDb(USER* users, int size);
bool isUserExists(USER* users, int size,char* username);

char* getUserPassword(USER* users, int size,char* username);

USER getUser(USER* users, int size,char* username);

bool removeSpaces(char* source);
bool isCommentLine(char* tempLine);

char** str_split(char* a_str, const char a_delim);

#endif /* USERSDB_H_ */
