#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "serverManager.h"
#include "room.h"

int main(int argc, char **argv)
{
	Player me, enemy;
	
	for(int i = 0; i < 10; ++i)
		for(int j = 0; j < 10; ++j){
			me.board[i][j] = SEA;
			enemy.board[i][j] = 0;
		}
	memset(me.ships, -1, 20*sizeof(int));
	memset(enemy.ships, -1, 20*sizeof(int));
	
	char ip[16];
	char buff[256];
	char hostname[32];
	char option;
	int sock_server;
	
	memset(ip, 0, 16);
	memset(buff, 0, 256);
	printf("Put host IP:\n");
	scanf("%s", (char*)ip);
	getchar();
	if((sock_server = joinServer(ip)) == -1) 	printf("Joining server %s failed.\n", ip);
	else								printf("Joined server, IP: %s\n", ip);
	
	printf("Who are you?\n");
	scanf("%s", (char*)me.name);
	getchar();
	
	do{
		printf("Wanna (c)reate new room, (j)oin to one or (l)ist some?\n");
		scanf("\n\t\r%c", &option);
		getchar();
		
		if(option == LIST){
			if(sendRequest(sock_server, option, 0)){
				printf("Nothing has been sent.\n");
				exit(1);
			}
		}else{
			printf("Enter host's name:\n");
			scanf("%s", (char*)hostname);
			getchar();
			if(sendRequest(sock_server, option, 2, hostname, me.name)){
				printf("Nothing has been sent.\n");
				exit(1);
			}
		}

		if(recv(sock_server, buff, 256, 0) == -1){
			perror("recieve failed");
			exit(1);
		}
		printf("%s", buff);
		memset(buff, 0, 256);
	}while(option == LIST);
	
	printf("Wait untill everybody is connected...\n");
	if(recv(sock_server, buff, 256, 0) == -1){
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
	
	memset(buff, 0, 256);
	
	
	putShipsOnMap(&me);
	char packed_board[101];
	packBoard(&me, packed_board);
	if(sendRequest(sock_server, FILL_INFO, 3, hostname, me.name, packed_board) == -1){
		printf("Nothing has been sent.\n");
		exit(1);
	}
	printf("Waiting for your opponent...\n");
	if(recv(sock_server, buff, 256, 0) == -1){
		perror("recieve failed");
		exit(1);
	}
	else
		printf("Gotcha!\n");
		
	printf("%s", buff);
	memset(buff, 0, 256);
	
	//while(1){
	//}
	
	close(sock_server);
	
	return 0;
}

