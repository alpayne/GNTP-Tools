/*

growl.c

	Support libraries for for GTNP v1.0 transactions.

Authors:

	Yasuhiro Matsumoto <mattn.jp@gmail.com>
	Peter Sinnott <link@redbrick.dcu.ie>
	Al Payne <alpayne@pleiades.com>	

Copyright 2011

Licensed under the BSD License

*/

#ifdef _WIN32
#include <windows.h>
#else
#include <arpa/inet.h>
#include <stdint.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "md5.h"
#include "tcp.h"
#include "growl.h"

static const char hex_table[] = "0123456789ABCDEF";
static char* string_to_hex_alloc(const char* str, int len) {
	int n, l;
	char* tmp = (char*)calloc(1, len * 2 + 1);
	if (tmp)
    {
        for (l = 0, n = 0; l < len; l++) {
            tmp[n++] = hex_table[(str[l] & 0xF0) >> 4];
            tmp[n++] = hex_table[str[l] & 0x0F];
        }
    }
    return tmp;
}

static volatile int growl_init_ = 0;

int growl_init()
{
        if( growl_init_ == 0)
        {
                #ifdef _WIN32
                WSADATA wsaData;
                if( WSAStartup( MAKEWORD( 2 , 0 ) , &wsaData) != 0 )
                {
                        return -1;
                }
                #endif

                srand(time(NULL));
                growl_init_ = 1;
        }
        return 1;
}


void growl_shutdown()
{
        if( growl_init_ == 1 )
        {
                #ifdef _WIN32
                WSACleanup();
                #endif
        }
}


char* gen_salt_alloc(int count) {
	char* salt = (char*)malloc(count + 1);
	int n;
	for (n = 0; n < count; n++) salt[n] = (((int)rand()) % 255) + 1;
	salt[n] = 0;
	return salt;
}

char* gen_password_hash_alloc(const char* password, const char* salt) {
	md5_context md5ctx;
	char md5tmp[20] = {0};
	char* md5digest;

	md5_starts(&md5ctx);
	md5_update(&md5ctx, (uint8_t*)password, strlen(password));
	md5_update(&md5ctx, (uint8_t*)salt, strlen(salt));
	md5_finish(&md5ctx, (uint8_t*)md5tmp);

	md5_starts(&md5ctx);
	md5_update(&md5ctx, (uint8_t*)md5tmp, 16);
	md5_finish(&md5ctx, (uint8_t*)md5tmp);
	md5digest = string_to_hex_alloc(md5tmp, 16);

	return md5digest;
}

char *growl_generate_authheader_alloc(const char*const password)
{
	char* authheader = NULL;

	if (password) {
		char *salt = gen_salt_alloc(8);
		char *keyhash = gen_password_hash_alloc(password, salt);
		char *salthash = string_to_hex_alloc(salt, 8);
		free(salt);
		authheader = (char*)malloc(strlen(keyhash) + strlen(salthash) + 7);
		sprintf(authheader, " MD5:%s.%s", keyhash, salthash);
		free(salthash);
		free(keyhash);
	}
	
	return authheader;
}


int growl_tcp_register( const char *const server , const char *const appname , const char **const notifications , const int notifications_count ,
		const char *const password, const char* const icon  )
{
	int sock = -1;
	int i=0;
	char *authheader;

	growl_init();
	authheader = growl_generate_authheader_alloc(password);
	sock = growl_tcp_open(server);
	if (sock == -1) goto leave;
    
	growl_tcp_write(sock, "GNTP/1.0 REGISTER NONE %s", authheader ? authheader : "");
	growl_tcp_write(sock, "Application-Name: %s", appname);
	if(icon) growl_tcp_write(sock, "Application-Icon: %s", icon);	
	growl_tcp_write(sock, "Notifications-Count: %d", notifications_count + 1);
	growl_tcp_write(sock, "" );

	for(i=0;i<=notifications_count;i++)
	{
		growl_tcp_write(sock, "Notification-Name: %s", notifications[i]);
		growl_tcp_write(sock, "Notification-Display-Name: %s", notifications[i]);
		growl_tcp_write(sock, "Notification-Enabled: True" );
		if(icon) growl_tcp_write(sock, "Notification-Icon: %s",  icon);
		growl_tcp_write(sock, "" );
	}
	while (1) {
		char* line = growl_tcp_read(sock);
		int len = strlen(line);
		if (strncmp(line, "GNTP/1.0 -ERROR", 15) == 0) {
			fprintf(stderr, "Failed to register application.\n");
			free(line);
			goto leave;
		}
		free(line);
		if (len == 0) break;
	}
	growl_tcp_close(sock);
	sock = 0;

	leave:
	free(authheader);

	return (sock == 0) ? 0 : -1;
}





int growl_tcp_notify( const char *const server,const char *const appname,const char *const notify,const char *const title, const char *const message ,
                                const char *const password, const char* const url, const char* const icon, const char* const sticky, const int priority)
{
	int sock = -1;

	char *authheader = growl_generate_authheader_alloc(password);
	
	growl_init();

	sock = growl_tcp_open(server);
	if (sock == -1) goto leave;

	growl_tcp_write(sock, "GNTP/1.0 NOTIFY NONE %s", authheader ? authheader : "");
	growl_tcp_write(sock, "Application-Name: %s", appname);
	growl_tcp_write(sock, "Notification-Name: %s", notify);
	growl_tcp_write(sock, "Notification-Title: %s", title);
	growl_tcp_write(sock, "Notification-Text: %s", message);
	if (sticky) growl_tcp_write(sock, "Notification-Sticky: %s", sticky);
	if (priority) growl_tcp_write(sock, "Notification-Priority: %d", priority);
	if (icon) growl_tcp_write(sock, "Notification-Icon: %s", icon);
	if (url) growl_tcp_write(sock, "Notification-Callback-Target: %s", url  );

	growl_tcp_write(sock, "");
	while (1) {
		char* line = growl_tcp_read(sock);
		int len = strlen(line);
		if (strncmp(line, "GNTP/1.0 -ERROR", 15) == 0) {
			fprintf(stderr, "failed to post notification\n");
			free(line);
			goto leave;
		}
		free(line);
		if (len == 0) break;
	}
	growl_tcp_close(sock);
	sock = 0;

leave:
	free(authheader);

	return (sock == 0) ? 0 : -1;
}

void growl_append_md5( unsigned char *const data , const int data_length , const char *const password )
{
	md5_context md5ctx;
	char md5tmp[20] = {0};

	md5_starts(&md5ctx);
	md5_update(&md5ctx, (uint8_t*)data, data_length );
	if(password != NULL)
	{
		md5_update(&md5ctx, (uint8_t*)password, strlen(password));
	}
	md5_finish(&md5ctx, (uint8_t*)md5tmp);

	memcpy( data + data_length , md5tmp , 16 );
}


int growl_udp_register( const char *const server , const char *const appname , const char **const notifications , const int notifications_count , const char *const password  )
{
	int register_header_length = 22+strlen(appname);
	unsigned char *data;
	int pointer = 0;
	int rc = 0;
	int i=0;

	uint8_t GROWL_PROTOCOL_VERSION  = 1;
	uint8_t GROWL_TYPE_REGISTRATION = 0;

	uint16_t appname_length = ntohs(strlen(appname));
	uint8_t _notifications_count = notifications_count;
	uint8_t default_notifications_count = notifications_count;
	uint8_t j;

	growl_init();

	for(i=0;i<notifications_count;i++)
	{
		register_header_length += 3 + strlen(notifications[i]);
	}	
	data = (unsigned char*)calloc(1, register_header_length);

	pointer = 0;
	memcpy( data + pointer , &GROWL_PROTOCOL_VERSION , 1 );	
	pointer++;
	memcpy( data + pointer , &GROWL_TYPE_REGISTRATION , 1 );
	pointer++;
	memcpy( data + pointer , &appname_length , 2 );	
	pointer += 2;
	memcpy( data + pointer , &_notifications_count , 1 );	
	pointer++;
	memcpy( data + pointer, &default_notifications_count , 1 );	
	pointer++;
	sprintf( (char*)data + pointer , "%s" , appname );
	pointer += strlen(appname);

	for(i=0;i<notifications_count;i++)
	{
		uint16_t notify_length = ntohs(strlen(notifications[i]));
		memcpy( data + pointer, &notify_length , 2 );		
		pointer +=2;
		sprintf( (char*)data + pointer , "%s" , notifications[i] );
		pointer += strlen(notifications[i]);
	} 

	for(j=0;j<notifications_count;j++)
	{
		memcpy( data + pointer , &j , 1 );
		pointer++;
	}

	growl_append_md5( data , pointer , password );
	pointer += 16;

	rc = growl_tcp_datagram( server , data , pointer );
	free(data);
	return rc;
}


int growl_udp_notify( const char *const server,const char *const appname,const char *const notify,const char *const title, const char *const message ,
                                const char *const password )
{
	int notify_header_length = 28 + strlen(appname)+strlen(notify)+strlen(message)+strlen(title);
	unsigned char *data = (unsigned char*)calloc(1, notify_header_length);
	int pointer = 0;
	int rc = 0;

	uint8_t GROWL_PROTOCOL_VERSION  = 1;
	uint8_t GROWL_TYPE_NOTIFICATION = 1;

	uint16_t flags = ntohs(0);
	uint16_t appname_length = ntohs(strlen(appname));
	uint16_t notify_length = ntohs(strlen(notify));
	uint16_t title_length = ntohs(strlen(title));
	uint16_t message_length = ntohs(strlen(message));

	growl_init();
	
	pointer = 0;
	memcpy( data + pointer , &GROWL_PROTOCOL_VERSION , 1 );	
	pointer++;
	memcpy( data + pointer , &GROWL_TYPE_NOTIFICATION , 1 );
	pointer++;
	memcpy( data + pointer , &flags , 2 );
	pointer += 2;
	memcpy( data + pointer , &notify_length , 2 );	
	pointer += 2;
	memcpy( data + pointer , &title_length , 2 );	
	pointer += 2;
	memcpy( data + pointer , &message_length , 2 );	
	pointer += 2;
	memcpy( data + pointer , &appname_length , 2 );	
	pointer += 2;
	strcpy( (char*)data + pointer , notify );
	pointer += strlen(notify);
	strcpy( (char*)data + pointer , title );
	pointer += strlen(title);
	strcpy( (char*)data + pointer , message );
	pointer += strlen(message);
	strcpy( (char*)data + pointer , appname );
	pointer += strlen(appname);


	growl_append_md5( data , pointer , password );
	pointer += 16;


	rc = growl_tcp_datagram( server , data , pointer );
	free(data);
	return rc;
}


int growl_udp( const char *const server,const char *const appname,const char *const notify,const char *const title, const char *const message ,
                                const char *const icon , const char *const password , const char *url )
{
	int rc = growl_udp_register(  server ,  appname ,  (const char **const)&notify , 1 , password  );
	if( rc == 0 )
	{
		rc = growl_udp_notify( server, appname, notify, title,  message , password );
	}
	return rc;
	(void)icon; (void)url; /* prevent unused warnings */
}


#ifdef _WIN32

static void GrowlNotify_impl_(HWND hwnd, HINSTANCE hinst, LPSTR lpszCmdLine, int nCmdShow) {
	char* server = "127.0.0.1:23053";
	char* password = NULL;
	char* appname = "gntp-send";
	char* notify = "gntp-send notify";
	char* title = NULL;
	char* message = NULL;
	char* icon = NULL;
	char* url = NULL;
	char* first = strdup(lpszCmdLine);
	char* ptr = first;
	#define SKIP(x)	while (*x && *x != ' ') x++; if (*x == ' ') *x++ = 0;
	server = ptr;  SKIP(ptr);
	appname = ptr; SKIP(ptr);
	notify = ptr;  SKIP(ptr);
	title = ptr;   SKIP(ptr);
	message = ptr; SKIP(ptr);
	icon = ptr;    SKIP(ptr);
	url = ptr;     SKIP(ptr);
	growl(server,appname,notify,title,message,icon,password,url);
	free(first);
}

void GrowlNotify(HWND hwnd, HINSTANCE hinst, LPSTR lpszCmdLine, int nCmdShow) {
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) return;
	GrowlNotify_impl_(hwnd, hinst, lpszCmdLine, nCmdShow);
	WSACleanup();
}
#endif

