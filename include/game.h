#ifndef GAME_H
#define GAME_H

typedef enum field_state {SEA, SHIP, DAMAGED, WRECK} fldState;

typedef union all_ships{
	fldState ship4mast[1][4];
	fldState ship3mast[2][3];
	fldState ship2mast[3][2];
	fldState ship4mast[4][1];
	fldState all_together[20];
} Fleet;

typedef struct player{
	char name[32];
	fldState board[10][10];
	Fleet ships;
} Player;

void putShipsOnMap(Player*);
void addShip(Player*, int, int, char*, char*);
void removeShip(Player*, int, int);

#endif
