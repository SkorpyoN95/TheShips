#include "game.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

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
			"To get random board: [11]\n"
			"To reset board: [-11]\n"
			"To accept: [0]\n");
	scanf("\n");
	while(!accept){
		int result;
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
			case 1: result = addShip(pl, 4, 0, begin, end); break;
			case 2: result = addShip(pl, 3, 4, begin, end); break;
			case 3: result = addShip(pl, 3, 7, begin, end); break;
			case 4: result = addShip(pl, 2, 10, begin, end); break;
			case 5: result = addShip(pl, 2, 12, begin, end); break;
			case 6: result = addShip(pl, 2, 14, begin, end); break;
			case 7: result = addShip(pl, 1, 16, begin, end); break;
			case 8: result = addShip(pl, 1, 17, begin, end); break;
			case 9: result = addShip(pl, 1, 18, begin, end); break;
			case 10: result = addShip(pl, 1, 19, begin, end); break;
			case 11: result = 0; randomizeBoard(pl); break;
			case -1: result = removeShip(pl, 4, 0); break;
			case -2: result = removeShip(pl, 3, 4); break;
			case -3: result = removeShip(pl, 3, 7); break;
			case -4: result = removeShip(pl, 2, 10); break;
			case -5: result = removeShip(pl, 2, 12); break;
			case -6: result = removeShip(pl, 2, 14); break;
			case -7: result = removeShip(pl, 1, 16); break;
			case -8: result = removeShip(pl, 1, 17); break;
			case -9: result = removeShip(pl, 1, 18); break;
			case -10: result = removeShip(pl, 1, 19); break;
			case -11: result = 0; resetBoard(pl); break;
			default: printf("Unrecognised command\n"); break;
		}
		switch(result){
			case 0: printf("Done\n"); break;
			case 1: printf("Invalid coordinates\n"); break;
			case 2: printf("Invalid ship's shape\n"); break;
			case 3: printf("Invalid ship's length\n"); break;
			case 4: printf("Actually that ship is on the board\n"); break;
			case 5: printf("Area is occupied\n"); break;
			case 6: printf("Collision detected\n"); break;
			case -1: printf("There is no ship there\n"); break;
			default: printf("Dunno what happend\n");
		}
		showBoard(pl->board);
	}
}

int addShip(Player* pl, int masts, int number, char* begin, char* end){
	int x1, y1, x2, y2;
	x1 = tolower(begin[0]) - 'a'; y1 = atoi(begin+1) - 1;
	x2 = tolower(end[0]) - 'a'; y2 = atoi(end+1) - 1;
	
	if(x1 < 0 || x1 > 9 || x2 < 0 || x2 > 9 || y1 < 0 || y1 > 9 || y2 < 0 || y2 > 9) return 1;
	
	if(x1 != x2 && y1 != y2) return 2;
	
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
	
	if((x2 - x1) != (masts-1) && (y2 - y1) != (masts-1)) return 3;
	
	if(x1 == x2){
		for(int i = y1; i <= y2; ++i){
			if(pl->ships[number + i - y1] != -1) return 4;
			if(pl->board[x1][i] != SEA) return 5;
			if(	(x1 > 0 && pl->board[x1 - 1][i] != SEA) ||
				(x1 < 9 && pl->board[x1 + 1][i] != SEA))
				return 6;
		}
		if(		(y1 > 0 && ((x1 > 0 && pl->board[x1 - 1][y1 - 1] != SEA) || (pl->board[x1][y1 - 1] != SEA) || (x1 < 9 && pl->board[x1 + 1][y1 - 1] != SEA)))
			||	(y2 < 9 && ((x1 > 0 && pl->board[x1 - 1][y2 + 1] != SEA) || (pl->board[x1][y2 + 1] != SEA) || (x1 < 9 && pl->board[x1 + 1][y2 + 1] != SEA))))
			return 6;
	}else{
		for(int i = x1; i <= x2; ++i){
			if(pl->ships[number + i - x1] != -1) return 4;
			if(pl->board[i][y1] != SEA) return 5;
			if(	(y1 > 0 && pl->board[i][y1 - 1] != SEA) ||
				(y1 < 9 && pl->board[i][y1 + 1] != SEA))
				return 6;
		}
		if(		(x1 > 0 && ((y1 > 0 && pl->board[x1 - 1][y1 - 1] != SEA) || (pl->board[x1 - 1][y1] != SEA) || (y1 < 9 && pl->board[x1 - 1][y1 + 1] != SEA)))
			||	(x2 < 9 && ((y1 > 0 && pl->board[x2 + 1][y1 - 1] != SEA) || (pl->board[x2 - 1][y1] != SEA) || (y1 < 9 && pl->board[x1 + 1][y1 + 1] != SEA))))
			return 6;
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
	return 0;
}

int removeShip(Player* pl, int masts, int number){
	for(int i = 0; i < masts; ++i)
		if(pl->ships[number+i] == -1) return -1;
			
	for(int i = 0; i < masts; ++i){
		pl->board[pl->ships[number+i]/10][pl->ships[number+i]%10] = SEA;
		pl->ships[number+i] = -1;
	}
		
	return 0;
}

void randomizeBoard(Player* pl){
	srand(time(NULL));
	while(pl->ships[0] == -1){
		char begin[4] = {0,0,0,0}, end[4] = {0,0,0,0};
		int x, y;
		if(rand()%2){
			x = 'a' + rand()%10;
			y = 1 + rand()%7;
			snprintf(begin, 4, "%c%d", x, y);
			snprintf(end, 4, "%c%d", x, y+3);
		}else{
			x = 'a' + rand()%7;
			y = 1 + rand()%10;
			snprintf(begin, 4, "%c%d", x, y);
			snprintf(end, 4, "%c%d", x+3, y);
		}
		addShip(pl, 4, 0, begin, end);
	}
	while(pl->ships[4] == -1){
		char begin[4] = {0,0,0,0}, end[4] = {0,0,0,0};
		int x, y;
		if(rand()%2){
			x = 'a' + rand()%10;
			y = 1 + rand()%8;
			snprintf(begin, 4, "%c%d", x, y);
			snprintf(end, 4, "%c%d", x, y+2);
		}else{
			x = 'a' + rand()%8;
			y = 1 + rand()%10;
			snprintf(begin, 4, "%c%d", x, y);
			snprintf(end, 4, "%c%d", x+2, y);
		}
		addShip(pl, 3, 4, begin, end);
	}
	while(pl->ships[7] == -1){
		char begin[4] = {0,0,0,0}, end[4] = {0,0,0,0};
		int x, y;
		if(rand()%2){
			x = 'a' + rand()%10;
			y = 1 + rand()%8;
			snprintf(begin, 4, "%c%d", x, y);
			snprintf(end, 4, "%c%d", x, y+2);
		}else{
			x = 'a' + rand()%8;
			y = 1 + rand()%10;
			snprintf(begin, 4, "%c%d", x, y);
			snprintf(end, 4, "%c%d", x+2, y);
		}
		addShip(pl, 3, 7, begin, end);
	}
	while(pl->ships[10] == -1){
		char begin[4] = {0,0,0,0}, end[4] = {0,0,0,0};
		int x, y;
		if(rand()%2){
			x = 'a' + rand()%10;
			y = 1 + rand()%9;
			snprintf(begin, 4, "%c%d", x, y);
			snprintf(end, 4, "%c%d", x, y+1);
		}else{
			x = 'a' + rand()%9;
			y = 1 + rand()%10;
			snprintf(begin, 4, "%c%d", x, y);
			snprintf(end, 4, "%c%d", x+1, y);
		}
		addShip(pl, 2, 10, begin, end);
	}
	while(pl->ships[12] == -1){
		char begin[4] = {0,0,0,0}, end[4] = {0,0,0,0};
		int x, y;
		if(rand()%2){
			x = 'a' + rand()%10;
			y = 1 + rand()%9;
			snprintf(begin, 4, "%c%d", x, y);
			snprintf(end, 4, "%c%d", x, y+1);
		}else{
			x = 'a' + rand()%9;
			y = 1 + rand()%10;
			snprintf(begin, 4, "%c%d", x, y);
			snprintf(end, 4, "%c%d", x+1, y);
		}
		addShip(pl, 2, 12, begin, end);
	}
	while(pl->ships[14] == -1){
		char begin[4] = {0,0,0,0}, end[4] = {0,0,0,0};
		int x, y;
		if(rand()%2){
			x = 'a' + rand()%10;
			y = 1 + rand()%9;
			snprintf(begin, 4, "%c%d", x, y);
			snprintf(end, 4, "%c%d", x, y+1);
		}else{
			x = 'a' + rand()%9;
			y = 1 + rand()%10;
			snprintf(begin, 4, "%c%d", x, y);
			snprintf(end, 4, "%c%d", x+1, y);
		}
		addShip(pl, 2, 14, begin, end);
	}
	while(pl->ships[16] == -1){
		char begin[4] = {0,0,0,0};
		int x, y;
		x = 'a' + rand()%10;
		y = 1 + rand()%10;
		snprintf(begin, 4, "%c%d", x, y);
		addShip(pl, 1, 16, begin, begin);
	}
	while(pl->ships[17] == -1){
		char begin[4] = {0,0,0,0};
		int x, y;
		x = 'a' + rand()%10;
		y = 1 + rand()%10;
		snprintf(begin, 4, "%c%d", x, y);
		addShip(pl, 1, 17, begin, begin);
	}
	while(pl->ships[18] == -1){
		char begin[4] = {0,0,0,0};
		int x, y;
		x = 'a' + rand()%10;
		y = 1 + rand()%10;
		snprintf(begin, 4, "%c%d", x, y);
		addShip(pl, 1, 18, begin, begin);
	}
	while(pl->ships[19] == -1){
		char begin[4] = {0,0,0,0};
		int x, y;
		x = 'a' + rand()%10;
		y = 1 + rand()%10;
		snprintf(begin, 4, "%c%d", x, y);
		addShip(pl, 1, 19, begin, begin);
	}
	return;
}

void resetBoard(Player* pl){
	for(int i = 0; i < 10; ++i)
		for(int j = 0; j < 10; ++j)
			pl->board[i][j] = SEA;
			
	memset(pl->ships, -1, 20 * sizeof(int));
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

void show2Boards(fldState brd1[10][10], fldState brd2[10][10]){
	for(int i = 0; i < 10; i++){
		for(int j = 0; j < 10; j++)
			switch(brd1[i][j]){
				case SEA:		printf(BLU_POLE); break;
				case SHIP:		printf(GRN_POLE); break;
				case DAMAGED:	printf(YEL_POLE); break;
				case WRECK:		printf(RED_POLE); break;
				default:		printf(BLK_POLE); break;
			}
		printf("\t\t");
		for(int j = 0; j < 10; j++)
			switch(brd2[i][j]){
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
		if(pl->board[pl->ships[i]/10][pl->ships[i]%10] == SHIP)	counter++;
		
	return counter;
}

void packBoard(Player* pl, char* where, char* ships){
	for(int i = 0; i < 10; ++i)
		for(int j = 0; j < 10; ++j)
			where[10*i + j] = (char)(pl->board[i][j]);
			
	where[100] = '\0';
	for(int i = 0; i < 20; ++i)
		ships[i] = pl->ships[i];
	ships[20] = '\0';
	return;
}

void unpackBoard(char* what, char* ships, Player* pl){
	for(int i = 0; i < 10; ++i)
		for(int j = 0; j < 10; ++j)
			pl->board[i][j] = (fldState)what[10*i + j];
			
	for(int i = 0; i < 20; ++i)
		pl->ships[i] = ships[i];
	return;
}

int isShipSinked(Player* pl, int n){
	int offset = 0, masts = 0, where = 0;
	switch(n){
		case 1: offset = 0; masts = 4; break;
		case 2: offset = 4; masts = 3; break;
		case 3: offset = 7; masts = 3; break;
		case 4: offset = 10; masts = 2; break;
		case 5: offset = 12; masts = 2; break;
		case 6: offset = 14; masts = 2; break;
		case 7: offset = 16; masts = 1; break;
		case 8: offset = 17; masts = 1; break;
		case 9: offset = 18; masts = 1; break;
		case 10: offset = 19; masts = 1; break;
		default: return 0;
	}
	for(int i = offset, end = offset + masts; i < end; ++i){
		where = pl->ships[i];
		if(pl->board[where/10][where%10] != DAMAGED) return 0;
	}
	return 1;
}
