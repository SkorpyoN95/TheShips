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
	memset(pl->board, SEA, 100*sizeof(fldState));
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
			case 0: accept = 1; break;
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
			default: printf("Unrecognised command\n"); break;
		}
		showBoard(*(pl->board));
	}
}

void addShip(Player* pl, int masts, int number, char* begin, char* end){
	int first, last, step;
	begin[0] = tolower(begin[0]); end[0] = tolower(end[0]);
	if(begin[0] < 'a' || begin[0] > 'j' || end[0] < 'a' || end[0] > 'j'){
		printf("Invalid characters\n");
		return;
	}
	
	first = atoi(begin + 1) - 1; last = atoi(end + 1) - 1;
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
		if(*(*(pl->board + i)) != SEA){
			printf("This place is occupied\n");
			return;
		}
		if(step == 1){
			if(i > 9 && *(*(pl->board + i - 10)) != SEA){
				printf("Collision detected\n");
				return;
			}
			if(i < 90 && *(*(pl->board + i + 10)) != SEA){
				printf("Collision detected\n");
				return;
			}
		}
		if(step == 10){
			if(i%10 > 0 && *(*(pl->board + i - 1)) != SEA){
				printf("Collision detected\n");
				return;
			}
			if(i%10 < 9 && *(*(pl->board + i + 1)) != SEA){
				printf("Collision detected\n");
				return;
			}
		}
		if(i == first){
			if(step == 1 && i%10 > 0){
				if(i > 10 && *(*(pl->board + i - 11)) != SEA){
					printf("Collision detected\n");
					return;
				}
				if(i > 0 && *(*(pl->board + i - 1)) != SEA){
					printf("Collision detected\n");
					return;
				}
				if(i < 90 && *(*(pl->board + i + 9)) != SEA){
					printf("Collision detected\n");
					return;
				}
			}
			if(step == 10 && i > 9){
				if(i%10 > 0 && *(*(pl->board + i - 11)) != SEA){
					printf("Collision detected\n");
					return;
				}
				if(*(*(pl->board + i - 10)) != SEA){
					printf("Collision detected\n");
					return;
				}
				if(i%10 < 9 && *(*(pl->board + i - 9)) != SEA){
					printf("Collision detected\n");
					return;
				}
			}
		}
		if(i == last){
			if(step == 1 && i%10 < 9){
				if(i > 9 && *(*(pl->board + i - 9)) != SEA){
					printf("Collision detected\n");
					return;
				}
				if(i < 99 && **(pl->board + i + 1) != SEA){
					printf("Collision detected\n");
					return;
				}
				if(i < 90 && **(pl->board + i + 11) != SEA){
					printf("Collision detected\n");
					return;
				}
			}
			if(step == 10 && i < 90){
				if(i%10 > 0 && **(pl->board + i + 9) != SEA){
					printf("Collision detected\n");
					return;
				}
				if(**(pl->board + i + 10) != SEA){
					printf("Collision detected\n");
					return;
				}
				if(i%10 < 9 && **(pl->board + i + 11) != SEA){
					printf("Collision detected\n");
					return;
				}
			}
		}
	}//validation ends here
	
	for(int i = first; i <= last; i += step){
		*(*(pl->board) + i) = SHIP;
		pl->ships.all_together[number] = i;
		number++;
	}
		
	return;
}

void showBoard(fldState* brd){
	for(int i = 0; i < 10; i++){
		for(int j = 0; j < 10; j++)
			switch(brd[10*i + j]){
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
