#include "room.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void addRoom(rNode** head, char* name){
	rNode* tmp = *head;
	while(tmp)	tmp = tmp->next;
	tmp = (rNode*)malloc(sizeof(rNode));
	tmp->next = NULL;
	tmp->room = (Room*)malloc(sizeof(Room));
	tmp->room->name = name;
	tmp->room->player1 = (Player*)malloc(sizeof(Player));
	tmp->room->player2 = (Player*)malloc(sizeof(Player));
	if(!*head) *head = tmp;
	return;
}

int deleteRoom(rNode** head, Room* room){
	if(head == NULL) return -1;
	rNode* tmp = *head;
	if((*head)->room == room){
		*head = (*head)->next;
		free(tmp);
		return 0;
	}
	while(tmp->next && tmp->next->room != room)	tmp = tmp->next;
	if(!tmp->next) return -1;
	free(tmp->next);
	tmp->next = NULL;
	return 0;
}

Room* findRoom(rNode* head, char* name){
	while(head && strcmp(name, head->room->name))	head = head->next;
	if(!head)	return NULL;
	return head->room;
}

void listFreeRooms(rNode* head){
	while(head){
		printf("%s\t%s\n", head->room->name, head->room->player1->name);
		head = head->next;
	}
}
