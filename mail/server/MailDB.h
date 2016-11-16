/*
 * MailDB.h
 *
 *  Created on: Nov 12, 2016
 *      Author: lirongazit
 */

#ifndef MAILDB_H_
#define MAILDB_H_

#include <stdbool.h>
#define MAXMAILS 32000

typedef struct mail_data* DATA;
typedef struct total_mails* MAILS;

bool isMailDBFull(MAILS mails);
int insertMail(char* unparseMail, MAILS mails,char* from);

#endif /* MAILDB_H_ */
