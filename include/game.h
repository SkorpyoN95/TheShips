#ifndef GAME_H
#define GAME_H

#define RED_POLE 	"\x1B[31m\u25A0\x1B[0m"
#define GRN_POLE	"\x1B[32m\u25A0\x1B[0m"
#define YEL_POLE	"\x1B[33m\u25A0\x1B[0m"
#define BLU_POLE	"\x1B[34m\u25A0\x1B[0m"
#define BLK_POLE	"\u25A0"
#define RESET		"\x1B[0m"

typedef enum field_state {SEA = 1, SHIP, DAMAGED, WRECK} fldState;

typedef struct{
	char name[32];
	int sock_id;
	fldState board[10][10];
	int ships[20];
} Player;

void putShipsOnMap(Player*);
int addShip(Player*, int, int, char*, char*);
int removeShip(Player*, int, int);
void randomizeBoard(Player*);
void resetBoard(Player*);
void showBoard(fldState[10][10]);
int countPlayerHP(Player*);
void packBoard(Player*,char*, char*);
void unpackBoard(char*, char*, Player*);
int isShipSinked(Player*, int);

#endif
