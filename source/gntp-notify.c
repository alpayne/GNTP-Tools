/*

gtnp-notify

	Send GTNP notifications to Growl for Mac OSX / Windows, or other GNTP aware notification tools.

	Supports GNTP v1.0

Authors:

	Yasuhiro Matsumoto <mattn.jp@gmail.com>
	Peter Sinnott <link@redbrick.dcu.ie>
	Al Payne <alpayne@pleiades.com>	
	

Copyright 2011

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

static char* string_to_utf8_alloc(const char* str) {
#ifdef _WIN32
	unsigned int codepage;
	size_t in_len = strlen(str);
	wchar_t* wcsdata;
	char* mbsdata;
	size_t mbssize, wcssize;

	codepage = GetACP();
	wcssize = MultiByteToWideChar(codepage, 0, str, in_len,  NULL, 0);
	wcsdata = (wchar_t*) malloc((wcssize + 1) * sizeof(wchar_t));
	wcssize = MultiByteToWideChar(codepage, 0, str, in_len, wcsdata, wcssize + 1);
	wcsdata[wcssize] = 0;

	mbssize = WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR) wcsdata, -1, NULL, 0, NULL, NULL);
	mbsdata = (char*) malloc((mbssize + 1));
	mbssize = WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR) wcsdata, -1, mbsdata, mbssize, NULL, NULL);
	mbsdata[mbssize] = 0;
	free(wcsdata);
	return mbsdata;
#else
	return strdup(str);
#endif
}

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
	char* notification = NULL;
	char* title = NULL;
	char* message = NULL;
	char* icon = NULL;
	char* url = NULL;
	char* sticky = NULL;
	int priority = 0;
	int tcpsend = 1;
	int errorcode = 0;   // 0=no errors, 1=parameters, 2=memory
	
	opterr = 0;

	while ((c = getopts(argc, argv, "a:n:H:P:p:sut:m:") != -1)) {
		switch (optopt) {
		case 'a': appname = optarg; break;
		case 'n': notification = optarg; break;
		case 'H': host = optarg; break;
		case 'P': password = optarg; break;
		case 'p':
			priority = atoi(optarg);
			if ( priority < -2 || priority > 2 ) {
				fprintf(stderr, "ERROR: -p, priority must be between -2 and 2.\n" );
				exit (1);
			}
			break;
		case 'u': tcpsend = 0; break;
		case 's': sticky = "True"; break; 
		case 't': title = optarg; break;
		case 'm': message = optarg; break;
		case '?': errorcode = 1; break;
		default: errorcode = 1; break;
		}
		optarg = NULL;
	}


	if ( !appname || !notification || !title || !message ) 
		errorcode = 1;

	if (errorcode ==0) {
		title = string_to_utf8_alloc(title);
		message = string_to_utf8_alloc(message);
	
		growl_init();	
		if (tcpsend) {
			rc = growl_tcp_notify(host, appname, notification, title, message, password, url, icon, sticky, priority);
		} else {
			rc = growl_udp(host,appname,notification,title,message,icon,password,url);
		}
		growl_shutdown();
	}
	if (errorcode==1) {
		fprintf (stderr,  "%s: [-s] [-p priority] [-H host:port] [-P password] -a application -n notification -t title -m message\n", argv[0]);
		rc = 1;
	}
	if (title) free(title);
	if (message) free(message);
	if (icon) free(icon);
	if (url) free(url);

	return rc;
}
