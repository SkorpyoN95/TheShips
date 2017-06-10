#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "serverManager.h"
#include "room.h"

int main(int argc, char **argv)
{
	Player me, enemy;
	
	memset(me.board, SEA, 100);
	memset(enemy.board, -1, 100);
	
	char ip[16];
	char buff[256];
	char hostname[32];
	char option;
	int fd;
	
	memset(ip, 0, 16);
	memset(buff, 0, 256);
	printf("Put host IP: ");
	scanf("%s", (char*)ip);
	if((fd = joinServer(ip)) == -1) 	printf("Joining server %s failed.\n", ip);
	else								printf("Joined server, IP: %s\n", ip);
	
	printf("Who are you?\n");
	scanf("%s", (char*)me.name);
	
	do{
		printf("Wanna (c)reate new room or (j)oin to one? ");
		scanf("\n");
		option = getchar();
		
		printf("Enter host's name:\n");
		scanf("%s", (char*)hostname);
		
		if(sendRequest(fd, option, 2, hostname, me.name)){
			printf("Nothing has been sent.\n");
			exit(1);
		}
	}while(option == LIST);
	
	printf("Wait untill everybody is connected...\n");
	if(recv(fd, buff, 256, 0) == -1){
		perror("recieve failed");
		exit(1);
	}
	else
		printf("Gotcha!\n");
	
	if(buff[0] != START){
		printf("Unexpected request type, program shuts down.\n");
		exit(1);
	}
	else
		printf("Lets play the game\n");
	
	
	putShipsOnMap(&me);
	
	//while(1){
	//}
	
	close(fd);
	
	return 0;
}

