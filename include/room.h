#ifndef ROOM_H
#define ROOM_H

#include "game.h"
#include <unistd.h>
#include <signal.h>

typedef struct single_room{
	char name[32];
	pid_t pid;
	int rqid;
	Player* player1;
	Player* player2;
} Room;

typedef struct room_node{
	struct room_node* next;
	Room* room;
} rNode;

typedef struct hist_log{
	struct hist_log* next;
	char log[80];
} hLog;

Room* addRoom(rNode**, char*);
Room* findRoom(rNode*, char*);
int deleteRoom(rNode**, Room*);
void listFreeRooms(rNode*, char*);
void addLog(hLog**, char*);

#endif
