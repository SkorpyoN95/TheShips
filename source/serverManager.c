#include "serverManager.h"

int launchServer(char* ip){
	int room_id, addr_id;
	struct sockaddr_in room, room_addr;
	memset((char*)&room, 0, sizeof(struct sockaddr_in));
	room.sin_family = AF_INET;
	room.sin_port = htons(3456);
	room.sin_addr.s_addr = htonl(INADDR_ANY);
	
	if((room_id = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		perror("Launching a room: socket()");
		return -1;
	}
	
	if(bind(room_id, (struct sockaddr*)&room, sizeof(struct sockaddr_in)) == -1){
		perror("launching a room: bind()");
		close(room_id);
		return -1;
	}
	
	if(listen(room_id, 1) == -1){
		perror("launching a room: listen()");
		close(room_id);
		return -1;
	}
	
    char* request = "GET / HTTP/1.0\r\nHost: api.ipify.org\r\n\r\n";
    char buff[1024];
    
    memset(&room_addr, 0, sizeof(room_addr));
    room_addr.sin_family      = AF_INET;
    if((room_addr.sin_addr.s_addr = inet_addr("50.19.238.1")) == -1) perror("inet_addr");
    room_addr.sin_port        = htons(80);
    
    if((addr_id = socket(AF_INET, SOCK_STREAM, 0)) == -1) perror("ip: socket()");
	if(connect(addr_id, (struct sockaddr*)&room_addr, sizeof(struct sockaddr_in)) == -1) perror("ip: connect()");
    if(send(addr_id, request, strlen(request), 0) == -1) perror("ip: send()");
    if(recv(addr_id, buff, 1023, 0) == -1) perror("ip: recv()");
    close(addr_id);
    
    for(int i = 0; i < 1024; i++)
		if(buff[i] == '\n')
			if(buff[i+1] >= '0' && buff[i+1] <= '9'){
				strncpy(ip, buff+i+1, 16);
				break;
			}
    
    return room_id;
}

int joinServer(char* ip){
	int room_id;
	struct sockaddr_in room;
	memset((char*)&room, 0, sizeof(struct sockaddr_in));
	room.sin_family = AF_INET;
	room.sin_port = htons(3456);
	room.sin_addr.s_addr = inet_addr(ip);
	
	if((room_id = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		perror("joining a room: socket()");
		return -1;
	}
	
	if(connect(room_id, (struct sockaddr*)&room, sizeof(struct sockaddr_in)) == -1){
		perror("joining a room: connect()");
		close(room_id);
		return -1;
	}
	
	return room_id;
}
