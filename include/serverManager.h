#ifndef SOCKET_MANAGER_H
#define SOCKET_MANAGER_H

#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>

typedef enum request_type { CREATE = 'c', JOIN = 'j', LIST = 'l' } reqType;

int launchServer(char*);
int joinServer(char*);
int sendRequest(int, reqType, int, ...);

#endif
