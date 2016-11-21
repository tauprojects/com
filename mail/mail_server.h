/*
 * mail_server.h
 *
 *  Created on: Nov 12, 2016
 *      Author: lirongazit
 */

#ifndef MAIL_SERVER_H_
#define MAIL_SERVER_H_

#include "UsersDB.h"

#include <stdbool.h>


int parseMail(char* unparseMail ,char* from, MAIL mail);
void printMail(MAIL mail);
//void removeSpaces(char* source);
/**
 * Authenticate user connection
 *
 * @params client_message - full message recived from client
 * 			username  - pointer to username value to be fill in.
 * @return - boolean value. True if user authenticated, False otherwise.
 *
 */
bool auth_user(char* client_message, char* username);

/**
 * Convert String Message to DATA object
 *
 * @params string
 * @return - DATA Struct
 *
 */
MAIL strToMail(char* data);

/**
 * Convert String Message to DATA object
 *
 * @params string
 * @return - DATA Struct
 *
 */
MAIL createUser(char* user);


/**
 * Show the mail on inbox for user
 *
 * @params user
 * @return - a list of mails in the user's inbox
 *
 */
char* show_inbox(USER user);

/**
 * Show one mail on inbox for user
 *
 * @params user
 * @return - a one mail in the user's inbox
 *
 */
char* show_mail(MAIL_PER_USER mail);

/**
 * Show mail info in inbox of user
 *
 * @params user, id - mail id in user's inbox
 * @return - a mail info of specific id on user's inbox
 *
 */
char* get_mail(USER user,int id);

/**
 * Delete mail from the server
 *
 * @params user, id - mail id in user's inbox
 * @return - 0 on success, -1 if there is no such id
 *
 */
int delete_mail(USER user,int id);

/**
 * compose mail in the server
 *
 * @params from,to, subject and content of the mail
 * @return - 0 on success, -1 if mail server is full, -2 if number of recipient > 20
 *
 */
int compose(TOTAL_MAILS mailsDB,UsersDB users, MAIL mail);

char** str_split(char* a_str, const char a_delim,int* countStr);
#endif /* MAIL_SERVER_H_ */
