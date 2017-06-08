#ifndef GAME_H
#define GAME_H

typedef enum field_state {SEA, SHIP, DAMAGED, WRECK} fState;

typedef struct player{
	char name[32];
} Player;

#endif
