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
#define MAX_BUF_SIZE 100
int get_local_ip(const char *eth_inf, char *ip, int sd);//获取本机ip地址
void *rec_data(void *fd);

struct infstr{
	char user_str[100];
	char ip_str[100];
} inf;

int main(int argc, char *argv[]){
	const char *eth_name = "wlp6s0";
	char ip[16];
	int sockfd;
	char model[100];
	char buffer[1024];
	char user[100];
	char send_msg[100];
	struct sockaddr_in server_addr;
	struct hostent *host;
	int nbytes;
	char hi[30] = "Hi, server\n";
	pthread_t thread;
	int i;
	
	if(argc != 2){
		fprintf(stderr, "Usage:%s hostname \a\n", argv[0]);
		exit(1);
	}
	
	if((host = gethostbyname(argv[1])) == NULL){
		fprintf(stderr, "Gethostname error\n");
		exit(1);
	}
	
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		fprintf(stderr, "Socket Error:%s\a\n", strerror(errno));
		exit(1);
	}
	
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(portnumber);
	
	server_addr.sin_addr = *((struct in_addr*)host->h_addr);
	
	if(connect(sockfd, (struct sockaddr*)(&server_addr), sizeof(struct sockaddr)) == -1){
		fprintf(stderr, "Connnect Error:%s\a\n", strerror(errno));
		exit(1);
	}
	get_local_ip(eth_name, ip, sockfd);
	
	printf("log in or sign up\n");
	fgets(model, MAX_BUF_SIZE, stdin);
	while(1){
		if(strcmp(model,"log in\n") == 0){
			printf("user:");
			fgets(user, MAX_BUF_SIZE, stdin);
			for(i = 0; i < MAX_BUF_SIZE; i++){
				if(user[i] == '\n'){
					user[i] = '\0';
					break;
				}
			}
			strcpy(inf.user_str, user);
			strcpy(inf.ip_str, ip);
			if(write(sockfd, &inf, sizeof(inf)) == -1){
				fprintf(stderr, "Write Error:%s\n", strerror(errno));
				exit(1);
			}
			
			if((nbytes = read(sockfd, buffer, 1024)) == -1){
				fprintf(stderr, "Read Error:%s\n", strerror(errno));
				exit(1);
			}
			buffer[nbytes] = '\0';
			printf("%s\n", buffer);
			break;
		}
		else if(strcmp(model,"sign up\n") == 0){
			
			break;
		}
		else{
			printf("No such a choice, please input again:");
			memset(model, 0, sizeof(model));
			fgets(model, MAX_BUF_SIZE, stdin);
		}
	}
	
	
	if(pthread_create(&thread, NULL, (void *)rec_data, &sockfd) != 0){
		fprintf(stderr, "Create thread Error:%s\n", strerror(errno));
		exit(1);
	}
	
	while(1){
		memset(send_msg, 0, sizeof(send_msg));
		//printf("send:");
		fgets(send_msg, MAX_BUF_SIZE, stdin);
		for(i = 0; i < strlen(send_msg); i++){
			if(send_msg[i] == '\n'){
				send_msg[i] = '\0';
				break;
			}
		}
		if(write(sockfd, send_msg, sizeof(send_msg)) == -1){
			fprintf(stderr, "Write Error:%s\n", strerror(errno));
			exit(1);
		}
		if(strcmp(send_msg, "(end)")==0){
			break;
		}
		else if(strcmp(send_msg, "(sendTo)")==0){
			fgets(send_msg, MAX_BUF_SIZE, stdin);
			for(i = 0; i < strlen(send_msg); i++){
				if(send_msg[i] == '\n'){
					send_msg[i] = '\0';
					break;
				}
			}
			if(write(sockfd, send_msg, sizeof(send_msg)) == -1){
				fprintf(stderr, "Write Error:%s\n", strerror(errno));
				exit(1);
			}
			
			fgets(send_msg, MAX_BUF_SIZE, stdin);
			for(i = 0; i < strlen(send_msg); i++){
				if(send_msg[i] == '\n'){
					send_msg[i] = '\0';
					break;
				}
			}
			if(write(sockfd, send_msg, sizeof(send_msg)) == -1){
				fprintf(stderr, "Write Error:%s\n", strerror(errno));
				exit(1);
			}
			printf("(private message)\n");
		}
	}

	pthread_detach(thread);//destory thread
	close(sockfd);
	exit(0);
}

// 获取本机ip
int get_local_ip(const char *eth_inf, char *ip, int sd)
{
	struct sockaddr_in sin;
	struct ifreq ifr;
 
	strncpy(ifr.ifr_name, eth_inf, IFNAMSIZ);
	ifr.ifr_name[IFNAMSIZ - 1] = 0;
	
	// if error: No such device
	if (ioctl(sd, SIOCGIFADDR, &ifr) < 0)
	{
		printf("ioctl error: %s\n", strerror(errno));
		close(sd);
		return -1;
	}
 
	memcpy(&sin, &ifr.ifr_addr, sizeof(sin));
	snprintf(ip, 16, "%s", inet_ntoa(sin.sin_addr));
	return 0;
}

void *rec_data(void *fd){
	int nbytes;
	int sock_fd;
	char rec_msg[100];
	sock_fd = *((int *)fd);
	while(1){
		//memset(rec_msg, 0, sizeof(rec_msg));//有错误
		if((nbytes = read(sock_fd, rec_msg, sizeof(rec_msg))) == -1){
			fprintf(stderr, "Read Error:%s\n", strerror(errno));
			exit(1);
		}
		rec_msg[nbytes] = '\0';
		printf("%s\n", rec_msg);
	}
	//free(fd);
	//close(sock_fd);
	pthread_exit(NULL);
	
}



