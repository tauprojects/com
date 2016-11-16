/*
 * main_server.c
 *
 *  Created on: Nov 12, 2016
 *      Author: lirongazit
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mail_server.h"

#define DEFAULT_PORT 6423;


int main(int argc, char* argv[]) {
	//Checking argument state
	char *users_file;
	int port;
	if (argc == 2) {
		users_file=argv[1];
		port=DEFAULT_PORT;
	} else if (argc == 3 ) {
		users_file=argv[1];
		port=argv[2];
	} else {
		return -1; //exit(1)
	}
	UsersDB users = UsersDBCreate(users_file);



	return 0;
}




