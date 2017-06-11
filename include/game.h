#ifndef GAME_H
#define GAME_H

#define RED_POLE 	"\x1B[31m\u25A0\x1B[0m"
#define GRN_POLE	"\x1B[32m\u25A0\x1B[0m"
#define YEL_POLE	"\x1B[33m\u25A0\x1B[0m"
#define BLU_POLE	"\x1B[34m\u25A0\x1B[0m"
#define BLK_POLE	"\u25A0"
#define RESET		"\x1B[0m"

typedef enum field_state {SEA, SHIP, DAMAGED, WRECK} fldState;

typedef union all_ships{
	fldState ship4mast[1][4];
	fldState ship3mast[2][3];
	fldState ship2mast[3][2];
	fldState ship1mast[4][1];
	fldState all_together[20];
} Fleet;

typedef struct{
	char name[32];
	int sock_id;
	fldState board[10][10];
	Fleet ships;
} Player;

void putShipsOnMap(Player*);
void addShip(Player*, int, int, char*, char*);
void removeShip(Player*, int, int);
void showBoard(fldState*);

#endif
