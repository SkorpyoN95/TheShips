#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
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
	
	if(buff[0] != START){
		printf("Unexpected request type, program shuts down.\n");
		exit(1);
	}
	else
		system("clear");
	
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
	
	if(buff[0] != START){
		printf("Unexpected request type, program shuts down.\n");
		exit(1);
	}
		
	int run = 1;
	
	while(run){
		int x, y;
		memset(buff, 0, 256);
		system("clear");
		showBoard(me.board);
		printf("\n");
		showBoard(enemy.board);
		if(recv(sock_server, buff, 256, 0) == -1){
			perror("recieve failed");
			exit(1);
		}
		
		switch(buff[0]){
			case TURN: 		break;
			case GAME_OVER:	run = 0; printf("%s won! Congratulations!\n", buff+1); continue;
			case RESULT:	switch(buff[1]){
								case SEA:	printf("Oponnent missed!\n");
											sscanf(buff + 3, "%d:%d", &x, &y);
											me.board[x][y] = SEA;
											continue;
								case SHIP:	printf("Opponent hit you!\n");
											sscanf(buff + 3, "%d:%d", &x, &y);
											me.board[x][y] = DAMAGED;
											continue;
								case WRECK:	printf("Opoonent hit you and sank you!\n");
											for(int i = 0; i < buff[2] - '0'; ++i){
												sscanf(buff + 4 + 4*i, "%d:%d", &x, &y);
												enemy.board[x][y] = WRECK;
											}
											continue;
								default:	printf("Opponent just wasted his move\n"); continue;
							}
			default:		continue;
		}
		
		char coords[] = {0,0,0,0};
		do{
			system("clear");
			showBoard(me.board);
			printf("\n");
			showBoard(enemy.board);
			printf("Put valid coordinates you want to attack:\n");
			scanf("%s", coords);
		}while(tolower(coords[0]) < 'a' || tolower(coords[0]) > 'j' || atoi(coords+1) < 1 || atoi(coords+1) > 10 || enemy.board[tolower(coords[0]) - 'a'][atoi(coords+1) - 1] != 0);
		
		if(sendRequest(sock_server, MOVE, 2, hostname, coords) == -1){
			printf("Nothing has been sent.\n");
			exit(1);
		}
		
		memset(buff, 0, 256);
		if(recv(sock_server, buff, 256, 0) == -1){
			perror("recieve failed");
			exit(1);
		}
		
		if(buff[0] != RESULT){
			printf("Unexpected request type.\n");
			exit(1);
		}
		switch(buff[1]){
			case SEA:	printf("Miss!\n");
						sscanf(buff + 3, "%d:%d", &x, &y);
						enemy.board[x][y] = SEA;
						break;
			case SHIP:	printf("Hit!\n");
						sscanf(buff + 3, "%d:%d", &x, &y);
						enemy.board[x][y] = DAMAGED;
						break;
			case WRECK:	printf("Hit and sink!\n");
						for(int i = 0; i < buff[2] - '0'; ++i){
							sscanf(buff + 4 + 4*i, "%d:%d", &x, &y);
							enemy.board[x][y] = WRECK;
						}
						break;
			default:	printf("You just wasted your move\n"); break;
		}
	}
	
	close(sock_server);
	
	return 0;
}

