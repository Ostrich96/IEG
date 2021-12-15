#include<stdio.h>
#include<string.h>
#include<linux/input.h>
#include<fcntl.h>
#include<errno.h>
#include<sys/wait.h>
#include<sys/socket.h>
#include<sys/epoll.h>
#include<resolv.h>//操作系统域名解释器
#include<stdlib.h>
#include<netinet/in.h>//定义数据结构sockaddr_in
#include<arpa/inet.h>//ip地址转换函数
#include<unistd.h>

#define SERV_PORT 8083
#define IPADRESS "127.0.0.1"
#define MAXBUF 1024

#define DEV_PATH "/dev/input/event1"//利用cat /proc/bus/input/devices获得键盘事件

int main(int argc, char* argv[]){
    int sockfd,listenfd,keybdfd;
    struct input_event *t;
    struct sockaddr_in sockaddr;
    char buf[MAXBUF+1];
    unsigned int port;

    if((sockfd = socket(AF_INET,SOCK_STREAM,0))==-1){
        perror("socket open error：");
        exit(1);
    }else{
        printf("Socket created\n");
    }
    bzero(&sockaddr,sizeof(sockaddr));
    bzero(buf,MAXBUF);
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = htons(SERV_PORT);
    //inet_pton(AF_INET,IPADRESS,&sockaddr.sin_addr);
    sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(sockfd,(struct sockaddr*)&sockaddr,sizeof(sockaddr))== -1){
        perror("bind error:");
        exit(1);
    }else{
        printf("binded\n");
    
    }
    if(listen(sockfd,10) ==-1){
        perror("listen error:");
        exit(1);
    }else{
        printf("listening\n");
    }

    
    //system("vi 1");

    /*
    if(ioctl(keybdfd, EVIOCGRAB, 1))
        printf("Couldn't grab keyboard");
    else
        printf("Grabbed keyboard\n");
    */
    
    int socklen = sizeof(struct sockaddr_in);
    if((listenfd = accept(sockfd,(struct sockaddr*)&sockaddr,(socklen_t*)&socklen))==-1){
        perror("accept error:");
        exit(1);
    }else{
        printf("Accpeting\n");
    }
    //循环接收数据并写入键盘事件
    
    input_event last;
    last.code = 0;
    last.value = 0;
    last.type = 0;
    //system("vi 1");
    while(1){
        int len =0;

        
        
            if((len = recv(listenfd,buf,MAXBUF,0))>0){
                //printf("recved\n");

                t = (input_event*)buf;
                
                if((keybdfd = open(DEV_PATH,O_RDWR))<0){
                    printf("open device failed\n");
                    exit(1);
                }else{
                    //printf("open device succeed\n");
                }
                if(t->type == 4){
                    continue;
                }
                if(t->type == 1){
                    t->type = EV_KEY;
                }
                if(t->type == 0){
                    t->type = EV_SYN;
                    t->code = SYN_REPORT;
                }
                gettimeofday(&t->time,NULL);
                if(write(keybdfd,t,sizeof(struct input_event))!=sizeof(struct input_event)){
                    printf("press key error");
                    exit(1);
                }else{
                    printf("write\n %d %d %d\nsucceed\n",t->type,t->code,t->value);
                    
                    if(t->type == 1){
                        if(write(keybdfd,&last,sizeof(last))!=sizeof(last)){
                            printf("write last unsucceed\n");
                            exit(1);
                        }
                        else{
                            printf("write \n %d %d %d\nsucceed\n",last.type,last.code,last.value);
                        }
                    }
                    
                }
        }
        if(close(keybdfd)){
            printf("close keybdfd failed");
            exit(1);
        }
        
    }
    if(close(listenfd)){
            printf("close listenfd failed");
            exit(1);
        }   

    
    close(sockfd);
    return 0;
}