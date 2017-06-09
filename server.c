#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
//#include <gtk/gtk.h>
#include "serverManager.h"
#include "room.h"
#include "game.h"

int main(int argc, char **argv)
{
	char buff[128];
	char ip[16];
	int fd, cld;
	rNode* room_list = NULL;
	Room* door;
	char* command_ptr, * support_ptr;
	
	fd = launchServer(ip);
	printf("Server launched. IP: %s\n", ip);
	while(1){
		memset(buff, 0, 128);
		support_ptr = NULL;
		if((cld = accept(fd, NULL, NULL)) == -1) perror("accept failed");
		if(recv(cld, buff, 128, 0) == -1) perror("recieve failed");
		else{
			switch(buff[0]){
				case 'c': 	command_ptr = strtok_r(buff+1, ":", &support_ptr); 
							addRoom(&room_list, command_ptr);
							strncpy(room_list->room->player1->name, support_ptr, 32);
							printf("New room created. Name: %s\n", command_ptr);
							break;
				case 'j': 	command_ptr = strtok_r(buff+1, ":", &support_ptr);
							door = findRoom(room_list, command_ptr);
							if(door)	printf("You joined to room: %s\n", door->name);
							else 		printf("Room not found\n");
							listFreeRooms(room_list);
							break;
				default: printf("I got unkown request: %s\n", buff);
			}
		}
	}
	
	close(fd);
	
	return 0;
}

