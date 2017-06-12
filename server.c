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
		printf("yet another loop\n");
		memset(buff, 0, 256);
		support_ptr = NULL;
		if((cld = accept(fd, NULL, NULL)) == -1){
			perror("accept failed");
			continue;
		}else 	printf("client accepted\n");
		
		if(recv(cld, buff, 256, 0) == -1){
			perror("recieve failed");
			continue;
		}
		else{
			printf("recieved something\n");
			switch(buff[0]){
				
				case CREATE: 	command_ptr = strtok_r(buff+1, ":", &support_ptr); 
								act_room = addRoom(&room_list, command_ptr);
								if(act_room){
									strncpy(act_room->player1->name, support_ptr, 32);
									act_room->player1->sock_id = cld;
									dprintf(cld, "New room created. Name: %s\n", command_ptr);
								}
								break;
								
				case JOIN: 		command_ptr = strtok_r(buff+1, ":", &support_ptr);
								act_room = findRoom(room_list, command_ptr);
								if(act_room){
									if(act_room->player2){
										dprintf(cld, "This room is busy\n");
										break;
									}
									act_room->player2 = (Player*)malloc(sizeof(Player));
									strncpy(act_room->player2->name, support_ptr, 32);
									act_room->player2->sock_id = cld;
									dprintf(cld, "You joined to room: %s\n", act_room->name);
									launchRoom(fd, &room_list, act_room);
									printf("room launched\n");
								}
								else
									dprintf(cld, "Room not found\n");
									
								break;
								
				case LIST:		memset(buff, 0, 256);
								listFreeRooms(room_list, buff);
								dprintf(cld, "%s", buff);
								break;
								
				case FILL_INFO:	printf("%s\n", buff+1);
								command_ptr = strtok_r(buff+1, ":", &support_ptr); 
								act_room = findRoom(room_list, command_ptr);
								if(act_room){
									BaseMessage bsm;
									bsm.mtype = 1;
									memset(bsm.mtext, 0, 1024);
									strcpy(bsm.mtext, support_ptr);
									msgsnd(act_room->rqid, &bsm, 1024 + sizeof(int), 0);
									dprintf(cld, "Msg passed");
								}else
									dprintf(cld, "Room not found");
								break;
								
				default: 		printf("I got unkown request: %s\n", buff);
			}
		}
		printf("loop finishes here\n");
	}
	
	close(fd);
	msgctl(s_q_id, IPC_RMID, NULL);
	
	return 0;
}

