#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
//#include <gtk/gtk.h>
#include "serverManager.h"
#include "room.h"

int main(int argc, char **argv)
{
	//fState my_board[10][10];
	//fState enemy_board[10][10];
	
	//memset(my_board, SEA, 100);
	//memset(enemy_board, -1, 100);
	
	char ip[16];
	char msg[256];
	char hostname[32], nickname[32];
	int fd;
	
	memset(ip, 0, 16);
	memset(msg, 0, 256);
	printf("Put host IP: ");
	scanf("%s", (char*)ip);
	if((fd = joinServer(ip)) == -1) 	printf("Joining server %s failed.\n", ip);
	else								printf("Joined server, IP: %s\n", ip);
	
	printf("Who are you?\n");
	scanf("%s", (char*)nickname);
	
	printf("Wanna (c)reate new room or (j)oin to one? ");
	scanf("\n");
	char option = getchar();
	
	printf("Enter host's name:\n");
	scanf("%s", (char*)hostname);
	
	/*switch(option){
		case 'c': 	sprintf(msg, "%c%s:%s", option, hostname, nickname);
					if(write(fd, msg, strlen(msg)) == -1) perror("writing to host"); break;
		case 'j': 	if(write(fd, msg, strlen(msg)) == -1) perror("writing to host"); break;
		default: break;
	}*/
	sprintf(msg, "%c%s:%s", option, hostname, nickname);
	if(write(fd, msg, strlen(msg)) == -1) perror("writing to host");
	
	close(fd);
	
	return 0;
}

