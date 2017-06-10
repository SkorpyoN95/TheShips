#ifndef ROOM_H
#define ROOM_H

#include "game.h"
#include <unistd.h>

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

Room* addRoom(rNode**, char*);
Room* findRoom(rNode*, char*);
int deleteRoom(rNode**, Room*);
void listFreeRooms(rNode*);

#endif
