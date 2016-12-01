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
bool auth_user(char* client_message, char* username,USER* users, int size);

/**
 * Convert String Message to DATA object
 *
 * @params string
 * @return - DATA Struct
 *
 */
MAIL strToMail(char* data);

/**
 * Show the mails on inbox for user
 *
 * @params user,pointer to return message
 * @return - int - 0 for success, -1 for wrong ID, -2 if isTrash, -3 Internal Server Error
 */
int showInbox(USER user,TOTAL_MAILS mailDB,char* msg);

/**
 * Show one mail on inbox for user
 *
 * @params user
 * @return - a one mail in the user's inbox
 *
 */
int showMail(MAIL mail, int id,char *resMail);

/**
 * Show Specific mail by ID number
 *
 * @params user, id - mail id in user's inbox, pointer to return message
 * @return - int - 0 for success, -1 for wrong ID, -2 if isTrash, -3 Internal Server Error
 *
 */
int getMail(USER user,TOTAL_MAILS mailDB,int id, char* msg);

/**
 * Delete mail from the server
 *
 * @params user, id - mail id in user's inbox
 * @return - 0 on success, -1 if there is no such id
 *
 */
int deleteMail(USER user, TOTAL_MAILS mailDB,int id);

/**
 * compose mail in the server
 *
 * @params from,to, subject and content of the mail
 * @return - 0 on success, -1 if mail server is full, -2 if number of recipient > 20
 *
 */
int compose(TOTAL_MAILS mailsDB,USER* users,int size, MAIL mail);

#endif /* MAIL_SERVER_H_ */
