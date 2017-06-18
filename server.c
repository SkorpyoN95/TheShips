#include <sys/epoll.h>
#include <errno.h>
#include "serverManager.h"
#include "room.h"
#include "game.h"

rNode* room_list = NULL;
hLog* history = NULL;
struct epoll_event* events = NULL;

void closeRoom(int, siginfo_t*, void*);
void cleanUp();

int main(int argc, char **argv)
{
	char buff[256];
	char ip[16];
	int fd, cld, epoll_fd;
	struct epoll_event event;
	Room* act_room;
	char* command_ptr, * support_ptr;
	key_t server_key = ftok("./", 'P');
	atexit(cleanUp);
	signal(SIGINT, exit);
	if((s_q_id = msgget(server_key, 0666 | IPC_CREAT)) == -1){
		perror("Creating server msg queue");
		exit(1);
	}
	
	if((fd = launchServer(ip)) == -1) exit(1);
	if ((epoll_fd = epoll_create1(0)) == -1)
    {
      perror("epoll_create");
      exit(1);
    }
	event.data.fd = fd;
	event.events = EPOLLIN | EPOLLET | EPOLLRDHUP;
	if(epoll_ctl (epoll_fd, EPOLL_CTL_ADD, fd, &event) == -1)
    {
      perror("epoll_ctl");
      exit(1);
    }
    if((events = (struct epoll_event*)malloc(MAXEVENTS * sizeof(struct epoll_event))) == NULL){
		perror("epoll malloc");
		exit(1);
	}
	
	struct sigaction sa;
	sa.sa_sigaction = closeRoom;
	sa.sa_flags = SA_SIGINFO;
	sigemptyset(&sa.sa_mask);
	sigaddset(&sa.sa_mask, SIGCHLD);
	sigaction(SIGCHLD, &sa, NULL);
	
	printf("Server launched. IP: %s\n", ip);
	while(1){
		int n;
		if((n = epoll_pwait(epoll_fd, events, MAXEVENTS, -1, &sa.sa_mask)) == -1){
			perror("epoll wait");
			break;
		}
		for(int i = 0; i < n; ++i){
			if(events[i].data.fd == fd){		
				if((cld = accept(fd, NULL, NULL)) == -1){
					if(errno == EAGAIN || errno == EWOULDBLOCK) continue;
					else perror("accept failed");
				}else{
					event.data.fd = cld;
					event.events = EPOLLIN | EPOLLET;
					if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, cld, &event) == -1)	perror("epoll ctl");
					else printf("Player connected\n");
				}
			}else{
				if(events[i].events & EPOLLRDHUP){
					rNode* tmpr = room_list;
					while(tmpr && tmpr->room->player1->sock_id != events[i].data.fd && tmpr->room->player2->sock_id != events[i].data.fd)
						tmpr = tmpr->next;
					if(tmpr->room->player1->sock_id == events[i].data.fd )
						sendRequest(tmpr->room->player2->sock_id, GAME_OVER, 1, tmpr->room->player2->name);
					if(tmpr->room->player2->sock_id == events[i].data.fd )
						sendRequest(tmpr->room->player1->sock_id, GAME_OVER, 1, tmpr->room->player1->name);
					kill(tmpr->room->pid, SIGINT);
				}else{
					memset(buff, 0, 256);
					support_ptr = NULL;
					if(getResponse(events[i].data.fd, buff) == -1){
						printf("recieve failed\n");
						continue;
					}
					else{
						cld = events[i].data.fd;
						switch(buff[0]){
							
							case CREATE: 	command_ptr = strtok_r(buff+1, ":", &support_ptr); 
											act_room = addRoom(&room_list, command_ptr);
											if(act_room){
												strncpy(act_room->player1->name, support_ptr, 32);
												act_room->player1->sock_id = cld;
												dprintf(cld, "New room created. Name: %s\n", command_ptr);
											}
											break;
											
							case JOIN: 		command_ptr = strtok_r(buff+1, ":", &support_ptr);
											act_room = findRoom(room_list, command_ptr);
											if(act_room){
												if(act_room->player2){
													dprintf(cld, "This room is busy\n");
													break;
												}
												act_room->player2 = (Player*)malloc(sizeof(Player));
												strncpy(act_room->player2->name, support_ptr, 32);
												act_room->player2->sock_id = cld;
												dprintf(cld, "You joined to room: %s\n", act_room->name);
												launchRoom(fd, &room_list, act_room);
												printf("Room launched\n");
											}
											else
												dprintf(cld, "Room not found\n");
												
											break;
											
							case LIST:		memset(buff, 0, 256);
											listFreeRooms(room_list, buff);
											dprintf(cld, "%s", buff);
											break;
											
							case HISTORY:	memset(buff, 0, 256);
											hLog* tmp = history;
											while(tmp){
												sprintf(buff+strlen(buff), "%s\n", tmp->log);
												tmp = tmp->next;
											}
											if(!history) dprintf(cld, "No game has been played yet\n");
											else		 dprintf(cld, "%s", buff);
											break;
											
							case FILL_INFO:	command_ptr = strtok_r(buff+1, ":", &support_ptr); 
											act_room = findRoom(room_list, command_ptr);
											if(act_room){
												BaseMessage bsm;
												bsm.mtype = 1;
												memset(bsm.mtext, 0, 1024);
												strcpy(bsm.mtext, support_ptr);
												if(msgsnd(act_room->rqid, &bsm, 1024 + sizeof(int), 0) == -1)	perror("msgsnd");
												sendRequest(cld, START, 0);
											}else
												dprintf(cld, "-");
												
											break;
											
							case MOVE:		command_ptr = strtok_r(buff+1, ":", &support_ptr); 
											act_room = findRoom(room_list, command_ptr);
											if(act_room){
												BaseMessage bsm;
												bsm.mtype = 1;
												memset(bsm.mtext, 0, 1024);
												strcpy(bsm.mtext, support_ptr);
												if(msgsnd(act_room->rqid, &bsm, 1024 + sizeof(int), 0) == -1)
													perror("msgsnd");
											}
											break;
											
							default: 		printf("I got unknown request: %s\n", buff);
						}
					}
				}
			}	
		}
	}
	
	free(events);
	close(fd);
	msgctl(s_q_id, IPC_RMID, NULL);
	
	return 0;
}

void closeRoom(int signo, siginfo_t* siginf, void* ignore){
	rNode* tmp = room_list;
	while(tmp && tmp->room->pid != siginf->si_pid)	tmp = tmp->next;
	if(tmp){
		char txt[80];
		if(siginf->si_status == 1)	sprintf(txt, "%s won with %s",  tmp->room->player1->name, tmp->room->player2->name);
		else						sprintf(txt, "%s won with %s",  tmp->room->player2->name, tmp->room->player1->name);
		addLog(&history, txt);
		if(history->next && history->next->next && history->next->next->next){
			hLog* tmp_hist = history;
			history = history->next;
			free(tmp_hist);
		}
		deleteRoom(&room_list, tmp->room);
	}
}

void cleanUp(){
	free(events);
	hLog* htmp = NULL;
	while(history){
		htmp = history;
		history = history->next;
		free(htmp);
	}
	
	while(room_list)
		deleteRoom(&room_list, room_list->room);
}
