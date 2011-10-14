/*

gtnp-register

	Send GTNP register requests to Growl for Mac OSX / Windows, or other GNTP aware notification tools.

	Supports GNTP v1.0

Authors:

	Yasuhiro Matsumoto <mattn.jp@gmail.com>
	Peter Sinnott <link@redbrick.dcu.ie>
	Al Payne <alpayne@pleiades.com>	

Licensed under the BSD License

*/

#ifdef _WIN32
#include <winsock2.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "md5.h"
#include "tcp.h"
#include "growl.h"

/* variables */
int opterr = 1;                 /* getopt prints errors if this is on */
int optind = 1;                 /* token pointer */
int optopt;                     /* option character passed back to user */
char *optarg;                   /* flag argument (or value) */

/* function */
/* return option character, EOF if no more or ? if problem.
        The arguments to the function:
        argc, argv - the arguments to the main() function. An argument of "--"
        stops the processing.
        opts - a string containing the valid option characters.
        an option character followed by a colon (:) indicates that
        the option has a required argument.
*/
int getopts(int argc, char** argv, char* opts) {
	static int sp = 1;		/* character index into current token */
	register int c;
	register char *cp; 		/* pointer into current token */


	if(sp == 1) {
        /* check for more flag-like tokens */
		if(optind >= argc  || argv[optind][1] == '\0')
			return(EOF);
		else if (argv[optind][0] != '-' ) {
 	        fprintf (stderr, "%s: invalid option -- '%s'\n", argv[0], argv[optind]);
			sp = 1;
			optind++;
		} else if(strcmp(argv[optind], "--") == 0) {
			optind++;
			return(EOF);
		}
	}
	optopt = c = argv[optind][sp];

	if(c == ':' || (cp=strchr(opts, c)) == NULL) {
      //  if (opterr)
 	       fprintf (stderr, "%s: invalid option -- '%c'\n", argv[0], optopt);

        /* if no characters left in this token, move to next token */
		if(argv[optind][++sp] == '\0') {
			optind++;
			sp = 1;
		}
		return('?');
	}
	if(*++cp == ':') {
		/* if a value is expected, get it */
		if(argv[optind][sp+1] != '\0')
			optarg = &argv[optind++][sp+1];
		else if(++optind >= argc) {
			sp = 1;
			return('?');
		} else {
			optarg = argv[optind++];
		}
		sp = 1;
	} else {
		 /* set up to look at next char in token, next time */
		if(argv[optind][++sp] == '\0') {
			/* no more in current token, so setup next token */
			sp = 1;
			optind++;
		}
		optarg = NULL;
	}
    return(c);
}

int main(int argc, char* argv[]) {
	int c;
	int rc;
	char* host = "127.0.0.1";
	char* password = NULL;
	char* appname = "gntp-notify";
	char* icon = NULL;
	int tcpsend = 1;
	int errorcode = 0;   // 0=no errors, 1=parameters, 2=memory

	int notifications_count = -1;
	char** notifications = malloc(sizeof(char*));
	if ( notifications == NULL ) {
		fprintf( stderr, "ERROR: Memory could not be allocated for notifications array.  Exiting.\n" );
		exit (2);
	}
	
	opterr = 0;

	while ((c = getopts(argc, argv, "a:n:H:P:") != -1)) {
		switch (optopt) {
			case 'a': appname = optarg; break;
			case 'n': 
				notifications_count++;
	            char** tmp = realloc( notifications, (notifications_count+1) * sizeof(char*));
				if ( tmp == NULL ) {
					errorcode=2;
				} else {
					notifications = tmp;
					tmp = NULL;
				}
	            notifications[notifications_count] = malloc( strlen(optarg) );
				strcpy (notifications[notifications_count], optarg); 
				break;
			case 'H': host = optarg; break;
			case 'P': password = optarg; break;
			case 'u': tcpsend = 0; break;
			case '?': errorcode = 1; break;
			default: errorcode=1; break;
		}
		optarg = NULL;
	}

	if ( !appname || notifications_count < 0 ) 
		errorcode = 1;

	if (errorcode==0) {
		growl_init();	

		if (tcpsend) {
			rc = growl_tcp_register(host,appname,(const char **const)notifications,notifications_count,password, icon);
		}
		growl_shutdown();
	}

	if (errorcode==1) {
		fprintf (stderr,  "%s: [-H host:port] [-P password] -a application -n notification ...\n", argv[0]);
		rc = 1;
	} else if (errorcode==2) {
		fprintf (stderr, "ERROR: Memory could not be allocated for notifications array. Exiting.\n");
		rc = 2;
	}
	if (notifications) {
		free (notifications);
		notifications = NULL;
	}

	return rc;
}
