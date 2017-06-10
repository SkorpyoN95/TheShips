#include "serverManager.h"
#include "room.h"
#include "game.h"

int main(int argc, char **argv)
{
	char buff[256];
	char ip[16];
	int fd, cld;
	rNode* room_list = NULL;
	Room* act_room;
	char* command_ptr, * support_ptr;
	key_t server_key = ftok("./", 'P');
	if((s_q_id = msgget(server_key, 0666 | IPC_CREAT)) == -1){
		perror("Creating server msg queue");
		exit(1);
	}
	
	if((fd = launchServer(ip)) == -1) exit(1);
	printf("Server launched. IP: %s\n", ip);
	while(1){
		memset(buff, 0, 128);
		support_ptr = NULL;
		if((cld = accept(fd, NULL, NULL)) == -1){
			perror("accept failed");
			continue;
		}
		if(recv(cld, buff, 256, 0) == -1){
			perror("recieve failed");
			continue;
		}
		else{
			switch(buff[0]){
				
				case CREATE: 	command_ptr = strtok_r(buff+1, ":", &support_ptr); 
								act_room = addRoom(&room_list, command_ptr);
								if(act_room){
									strncpy(act_room->player1->name, support_ptr, 32);
									act_room->player1->sock_id = cld;
									printf("New room created. Name: %s\n", command_ptr);
								}
								break;
								
				case JOIN: 		command_ptr = strtok_r(buff+1, ":", &support_ptr);
								act_room = findRoom(room_list, command_ptr);
								if(act_room){
									act_room->player2 = (Player*)malloc(sizeof(Player));
									strncpy(act_room->player2->name, support_ptr, 32);
									act_room->player2->sock_id = cld;
									printf("You joined to room: %s\n", act_room->name);
									launchRoom(&room_list, act_room);
								}
								else
									printf("Room not found\n");
								break;
								
				case LIST:		listFreeRooms(room_list);
								break;
								
				default: 		printf("I got unkown request: %s\n", buff);
			}
		}
	}
	
	close(fd);
	msgctl(s_q_id, IPC_RMID, NULL);
	
	return 0;
}

