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
#include <time.h> 

#define portnumber 8633

void *rec_data(void *fd);
void email(char *email_send, char *verif);
void sendemail(char *email, char *body);
int open_socket(struct sockaddr *addr);

struct infnew{
	char user_new[100];
	char ip_new[100];
	char passwd_new[100];
} inf;

struct sign_up{
	char user_sign[100];
	char passwd_sign[100];
	char email_sign[100];
} sign;

struct data_usr{
	char data_user[100];
	char data_ip[100];
	char data_passwd[100];
	char data_email[100];
	//pthread_t data_thread;
	int data_fd;
	bool online;
	bool perm;
} data[10];

int cnt;
bool fir;
bool user_old;
const char ch[] = ":\0";
const char priv[] = "(private message)\0";
const char no_user[] = "*no such a user*\0";
const char l[] = "(\0";
const char r[] = ")\0";
const char sign_failed[] = "The user has been signed\0";
const char verif_failed[] = "The verification code is wrong\0";
const char log_failed[] = "The user name or the password is not correct\0";
const char conti[] = "continue\0";
const char hello[] = "Long time no see, how are you?\n";
const char log_succ[] = "login successful, enjoy the chat\n*********************************************************";
const char sign_succ[] = "sign up successful, you can log in now or continue to sign up a new user.\n*********************************************************************\n";
const char perm_y[] = "have permission\0";
const char perm_n[] = "have not permission\0";
const char stop[] = "stop talking\0";
char buffer[100];

char verif_code[10];
/******email-setting******/
struct data6
{
	unsigned int d4:6;
	unsigned int d3:6;
	unsigned int d2:6;
	unsigned int d1:6;
};
// 协议中加密部分使用的是base64方法
char con628(char c6);
void base64(char *dbuf,char *buf128,int len);
void sendemail(char *email,char *body);
int open_socket(struct sockaddr *addr);
/******email-setting******/

int main(int argc, char *argv[]){
	int sockfd, new_fd;
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	int sin_size;
	int nbytes;
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
	fir = true;
	
	while(1){
		memset(&inf, 0, sizeof(inf));//初始化
		memset(buffer, 0, sizeof(buffer));
		user_old = false;
		pthread_t thread;//创建新的线程接收客户端数据
		sin_size = sizeof(struct sockaddr_in);
		if((new_fd = accept(sockfd, (struct sockaddr *)(&client_addr), &sin_size)) == -1){
			fprintf(stderr, "Accept error:%s\n\a", strerror(errno));
			exit(1);
		}
		//fprintf(stderr, "Server get connection from %s\n\a", inet_ntoa(client_addr.sin_addr));
		printf("Server get connection from %s\n\a", inet_ntoa(client_addr.sin_addr));
		//create new thread
		if(pthread_create(&thread, NULL, (void *)rec_data, &new_fd) != 0){
			fprintf(stderr, "Create thread Error:%s\n", strerror(errno));
			exit(1);
		}
	}
	close(sockfd);
	exit(0);
}

void *rec_data(void *fd){
	int tmp;
	char msg[100];
	int client_fd;
	int nbytes;
	client_fd = *((int *)fd);
	char user_name[100];
	int k;
	char object[100];
	int fd_tmp;
	char list[1024];
	bool log_signal = false;
	while(1){
		memset(msg, 0, sizeof(msg));
		memset(user_name, 0, sizeof(user_name));
		strcpy(user_name, l);
		for(k = 0; k < cnt; k++){
			if(data[k].online == true && data[k].data_fd == client_fd){
				strcat(user_name, data[k].data_user);
				break;
			}
		}
		strcat(user_name, r);
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
		else if(strcmp(msg, "log in")==0 && !log_signal){
			int i;
			if((nbytes = read(client_fd, &inf, sizeof(struct infnew))) == -1){
				fprintf(stderr, "Read Error:%s\n", strerror(errno));
				exit(1);
			}
			for(i = 0; i < cnt; i++){
				if(strcmp(data[i].data_user, inf.user_new) == 0){
					if(strcmp(data[i].data_passwd, inf.passwd_new) == 0){
						strcpy(data[i].data_ip, inf.ip_new);
						//data[i].data_thread = thread;
						data[i].online = true;
						if(fir){
							data[i].perm = true;
							fir = false;
						}
						else{
							data[i].perm = false;
						}
						break;
					}
				}
			}
			//printf("I am here\n");
			if(i >= cnt){
				//printf("I am in if\n");
				if(write(client_fd, log_failed, strlen(log_failed)) == -1){
					fprintf(stderr, "Write Error:%s\n", strerror(errno));
					exit(1);
				}
			}
			else{
				//printf("I am in else\n");
				if(write(client_fd, log_succ, strlen(log_succ)) == -1){
					fprintf(stderr, "Write Error:%s\n", strerror(errno));
					exit(1);
				}
				log_signal = true;
			}
		}
		else if(strcmp(msg, "sign up") == 0 && !log_signal){
			int i;
			if((nbytes = read(client_fd, &sign, sizeof(struct sign_up))) == -1){
				fprintf(stderr, "Read Error:%s\n", strerror(errno));
				exit(1);
			}
				
			for(i = 0; i < cnt; i++){
				if(sign.user_sign == data[i].data_user){
					user_old = true;
					break;
				}
			}
				
			if(user_old){
				if(write(client_fd, sign_failed, strlen(sign_failed)) == -1){
					fprintf(stderr, "Write Error:%s\n", strerror(errno));
					exit(1);
				}
				user_old = false;
			}
			else{
				if(write(client_fd, conti, strlen(conti)) == -1){
					fprintf(stderr, "Write Error:%s\n", strerror(errno));
					exit(1);
				}
			}
			srand((unsigned)(time(NULL)));
			tmp = rand() % 10000;
			sprintf(verif_code, "%04d", tmp);
			printf("verif_code:%s\n", verif_code);
			email(sign.email_sign, verif_code);
			if((nbytes = read(client_fd, buffer, sizeof(buffer))) == -1){
				fprintf(stderr, "Read Error:%s\n", strerror(errno));
				exit(1);
			}
			//注册成功
			if(strcmp(buffer, verif_code) == 0){
				if(write(client_fd, sign_succ, strlen(sign_succ)) == -1){
					fprintf(stderr, "Write Error:%s\n", strerror(errno));
					exit(1);
				}
				strcpy(data[cnt].data_user, sign.user_sign);
				strcpy(data[cnt].data_passwd, sign.passwd_sign);
				strcpy(data[cnt].data_email, sign.email_sign);
				data[cnt].data_fd = client_fd;
				cnt++;
			}
			else{
				printf("buffer:%s\n", buffer);
				if(write(client_fd, verif_failed, strlen(verif_failed)) == -1){
					fprintf(stderr, "Write Error:%s\n", strerror(errno));
					exit(1);
				}
			}
		}
		else if(strcmp(msg, "(sendTo)")==0){
			int i;
			while(1){
				if((nbytes = read(client_fd, object, sizeof(object))) == -1){
					fprintf(stderr, "Read Error:%s\n", strerror(errno));
					exit(1);
				}
				object[nbytes] = '\0';
				for(i = 0; i < cnt; i++){
					if(data[i].online == true && strcmp(data[i].data_user, object) == 0){
						break;
					}
				}
				if(i >= cnt){
					if(write(client_fd, no_user, strlen(no_user)) == -1){
						fprintf(stderr, "Write Error:%s\n", strerror(errno));
						exit(1);
					}
				}
				else{
					if((nbytes = read(client_fd, msg, sizeof(msg))) == -1){
						fprintf(stderr, "Read Error:%s\n", strerror(errno));
						exit(1);
					}
					msg[nbytes] = '\0';
					strcpy(user_name, l);
					for(k = 0; k < cnt; k++){
						if(data[k].online == true && data[k].data_fd == client_fd){
							strcat(user_name, data[k].data_user);
							break;
						}
					}
					strcat(user_name, r);
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
					//break;
				}
				break;
			}
		}
		else if(strcmp(msg, "(online_list)")==0){
			int i;
			memset(list, 0, sizeof(list));
			for(i = 0; i < cnt; i++){
				if(data[i].online == true){
					strcat(list, data[i].data_user);
					list[strlen(list)] = '\t';
					strcat(list, data[i].data_ip);
					list[strlen(list)] = '\n';
					list[strlen(list)] = '\t';
					list[strlen(list)] = '\t';
					list[strlen(list)] = '\t';
				}
			}
			list[strlen(list)] = '\0';
			if(write(client_fd, list, strlen(list)) == -1){
				fprintf(stderr, "Write Error:%s\n", strerror(errno));
				exit(1);
			}
		}
		else if(strcmp(msg, "(banned)") == 0){
			int i;
			for(i = 0; i < cnt; i++){
				if(data[i].online == true && strcmp(data[i].data_fd, client_fd) == 0){
					if(data[i].perm){
						if(write(client_fd, perm_y, strlen(perm_y)) == -1){
							fprintf(stderr, "Write Error:%s\n", strerror(errno));
							exit(1);
						}
						if((nbytes = read(client_fd, object, sizeof(object))) == -1){
							fprintf(stderr, "Read Error:%s\n", strerror(errno));
							exit(1);
						}
						object[nbytes] = '\0';
						for(i = 0; i < cnt; i++){
							if(data[i].online == true && strcmp(data[i].data_user, object) == 0){
								fd_tmp = data[i].data_fd;
								break;
							}
						}
						if(i >= cnt){
							if(write(client_fd, no_user, strlen(no_user)) == -1){
								fprintf(stderr, "Write Error:%s\n", strerror(errno));
								exit(1);
							}
						}
						else{
							if(write(fd_tmp, stop, strlen(stop)) == -1){
								fprintf(stderr, "Write Error:%s\n", strerror(errno));
								exit(1);
							}
						}
					}
					else{
						if(write(client_fd, perm_n, strlen(perm_n)) == -1){
							fprintf(stderr, "Write Error:%s\n", strerror(errno));
							exit(1);
						}
					}
				}
				
			}
			
			if((nbytes = read(client_fd, object, sizeof(object))) == -1){
				fprintf(stderr, "Read Error:%s\n", strerror(errno));
				exit(1);
			}
			object[nbytes] = '\0';
		}
		else if(log_signal){
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

/********sendEmail********/

void email(char *email_send, char *verif){
	char email_rec[100];
	strcpy(email_rec, email_send);
	char body[] = "From: \"Aincrad\"<851056505@qq.com>\r\n"
	"To: \"dasiy\"<851056505@qq.com>\r\n"
	"Subject: verification code\r\n\r\n";
	strcat(body, verif);
	//"Hello World, Hello Email!";
	sendemail(email_rec, body);
}

char con628(char c6)
{
	char rtn = '\0';
	if (c6 < 26) rtn = c6 + 65;
	else if (c6 < 52) rtn = c6 + 71;
	else if (c6 < 62) rtn = c6 - 4;
	else if (c6 == 62) rtn = 43;
	else rtn = 47;
	return rtn;
}
 
// base64的实现
void base64(char *dbuf, char *buf128, int len)
{
	struct data6 *ddd = NULL;
	int i = 0;
	char buf[256] = {0};
	char *tmp = NULL;
	char cc = '\0';
	memset(buf, 0, 256);
	strcpy(buf, buf128);
	for(i = 1; i <= len/3; i++)
	{
		tmp = buf+(i-1)*3;
		cc = tmp[2];
		tmp[2] = tmp[0];
		tmp[0] = cc;
		ddd = (struct data6 *)tmp;
		dbuf[(i-1)*4+0] = con628((unsigned int)ddd->d1);
		dbuf[(i-1)*4+1] = con628((unsigned int)ddd->d2);
		dbuf[(i-1)*4+2] = con628((unsigned int)ddd->d3);
		dbuf[(i-1)*4+3] = con628((unsigned int)ddd->d4);
	}
	if(len%3 == 1)
	{
		tmp = buf+(i-1)*3;
		cc = tmp[2];
		tmp[2] = tmp[0];
		tmp[0] = cc;
		ddd = (struct data6 *)tmp;
		dbuf[(i-1)*4+0] = con628((unsigned int)ddd->d1);
		dbuf[(i-1)*4+1] = con628((unsigned int)ddd->d2);
		dbuf[(i-1)*4+2] = '=';
		dbuf[(i-1)*4+3] = '=';
	}
	if(len%3 == 2)
	{
		tmp = buf+(i-1)*3;
		cc = tmp[2];
		tmp[2] = tmp[0];
		tmp[0] = cc;
		ddd = (struct data6 *)tmp;
		dbuf[(i-1)*4+0] = con628((unsigned int)ddd->d1);
		dbuf[(i-1)*4+1] = con628((unsigned int)ddd->d2);
		dbuf[(i-1)*4+2] = con628((unsigned int)ddd->d3);
		dbuf[(i-1)*4+3] = '=';
	}
	return;
}
// 发送邮件
void sendemail(char *email, char *body)
{
	int sockfd = 0;
	struct sockaddr_in their_addr = {0};
	char buf[1500] = {0};
	char rbuf[1500] = {0};
	char login[128] = {0};
	char pass[128] = {0};
	#ifdef WIN32
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 2), &WSAData);
	#endif
	memset(&their_addr, 0, sizeof(their_addr));
	their_addr.sin_family = AF_INET;
	their_addr.sin_port = htons(25);//25
	their_addr.sin_addr.s_addr = inet_addr("14.17.57.241");//qq smtp 服务器
	// 连接邮件服务器，如果连接后没有响应，则2 秒后重新连接
	sockfd = open_socket((struct sockaddr *)&their_addr);
	memset(rbuf,0,1500);
	while(recv(sockfd, rbuf, 1500, 0) == 0)
	{
		printf("reconnect...\n");
		sleep(2);
		//close(sockfd);
		sockfd = open_socket((struct sockaddr *)&their_addr);
 
		memset(rbuf,0,1500);
	}
 
	//*printf("%s\n", rbuf);
 
	// EHLO
	memset(buf, 0, 1500);
	sprintf(buf, "EHLO abcdefg-PC\r\n");
	send(sockfd, buf, strlen(buf), 0);
	memset(rbuf, 0, 1500);
	recv(sockfd, rbuf, 1500, 0);
	//*printf("%s\n", rbuf);
 
	// AUTH LOGIN
	memset(buf, 0, 1500);
	sprintf(buf, "AUTH LOGIN\r\n");
	send(sockfd, buf, strlen(buf), 0);
	//*printf("%s\n", buf);
	memset(rbuf, 0, 1500);
	recv(sockfd, rbuf, 1500, 0);
	//*printf("%s\n", rbuf);
 
	// USER
	memset(buf, 0, 1500);
	sprintf(buf,"851056505");//你的qq号
	memset(login, 0, 128);
	base64(login, buf, strlen(buf));
	sprintf(buf, "%s\r\n", login);
	send(sockfd, buf, strlen(buf), 0);
	//*printf("%s\n", buf);
	memset(rbuf, 0, 1500);
	recv(sockfd, rbuf, 1500, 0);
	//*printf("%s\n", rbuf);
 
	// PASSWORD
	sprintf(buf, "wmxgpxgmqdhybbad");//你的qq密码
	memset(pass, 0, 128);
	base64(pass, buf, strlen(buf));
	sprintf(buf, "%s\r\n", pass);
	send(sockfd, buf, strlen(buf), 0);
	//*printf("%s\n", buf);
 
	memset(rbuf, 0, 1500);
	recv(sockfd, rbuf, 1500, 0);
	//*printf("%s\n", rbuf);
 
	// MAIL FROM
	memset(buf, 0, 1500);
	sprintf(buf, "MAIL FROM: <851056505@qq.com>\r\n");
	send(sockfd, buf, strlen(buf), 0);
	memset(rbuf, 0, 1500);
	recv(sockfd, rbuf, 1500, 0);
	//*printf("%s\n", rbuf);
 
	// RCPT TO 第一个收件人
	sprintf(buf, "RCPT TO:<%s>\r\n", email);
	send(sockfd, buf, strlen(buf), 0);
	memset(rbuf, 0, 1500);
	recv(sockfd, rbuf, 1500, 0);
	//*printf("%s\n", rbuf);
 
	// DATA 准备开始发送邮件内容
	sprintf(buf, "DATA\r\n");
	send(sockfd, buf, strlen(buf), 0);
	memset(rbuf, 0, 1500);
	recv(sockfd, rbuf, 1500, 0);
	//*printf("%s\n", rbuf);
 
	// 发送邮件内容，\r\n.\r\n内容结束标记
	sprintf(buf, "%s\r\n.\r\n", body);
	send(sockfd, buf, strlen(buf), 0);
	memset(rbuf, 0, 1500);
	recv(sockfd, rbuf, 1500, 0);
	//*printf("%s\n", rbuf);
 
	// QUIT
	sprintf(buf, "QUIT\r\n");
	send(sockfd, buf, strlen(buf), 0);
	memset(rbuf, 0, 1500);
	recv(sockfd, rbuf, 1500, 0);
	//*printf("%s\n", rbuf);
 
// VC2005 需要使用
	//closesocket(sockfd);
	close(sockfd);
	#ifdef WIN32
	WSACleanup();
	#endif
	return;
 
}
	// 打开TCP Socket连接
int open_socket(struct sockaddr *addr)
{
		int sockfd = 0;
		sockfd=socket(PF_INET, SOCK_STREAM, 0);
		if(sockfd < 0)
		{
			fprintf(stderr, "Open sockfd(TCP) error!\n");
			exit(-1);
		}
		if(connect(sockfd, addr, sizeof(struct sockaddr)) < 0)
		{
			fprintf(stderr, "Connect sockfd(TCP) error!\n");
			exit(-1);
		}
		return sockfd;
} 

