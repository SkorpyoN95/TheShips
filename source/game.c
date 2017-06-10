#include "game.h"
#include <string.h>

void putShipsOnMap(Player* pl){
	int accept = 0;
	char input[16];
	memset(input, 0, 16);
	char* parse, * support;
	printf(	"Let's set your board!\n"
			"To add a ship to the board: [1 - 10 #masts descending] [coord begin] [coord end if #masts > 1]\n"
			"To remove a ship from the board: [-1 - -10 #masts descending]\n"
			"To accept: [0]\n");
	while(!accept){
		
	}
}

void addShip(Player* pl, int masts, int number, char* begin, char* end){
	begin[0] = tolower(begin[0]); end[0] = tolower(end[0]);
	if(begin[0] < 'a' || begin[0] > 'j' || end[0] < 'a' || end[0] > 'j'){
		printf("Invalid characters\n");
		return;
	}
	int first = atoi(begin[1]), last = atoi(end[1]);
	if(begin[0] != end[0] && first != last){
		printf("That's not a ship-shape\n");
		return;
	}
	first += (begin[0] - 'a')*10; last += (end[0] - 'a')*10;
	if(last-first != masts-1 && last-first != (masts-1)*10){
		printf("Ships' length doesn't match the range\n");
		return;
	}
}
