#include "serverManager.h"
#include <ctype.h>

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
		result = dprintf(sock_id, "%c%c",1, t) > 0 ? 0 : -1;
	else{
		char* format = (char*)malloc(n+n+n+1);
		char buff[255] = {0};
		memset(format, 0, n+n+n+1);
		format[0] = t; format[1] = '%'; format[2] = 's';
		for(int i = 0; i < n-1; ++i)
			strncpy(format + 3 + 3*i, ":%s", 3);
		va_list vl;
		va_start(vl, n);
		vsprintf(buff, format, vl);
		va_end(vl);
		result = dprintf(sock_id, "%c%s", (char)strlen(buff), buff) > 0 ? 0 : -1;
		free(format);
	}
	return result;
}

int getResponse(int fd, char* buff){
	char length;
	if(read(fd, &length, 1) == -1) return -1;
	if(read(fd, buff, (int)length) == -1) return -1;
	return 0;
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
		case 0:		if((r_q_id = msgget(IPC_PRIVATE, 0666)) == -1){
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
					cmd = strtok_r(NULL, ":", &sup);
					
					int sock_cmp = strcmp(p1.name, room->player1->name);
					if(!sock_cmp)
						p1.sock_id = room->player1->sock_id;
					else
						p1.sock_id = room->player2->sock_id;
						
					unpackBoard(cmd, sup, &p1);
					sup = NULL;
					
					if(msgrcv(r_q_id, &recieve, 1024 + sizeof(int), 1, 0) == -1){
						perror("Recieving room msg queue id failed");
						exit(1);
					}
					
					cmd = strtok_r(recieve.mtext, ":", &sup);
					strcpy(p2.name, cmd);
					cmd = strtok_r(NULL, ":", &sup);
					
					if(!sock_cmp)
						p2.sock_id = room->player2->sock_id;
					else
						p2.sock_id = room->player1->sock_id;
						
					unpackBoard(cmd, sup, &p2);
					
					int run = 1;
					Player* turn = &p1, * turn_over = &p2, * tmp = NULL;
					
					while(run){
						if(sendRequest(turn->sock_id, TURN, 0) == -1)
							printf("sendRequest TURN failed\n");
						
						if(msgrcv(r_q_id, &recieve, 1024 + sizeof(int), 1, 0) == -1){
							perror("Recieving room msg queue id failed");
							exit(1);
						}
						
						int x = tolower(recieve.mtext[0]) - 'a';
						int y = atoi(recieve.mtext + 1) - 1;
						if(turn_over->board[x][y] == SHIP){
							turn_over->board[x][y] = DAMAGED;
							int index = 10 * x + y, i = 0, number = 1;
							for(; i < 20; ++i)	if(turn_over->ships[i] == index) break;
							if(i > 15) number = i%10 + 1;
							else if(i > 9) number = (i - 2) / 2;
								 else if(i > 3) number = (i + 2) / 3;
									  else number = 1;
									  
							if(isShipSinked(turn_over, number)){
								char result[16] = {0};
								if(i < 4)	snprintf(result, 16, "%c4:%d:%d:%d:%d:%d:%d:%d:%d", WRECK, turn_over->ships[0]/10, turn_over->ships[0]%10,
															turn_over->ships[1]/10, turn_over->ships[1]%10, turn_over->ships[2]/10, turn_over->ships[2]%10,
															turn_over->ships[3]/10, turn_over->ships[3]%10);
								if(i == 4 || i == 5 || i == 6)	snprintf(result, 16, "%c3:%d:%d:%d:%d:%d:%d", WRECK,
															turn_over->ships[4]/10, turn_over->ships[4]%10, turn_over->ships[5]/10, turn_over->ships[5]%10,
															turn_over->ships[6]/10, turn_over->ships[6]%10);
								if(i == 7 || i == 8 || i == 9)	snprintf(result, 16, "%c3:%d:%d:%d:%d:%d:%d", WRECK,
															turn_over->ships[7]/10, turn_over->ships[7]%10, turn_over->ships[8]/10, turn_over->ships[8]%10,
															turn_over->ships[9]/10, turn_over->ships[9]%10);
								if(i == 10 || i == 11)	snprintf(result, 16, "%c2:%d:%d:%d:%d", WRECK, turn_over->ships[10]/10, turn_over->ships[10]%10,
															turn_over->ships[11]/10, turn_over->ships[11]%10);
								if(i == 12 || i == 13)	snprintf(result, 16, "%c2:%d:%d:%d:%d", WRECK, turn_over->ships[12]/10, turn_over->ships[12]%10,
															turn_over->ships[13]/10, turn_over->ships[13]%10);
								if(i == 14 || i == 15)	snprintf(result, 16, "%c2:%d:%d:%d:%d", WRECK, turn_over->ships[14]/10, turn_over->ships[14]%10,
															turn_over->ships[15]/10, turn_over->ships[15]%10);
								if(i > 15)	snprintf(result, 16, "%c1:%d:%d", WRECK, x, y);
								if(sendRequest(turn->sock_id, RESULT, 1, result) == -1) printf("Game Loop: sendRequest failed in WRECK section\n");
								if(sendRequest(turn_over->sock_id, RESULT, 1, result) == -1) printf("Game Loop: sendRequest failed in WRECK section\n");
							}else{
								char result[16] = {0};
								snprintf(result, 16, "%c:%d:%d", SHIP, x, y);
								if(sendRequest(turn->sock_id, RESULT, 1, result) == -1) printf("Game Loop: sendRequest failed in SHIP section\n");
								if(sendRequest(turn_over->sock_id, RESULT, 1, result) == -1) printf("Game Loop: sendRequest failed in SHIP section\n");
							}
							if(countPlayerHP(turn_over) == 0){
								if(sendRequest(turn->sock_id, GAME_OVER, 1, turn->name) == -1) printf("Game Loop: sendRequest failed in GAME_OVER section\n");
								if(sendRequest(turn_over->sock_id, GAME_OVER, 1, turn->name) == -1) printf("Game Loop: sendRequest failed in GAME_OVER section\n");
								run = 0;
								continue;
							}
						}else{
							char result[16] = {0};
							snprintf(result, 16, "%c:%d:%d", SEA, x, y);
							if(sendRequest(turn->sock_id, RESULT, 1, result) == -1) printf("Game Loop: sendRequest failed in SEA section\n");
							if(sendRequest(turn_over->sock_id, RESULT, 1, result) == -1) printf("Game Loop: sendRequest failed in SEA section\n");
							tmp = turn_over;
							turn_over = turn;
							turn = tmp;
						}
					}
					
					exit(0);
					break;
		default:	room->pid = room_pid;
					if(msgrcv(s_q_id, &recieve, 1024 + sizeof(int), 999, 0) == -1)	perror("Recieving room msg queue id failed");
					else 	room->rqid = recieve.mid;
					sendRequest(room->player1->sock_id, START, 0);
					sendRequest(room->player2->sock_id, START, 0);
					return;
	}
}
