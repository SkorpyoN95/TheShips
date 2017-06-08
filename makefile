SRC =\
	source/serverManager.c \
	source/room.c \
	source/game.c

all: server client
	
	
server:
	gcc $(FLAGS) -Wall server.c $(SRC) -o server -Iinclude/
	
client:
	gcc $(FLAGS) -Wall client.c $(SRC) -o client -Iinclude/
	
clean:
	rm -f server client
