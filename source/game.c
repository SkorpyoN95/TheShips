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
	int first, last, step;
	begin[0] = tolower(begin[0]); end[0] = tolower(end[0]);
	if(begin[0] < 'a' || begin[0] > 'j' || end[0] < 'a' || end[0] > 'j'){
		printf("Invalid characters\n");
		return;
	}
	
	first = atoi(begin[1]) - 1; last = atoi(end[1]) - 1;
	if(first < 0 || first > 9 || last < 0 || last > 9){
		printf("Invalid characters\n");
		return;
	}
	
	if(begin[0] != end[0] && first != last){
		printf("That's not a ship-shape\n");
		return;
	}
	first += (begin[0] - 'a')*10; last += (end[0] - 'a')*10;
	
	step = masts != 1 ? (last - first) / (masts - 1) : 1;
	if(last-first != masts-1 && last-first != (masts-1)*10){
		printf("Ships' length doesn't match the range\n");
		return;
	}
	
	for(int i = first; i <= last; i += step){
		if(*(pl->board + i) != SEA){
			printf("This place is occupied\n");
			return;
		}
		if(step == 1){
			if(i > 9 && *(pl->board + i - 10) != SEA){
				printf("Collision detected\n");
				return;
			}
			if(i < 90 && *(pl->board + i + 10) != SEA){
				printf("Collision detected\n");
				return;
			}
		}
		if(step == 10){
			if(i%10 > 0 && *(pl->board + i - 1) != SEA){
				printf("Collision detected\n");
				return;
			}
			if(i%10 < 9 && *(pl->board + i + 1) != SEA){
				printf("Collision detected\n");
				return;
			}
		}
		if(i == first){
			if(step == 1 && i%10 > 0){
				if(i > 10 && *(pl->board + i - 11) != SEA){
					printf("Collision detected\n");
					return;
				}
				if(i > 0 && *(pl->board + i - 1) != SEA){
					printf("Collision detected\n");
					return;
				}
				if(i < 90 && *(pl->board + i + 9) != SEA){
					printf("Collision detected\n");
					return;
				}
			}
			if(step == 10 && i > 9){
				if(i%10 > 0 && *(pl->board + i - 11) != SEA){
					printf("Collision detected\n");
					return;
				}
				if(*(pl->board + i - 10) != SEA){
					printf("Collision detected\n");
					return;
				}
				if(i%10 < 9 && *(pl->board + i - 9) != SEA){
					printf("Collision detected\n");
					return;
				}
			}
		}
		if(i == last){
			if(step == 1 && i%10 < 9){
				if(i > 9 && *(pl->board + i - 9) != SEA){
					printf("Collision detected\n");
					return;
				}
				if(i < 99 && *(pl->board + i + 1) != SEA){
					printf("Collision detected\n");
					return;
				}
				if(i < 90 && *(pl->board + i + 11) != SEA){
					printf("Collision detected\n");
					return;
				}
			}
			if(step == 10 && i < 90){
				if(i%10 > 0 && *(pl->board + i + 9) != SEA){
					printf("Collision detected\n");
					return;
				}
				if(*(pl->board + i + 10) != SEA){
					printf("Collision detected\n");git 
					return;
				}
				if(i%10 < 9 && *(pl->board + i + 11) != SEA){
					printf("Collision detected\n");
					return;
				}
			}
		}
	}//validation ends here
}
