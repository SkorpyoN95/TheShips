SRC =\
	source/serverManager.c \
	source/room.c \
	source/game.c
	
LIBS =\
	serverManager.o \
	room.o \
	game.o

all: server client
	
	
server: libs
	gcc $(FLAGS) -Wall server.c $(LIBS) -o server -Iinclude/
	
client: libs
	gcc $(FLAGS) -Wall client.c $(LIBS) -o client -Iinclude/
	
libs:
	gcc $(FLAGS) -Wall -c $(SRC) -Iinclude/
	
clean:
	rm -f server client *.o
