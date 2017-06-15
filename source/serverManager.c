#include "serverManager.h"

int launchServer(char* ip){
	int room_id, addr_id;
	struct sockaddr_in room, room_addr;
	memset((char*)&room, 0, sizeof(struct sockaddr_in));
	room.sin_family = AF_INET;
	room.sin_port = htons(3456);
	room.sin_addr.s_addr = htonl(INADDR_ANY);
	
	if((room_id = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0)) == -1){
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

int sendRequest(int sock_id, reqType t, int n, ...){
	if(n < 0) return -1;
	int result;
	if(!n)
		result = dprintf(sock_id, "%c", t) > 0 ? 0 : -1;
	else{
		char* format = (char*)malloc(n+n+n+1);
		memset(format, 0, n+n+n+1);
		format[0] = t; format[1] = '%'; format[2] = 's';
		for(int i = 0; i < n-1; ++i)
			strncpy(format + 3 + 3*i, ":%s", 3);
		va_list vl;
		va_start(vl, n);
		result = vdprintf(sock_id, format, vl) > 0 ? 0 : -1;
		va_end(vl);
		free(format);
	}
	return result;
}

void launchRoom(int fd, rNode** head, Room* room){
	pid_t room_pid;
	Player p1, p2;
	int r_q_id;
	char* cmd, * sup;
	BaseMessage send, recieve;
	//PlayerDataMessage pdm_rcv;
	switch(room_pid = fork()){
		case -1:	perror("Creating new process for room");
					deleteRoom(head, room);
					break;
		case 0:		close(fd);
					if((r_q_id = msgget(IPC_PRIVATE, 0666)) == -1){
						perror("Creating room msg queue");
						exit(1);
					}
					send.mtype = 999;
					send.mid = r_q_id;
					if(msgsnd(s_q_id, &send, 1024 + sizeof(int), 0) == -1){
						perror("Sending room msg queue id failed");
						exit(1);
					}
					
					if(msgrcv(r_q_id, &recieve, 1024 + sizeof(int), 1, 0) == -1){
						perror("Recieving room msg queue id failed");
						exit(1);
					}
					
					cmd = strtok_r(recieve.mtext, ":", &sup);
					strcpy(p1.name, cmd);
					unpackBoard(sup, &p1);
					sup = NULL;
					
					if(msgrcv(r_q_id, &recieve, 1024 + sizeof(int), 1, 0) == -1){
						perror("Recieving room msg queue id failed");
						exit(1);
					}
					
					cmd = strtok_r(recieve.mtext, ":", &sup);
					strcpy(p2.name, cmd);
					unpackBoard(sup, &p2);
					
					showBoard(p1.board);
					showBoard(p2.board);
					
					exit(0);
					break;
		default:	room->pid = room_pid;
					if(msgrcv(s_q_id, &recieve, 1024 + sizeof(int), 999, 0) == -1)	perror("Recieving room msg queue id failed");
					else 	room->rqid = recieve.mid;
					dprintf(room->player1->sock_id, "%c", START);
					dprintf(room->player2->sock_id, "%c", START);
					return;
	}
}
