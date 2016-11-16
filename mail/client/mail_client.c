/*
 * mail_client.c
 *
 *  Created on: Nov 12, 2016
 *      Author: lirongazit
 */

#define DEFAULT_PORT 6423;
#define DEFAULT_HOST "localhost";

int main(int argc, char* argv[]) {
	//Checking argument state
	char *hostname;
	int port;
	if(argc ==1){
		hostname=DEFAULT_HOST;
		port=DEFAULT_PORT;
	}
	else if (argc == 2) {
		hostname=argv[1];
		port=DEFAULT_PORT;
	} else if (argc == 3 ) {
		hostname=argv[1];
		port=argv[2];
	} else {
		return -1; //exit(1)
	}

// after he get an "welcome" messages he will send user&password


	return 0;
}

