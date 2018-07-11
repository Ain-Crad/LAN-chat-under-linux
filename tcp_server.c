#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <net/if.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdbool.h>

#define portnumber 8633

void *rec_data(void *fd);

struct infnew{
	char user_new[100];
	char ip_new[100];
} inf;

struct data_usr{
	char data_user[100];
	char data_ip[100];
	pthread_t data_thread;
	int data_fd;
	bool online;
} data[10];

int cnt;
bool user_old;
const char ch[3] = ":\0";
const char priv[20] = "(private message)\0";

int main(int argc, char *argv[]){
	int sockfd, new_fd;
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	int sin_size;
	int nbytes;
	char hello[] = "Long time no see, how are you?\n";
	char succ[] = "login successful, enjoy the chat\n";
	int i = 0;
	
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		fprintf(stderr, "Socket error:%s\n\a", strerror(errno));
		exit(1);
	}
	
	bzero(&server_addr, sizeof(struct sockaddr_in));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(portnumber);
	
	if(bind(sockfd, (struct sockaddr *)(&server_addr), sizeof(struct sockaddr)) == -1){
		fprintf(stderr, "Bind error:%s\n\a", strerror(errno));
		exit(1);
	}
	
	if(listen(sockfd, 5) == -1){
		fprintf(stderr, "Listen error:%s\n\a", strerror(errno));
		exit(1);
	}
	
	memset(data, 0, sizeof(data));//初始化
	cnt = 0;
	while(1){
		memset(&inf, 0, sizeof(inf));//初始化
		user_old = false;
		pthread_t thread;//创建新的线程接收客户端数据
		sin_size = sizeof(struct sockaddr_in);
		if((new_fd = accept(sockfd, (struct sockaddr *)(&client_addr), &sin_size)) == -1){
			fprintf(stderr, "Accept error:%s\n\a", strerror(errno));
			exit(1);
		}
		//fprintf(stderr, "Server get connection from %s\n\a", inet_ntoa(client_addr.sin_addr));
		printf("Server get connection from %s\n\a", inet_ntoa(client_addr.sin_addr));
		if((nbytes = read(new_fd, &inf, sizeof(inf))) == -1){
			fprintf(stderr, "Read Error:%s\n", strerror(errno));
			exit(1);
		}
		//return login successful
		if(write(new_fd, succ, strlen(succ)) == -1){
			fprintf(stderr, "Write Error:%s\n", strerror(errno));
			exit(1);
		}
		//judge if it's the old user
		for(i = 0; i < cnt; i++){
			if(inf.user_new == data[i].data_user){
				user_old = true;
				strcpy(data[cnt].data_ip, inf.ip_new);
				data[i].data_thread = thread;
				data[i].data_fd = new_fd;
				data[i].online = true;
				break;
			}
		}
		if(!user_old){
			strcpy(data[cnt].data_user, inf.user_new);
			strcpy(data[cnt].data_ip, inf.ip_new);
			data[cnt].data_thread = thread;
			data[cnt].data_fd = new_fd;
			data[cnt].online = true;
			cnt++;
		}
		//create new thread
		if(pthread_create(&thread, NULL, (void *)rec_data, &new_fd) != 0){
			fprintf(stderr, "Create thread Error:%s\n", strerror(errno));
			exit(1);
		}
		//printf("user:%s\n", inf.user_str);
		//printf("ip:%s\n", inf.ip_str);
		//close(new_fd);
	}
	
	close(sockfd);
	exit(0);
}

void *rec_data(void *fd){
	char msg[100];
	int client_fd;
	int nbytes;
	client_fd = *((int *)fd);
	char user_name[100];
	int k;
	char object[100];
	int fd_tmp;
	char list[1024];
	
	while(1){
		memset(msg, 0, sizeof(msg));
		for(k = 0; k < cnt; k++){
			if(data[k].online == true && data[k].data_fd == client_fd){
				strcpy(user_name, data[k].data_user);
				break;
			}
		}
		if((nbytes = read(client_fd, msg, sizeof(msg))) == -1){
			fprintf(stderr, "Read Error:%s\n", strerror(errno));
			exit(1);
		}
		//printf("read again\nmsg:%s\n",msg);
		msg[nbytes] = '\0';
		strcat(user_name, ch);
		strcat(user_name, msg);
		
		if(strcmp(msg, "(end)")==0){
			//printf("server:end\n");
			int i;
			for(i = 0; i < cnt; i++){
				if(data[i].data_fd == client_fd){
					data[i].online = false;
					break;
				}
			}
			break;
		}
		else if(strcmp(msg, "(sendTo)")==0){
			int i;
			if((nbytes = read(client_fd, object, sizeof(object))) == -1){
				fprintf(stderr, "Read Error:%s\n", strerror(errno));
				exit(1);
			}
			object[nbytes] = '\0';
			if((nbytes = read(client_fd, msg, sizeof(msg))) == -1){
				fprintf(stderr, "Read Error:%s\n", strerror(errno));
				exit(1);
			}
			msg[nbytes] = '\0';
			
			for(k = 0; k < cnt; k++){
				if(data[k].online == true && data[k].data_fd == client_fd){
					strcpy(user_name, data[k].data_user);
					break;
				}
			}
			strcat(user_name, ch);
			strcat(user_name, msg);
			strcat(user_name, priv);
			for(i = 0; i < cnt; i++){
				if(data[i].online == true && strcmp(data[i].data_user, object) == 0){
					fd_tmp = data[i].data_fd;
					break;
				}
			}
			for(i = 0; i < cnt; i++){
				if(data[i].online == true && data[i].data_fd == fd_tmp){
					if(write(data[i].data_fd, user_name, strlen(user_name)) == -1){
						fprintf(stderr, "Write Error:%s\n", strerror(errno));
						exit(1);
					}
				}
			}
		}
		else if(strcmp(msg, "online_list")==0){
			int i;
			memset(list, 0, sizeof(list));
			for(i = 0; i < cnt; i++){
				if(data[i].online == true){
					strcat(list, data[i].data_user);
					list[strlen(list)] = '\t';
					strcat(list, data[i].data_ip);
					list[strlen(list)] = '\n';
				}
			}
			list[strlen(list)] = '\0';
			if(write(client_fd, list, strlen(list)) == -1){
				fprintf(stderr, "Write Error:%s\n", strerror(errno));
				exit(1);
			}
		}
		else{
			int i = 0;
			for(i = 0; i < cnt; i++){
				if(data[i].online == true && data[i].data_fd != client_fd){
					if(write(data[i].data_fd, user_name, strlen(user_name)) == -1){
						fprintf(stderr, "Write Error:%s\n", strerror(errno));
						exit(1);
					}
				}
			}
		}
		//printf("receive from client is %s/n",char_recv);
	}
	//free(fd);
	close(client_fd);
	pthread_exit(NULL);
}





