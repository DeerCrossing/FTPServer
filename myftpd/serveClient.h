#pragma once


#include  <unistd.h>
#include  <stdlib.h>
#include  <stdio.h> 
#include  <string.h>     // strlen(), strcmp() etc 
#include  <errno.h>      // extern int errno, EINTR, perror() 
#include  <sys/types.h>  // pid_t, u_long, u_short 
#include  <sys/socket.h> // struct sockaddr, socket(), etc 
#include  <netinet/in.h> // struct sockaddr_in, htons(), htonl(),
						 // and INADDR_ANY
#include "../stream.h"


void serveClient(int sd);