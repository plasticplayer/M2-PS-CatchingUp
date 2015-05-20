//
//  Udp.cpp
//  server
//
//  Created by maxime max on 04/03/2015.
//  Copyright (c) 2015 Maxime Leblanc. All rights reserved.
//

#include "Tcp_Socket_Server.h"
#include "Tcp.h"
#include "Config.h"
#include "define.h"
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <errno.h>
#include <arpa/inet.h>

#ifdef SSL_ENABLE
#include "openssl/ssl.h"
#include "openssl/err.h"
#endif

#define BUFFSIZE 256


using namespace std;


#ifdef SSL_ENABLE
SSL_CTX* InitServerCTX(void)
{   SSL_METHOD *method;
    SSL_CTX *ctx;
 
    OpenSSL_add_all_algorithms();  	/* load & register all cryptos, etc. */
    SSL_load_error_strings();   	/* load all error messages */
    method =(SSL_METHOD*) SSLv3_server_method();  /* create new server-method instance */
    ctx = SSL_CTX_new(method);   	/* create new context from method */
    if ( ctx == NULL )
    {
        ERR_print_errors_fp(stderr);
        abort();
    }
    return ctx;
}
void LoadCertificates(SSL_CTX* ctx, char* CertFile, char* KeyFile)
{
    /* set the local certificate from CertFile */
    if ( SSL_CTX_use_certificate_file(ctx, CertFile, SSL_FILETYPE_PEM) <= 0 )
    {
        ERR_print_errors_fp(stderr);
        abort();
    }
    /* set the private key from KeyFile (may be the same as CertFile) */
    if ( SSL_CTX_use_PrivateKey_file(ctx, KeyFile, SSL_FILETYPE_PEM) <= 0 )
    {
        ERR_print_errors_fp(stderr);
        abort();
    }
    /* verify private key */
    if ( !SSL_CTX_check_private_key(ctx) )
    {
        fprintf(stderr, "Private key does not match the public certificate\n");
        abort();
    }
}
 
void ShowCerts(SSL* ssl)
{   X509 *cert;
    char *line;
 
    cert = SSL_get_peer_certificate(ssl); /* Get certificates (if available) */
    if ( cert != NULL )
    {
        printf("Server certificates:\n");
        line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        printf("Subject: %s\n", line);
        free(line);
        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
        printf("Issuer: %s\n", line);
        free(line);
        X509_free(cert);
    }
    else
        printf("No certificates.\n");
}

#endif


Tcp_Socket_Server* Tcp_Socket_Server::tcp_socket_server = NULL ;
int Tcp_Socket_Server::_Port ;

Tcp_Socket_Server::Tcp_Socket_Server( int port ) {
	_Port = port;
	tcp_socket_server = this;
}

int Tcp_Socket_Server::getPort(){
	if ( tcp_socket_server == NULL )
		return -1;

	else return _Port;
}

void Tcp_Socket_Server::start(){

	if( pthread_create( &_ThreadListenner , NULL ,  &(this->listenner) , (void*) this ) < 0)
	{
		LOGGER_ERROR("TCP : Error: could not create thread");
	}
}




void* Tcp_Socket_Server::listenner( void* data){
	#ifdef SSL_ENABLE
	SSL_library_init();
	SSL_CTX *ctx = InitServerCTX();        /* initialize SSL */
    	LoadCertificates(ctx, (char*) "mycert.pem", (char*) "mycert.pem"); /* load certs */
	SSL *ssl;
	#endif
	
	int socket_desc , client_sock , c;
	struct sockaddr_in server , client;


	//Create socket
	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
	if (socket_desc == -1)
	{
		LOGGER_ERROR("TCP : Could not create socket");
	}

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons( _Port );

	//Bind
	if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
	{
		//print the error message
		LOGGER_ERROR("Tcp bind failed. Error");
		return NULL;
	}
	LOGGER_DEBUG("TCP : Bind done");

	//Listen
	listen(socket_desc , 3);

	//Accept and incoming connection
	LOGGER_DEBUG("Tcp : Waiting for incoming connections...");
	c = sizeof(struct sockaddr_in);


	Recorder *r;

	while( ( client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
	{
		LOGGER_DEBUG("Connection accepted");

		r = Recorder::findRecorderByIp( inet_ntoa(client.sin_addr) );

		if ( r == NULL ){
			continue;
		}
		#ifdef SSL_ENABLE
		ssl = SSL_new(ctx);            	/* get new SSL state with context */
        	SSL_set_fd(ssl, client_sock);      	/* set connection socket to SSL state */
		if ( SSL_accept(ssl ) == -1 ){
			LOGGER_ERROR("SSL_accept failed");
		}
		ShowCerts(ssl);        		/* get any certificates */
   
		Tcp *client = new Tcp(  client_sock , r , ssl);
		#else
		Tcp *client = new Tcp(  client_sock , r );
		#endif
		r->setTcpSocket(client);
		client->deco[0] = (FuncDeco)&(r->decoTcp) ;
		client->start();
	}
	
	#ifdef SSL_ENABLE
	SSL_CTX_free(ctx);         /* release context */
	#endif
	return NULL;
}
