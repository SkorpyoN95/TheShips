#ifndef SOCKET_MANAGER_H
#define SOCKET_MANAGER_H

#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>

int launchServer(char*);
int joinServer(char*);

#endif
