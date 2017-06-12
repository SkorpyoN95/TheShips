#include "game.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

void putShipsOnMap(Player* pl){
	int accept = 0;
	char input[16];
	memset(input, 0, 16);
	char begin[4], end[4];
	memset(begin, 0, 4);
	memset(end, 0, 4);
	int option = 0;
	printf(	"Let's set your board!\n"
			"To add a ship to the board: [1 - 10 #masts descending] [coord begin] [coord end even if #masts = 1]\n"
			"To remove a ship from the board: [-1 - -10 #masts descending]\n"
			"To accept: [0]\n");
	scanf("\n");
	while(!accept){
		fgets(input, 16, stdin);
		sscanf(input, "%d %s %s\n", &option, begin, end);
		if(strlen(begin) > 3 || strlen(end) > 3){
			printf("Invalid input\n");
			continue;
		}
		switch(option){
			case 0: if(countPlayerHP(pl) == 20){
						accept = 1; break;
					}
					else{
						printf("You didn't assigned all ships yet.\n"); break;
					}
			case 1: addShip(pl, 4, 0, begin, end); break;
			case 2: addShip(pl, 3, 4, begin, end); break;
			case 3: addShip(pl, 3, 7, begin, end); break;
			case 4: addShip(pl, 2, 10, begin, end); break;
			case 5: addShip(pl, 2, 12, begin, end); break;
			case 6: addShip(pl, 2, 14, begin, end); break;
			case 7: addShip(pl, 1, 16, begin, end); break;
			case 8: addShip(pl, 1, 17, begin, end); break;
			case 9: addShip(pl, 1, 18, begin, end); break;
			case 10: addShip(pl, 1, 19, begin, end); break;
			case -1: removeShip(pl, 4, 0); break;
			case -2: removeShip(pl, 3, 4); break;
			case -3: removeShip(pl, 3, 7); break;
			case -4: removeShip(pl, 2, 10); break;
			case -5: removeShip(pl, 2, 12); break;
			case -6: removeShip(pl, 2, 14); break;
			case -7: removeShip(pl, 1, 16); break;
			case -8: removeShip(pl, 1, 17); break;
			case -9: removeShip(pl, 1, 18); break;
			case -10: removeShip(pl, 1, 19); break;
			default: printf("Unrecognised command\n"); break;
		}
		showBoard(pl->board);
	}
}

void addShip(Player* pl, int masts, int number, char* begin, char* end){
	int x1, y1, x2, y2;
	x1 = tolower(begin[0]) - 'a'; y1 = atoi(begin+1) - 1;
	x2 = tolower(end[0]) - 'a'; y2 = atoi(end+1) - 1;
	
	if(x1 < 0 || x1 > 9 || x2 < 0 || x2 > 9 || y1 < 0 || y1 > 9 || y2 < 0 || y2 > 9){
		printf("Invalid coordinates\n");
		return;
	}
	
	if(x1 != x2 && y1 != y2){
		printf("Invalid ship's shape\n");
		return;
	}
	
	if(x1 > x2){
		int tmp = x1;
		x1 = x2;
		x2 = tmp;
	}
	
	if(y1 > y2){
		int tmp = y1;
		y1 = y2;
		y2 = tmp;
	}
	
	if((x2 - x1) != (masts-1) && (y2 - y1) != (masts-1)){
		printf("Invalid ship's length\n");
		return;
	}
	
	if(x1 == x2){
		for(int i = y1; i <= y2; ++i){
			if(pl->ships[number + i - y1] != -1){
				printf("Actually that ship is on the board\n"); return;
			}
			if(pl->board[x1][i] != SEA){
				printf("%c%d is occupied\n", x1 + 65, i + 1); return;
			}
			if(	(x1 > 0 && pl->board[x1 - 1][i] != SEA) ||
				(x1 < 9 && pl->board[x1 + 1][i] != SEA)){
					printf("Collision detected\n"); return;
			}
		}
		if(		(y1 > 0 && ((x1 > 0 && pl->board[x1 - 1][y1 - 1] != SEA) || (pl->board[x1][y1 - 1] != SEA) || (x1 < 9 && pl->board[x1 + 1][y1 - 1] != SEA)))
			||	(y2 < 9 && ((x1 > 0 && pl->board[x1 - 1][y2 + 1] != SEA) || (pl->board[x1][y2 + 1] != SEA) || (x1 < 9 && pl->board[x1 + 1][y2 + 1] != SEA)))){
			printf("Collision detected\n"); return;
		}
	}else{
		for(int i = x1; i <= x2; ++i){
			if(pl->ships[number + i - x1] != -1){
				printf("Actually that ship is on board\n"); return;
			}
			if(pl->board[i][y1] != SEA){
				printf("%c%d is occupied\n", i + 65, y1 + 1); return;
			}
			if(	(y1 > 0 && pl->board[i][y1 - 1] != SEA) ||
				(y1 < 9 && pl->board[i][y1 + 1] != SEA)){
					printf("Collision detected\n"); return;
			}
		}
		if(		(x1 > 0 && ((y1 > 0 && pl->board[x1 - 1][y1 - 1] != SEA) || (pl->board[x1 - 1][y1] != SEA) || (y1 < 9 && pl->board[x1 - 1][y1 + 1] != SEA)))
			||	(x2 < 9 && ((y1 > 0 && pl->board[x2 + 1][y1 - 1] != SEA) || (pl->board[x2 - 1][y1] != SEA) || (y1 < 9 && pl->board[x1 + 1][y1 + 1] != SEA)))){
			printf("Collision detected\n"); return;
		}
	}
	
	if(x1 == x2)
		for(int i = y1; i <= y2; ++i){
			pl->board[x1][i] = SHIP;
			pl->ships[number + i - y1] = 10*x1 + i;
		}
	else
		for(int i = x1; i <= x2; ++i){
			pl->board[i][y1] = SHIP;
			pl->ships[number + i - x1] = 10*i + y1;
		}
	return;
}

void removeShip(Player* pl, int masts, int number){
	for(int i = 0; i < masts; ++i)
		if(pl->ships[number+i] == -1){
			printf("There is no ship there\n");
			return;
		}
			
	for(int i = 0; i < masts; ++i){
		pl->board[pl->ships[number+i]/10][pl->ships[number+i]%10] = SEA;
		pl->ships[number+i] = -1;
	}
		
	return;
}

void showBoard(fldState brd[10][10]){
	for(int i = 0; i < 10; i++){
		for(int j = 0; j < 10; j++)
			switch(brd[i][j]){
				case SEA:		printf(BLU_POLE); break;
				case SHIP:		printf(GRN_POLE); break;
				case DAMAGED:	printf(YEL_POLE); break;
				case WRECK:		printf(RED_POLE); break;
				default:		printf(BLK_POLE); break;
			}
		printf("\n");
	}
	return;
}

int countPlayerHP(Player* pl){
	int counter = 0;
	for(int i = 0; i < 20; ++i)
		if(pl->ships[i] != -1)	counter++;
		
	return counter;
}

void packBoard(Player* pl, char* where){
	for(int i = 0; i < 10; ++i)
		for(int j = 0; j < 10; ++j)
			where[10*i + j] = (char)(pl->board[i][j]);
			
	where[100] = '\0';
	printf("%s\n", where);
	return;
}

void unpackBoard(char* what, Player* pl){
	for(int i = 0; i < 10; ++i)
		for(int j = 0; j < 10; ++j)
			pl->board[i][j] = (fldState)what[10*i + j];
			
	return;
}
