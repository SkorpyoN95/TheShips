#include "room.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


Room* addRoom(rNode** head, char* name){
	rNode* tmp = *head;
	while(tmp)	tmp = tmp->next;
	
	if((tmp = (rNode*)malloc(sizeof(rNode))) == NULL)
		return NULL;
		
	tmp->next = NULL;
	if((tmp->room = (Room*)malloc(sizeof(Room))) == NULL){
		free(tmp);
		return NULL;
	}
	
	strncpy(tmp->room->name, name, 32);
	if((tmp->room->player1 = (Player*)malloc(sizeof(Player))) == NULL){
		free(tmp->room);
		free(tmp);
		return NULL;
	}
	tmp->room->player2 = NULL;
	
	if(!*head) *head = tmp;
	return tmp->room;
}

int deleteRoom(rNode** head, Room* room){
	if(head == NULL) return -1;
	rNode* tmp = *head;
	if((*head)->room == room){
		*head = (*head)->next;
		free(tmp->room->player1);
		free(tmp->room->player2);
		free(tmp->room);
		free(tmp);
		*head = NULL;
		return 0;
	}
	while(tmp->next && tmp->next->room != room)	tmp = tmp->next;
	if(!tmp->next) return -1;
	free(tmp->next->room->player1);
	free(tmp->next->room->player2);
	free(tmp->next->room);
	free(tmp->next);
	tmp->next = NULL;
	return 0;
}

Room* findRoom(rNode* head, char* name){
	while(head && strcmp(name, head->room->name))	head = head->next;
	if(!head)	return NULL;
	return head->room;
}

void listFreeRooms(rNode* head, char* buff){
	while(head){
		if(!head->room->player2)
			snprintf(buff + strlen(buff), 256-strlen(buff), "%s\t%s\n",head->room->name, head->room->player1->name);
		head = head->next;
	}
	if(!strlen(buff)){
		snprintf(buff, 256, "There is no room available\n");
		return;
	}
}

void addLog(hLog** head, char* log){
	hLog* tmp = *head;
	while(tmp)	tmp = tmp->next;
	tmp = (hLog*)malloc(sizeof(hLog));
	strncpy(tmp->log, log, 80);
	tmp->next = NULL;
	if(*head == NULL)	*head = tmp;
	return;
}
