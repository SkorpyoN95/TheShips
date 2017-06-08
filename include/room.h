#ifndef ROOM_H
#define ROOM_H

#include "game.h"

typedef struct single_room{
	char* name;
	int socket;
	Player* player1;
	Player* player2;
} Room;

typedef struct room_node{
	struct room_node* next;
	Room* room;
} rNode;

void addRoom(rNode**, char*);
Room* findRoom(rNode*, char*);
int deleteRoom(rNode**, Room*);
void listFreeRooms(rNode*);

#endif
