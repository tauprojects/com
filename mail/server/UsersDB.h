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
#include "MailDB.h"

/**
 * A data-structure which is used for configuring the system.
 */
#define MAXLEN 1025

typedef struct user_struct* USER;
typedef struct users_db* UsersDB;
typedef struct user_mail* MAIL;

/**
 *
 *
 */
UsersDB UsersDBCreate(const char* filename);

bool isUserExists(UsersDB users,char* username);

char* getUserPassword(UsersDB users,char* username);

USER getUser(UsersDB users,char* username);




#endif /* USERSDB_H_ */
