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
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "room.h"

int s_q_id;

typedef enum request_type { CREATE = 'c', JOIN = 'j', LIST = 'l', START = 's', FILL_INFO = 'i' } reqType;

typedef struct {
  long mtype;
  int mid;
  char mtext[1024];
} BaseMessage;

typedef struct {
	long mtype;
	short mno;
	Player mplayer;
} PlayerDataMessage;

int launchServer(char*);
int joinServer(char*);
int sendRequest(int, reqType, int, ...);
void launchRoom(int, rNode**, Room*);

#endif
