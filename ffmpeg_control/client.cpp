#include<stdio.h>
#include<string.h>
#include<linux/input.h>
#include<fcntl.h>
#include<errno.h>
#include<sys/socket.h>
#include<sys/epoll.h>
#include<resolv.h>//操作系统域名解释器
#include<stdlib.h>
#include<netinet/in.h>//定义数据结构sockaddr_in
#include<arpa/inet.h>//ip地址转换函数
#include<unistd.h>

#define SERV_PORT 8082
#define IPADRESS "127.0.0.1"
#define MAXBUF 1024

#define DEV_PATH "/dev/input/event1"//利用cat /proc/bus/input/devices获得键盘事件

int main(int argc,char**argv){
    int sockfd,len,new_fd;
    struct input_event t;
    struct sockaddr_in servaddr;
    char buf[MAXBUF+1];

    if((sockfd = socket(AF_INET,SOCK_STREAM,0))<0){
        perror("Socket ");
        exit(errno);
    }
    printf("Socket created\n");

    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    if(inet_pton(AF_INET,IPADRESS,&servaddr.sin_addr)<=0){
        perror(argv[1]);
        exit(errno);
    }
    printf("address created\n");
    //初始化完成

    if((new_fd = open(DEV_PATH, O_RDWR))<0){//监听键盘输入
        printf("open device failed");
        exit(1);
    }else{
        printf("listening the keyboard\n");
    }
    //abadon first three event;
    
    for(int i = 0;i <3;i++){
        read(new_fd,&t,sizeof(t));
    }
    


    //连接服务器
    if(connect(sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr))<0){
        perror("Connect ");
        exit(errno);
    }
    printf("server connected\n");

    if(ioctl(new_fd, EVIOCGRAB, 1))
        printf("Couldn't grab keyboard");
    else
        printf("Grabbed keyboard\n");

    while(1){
        bzero(buf,MAXBUF);
        int lengsize = read(new_fd,&t,sizeof(t));
        if(lengsize== sizeof(t)){
            memcpy(buf,&t,sizeof(t));
            if(send(sockfd,buf,lengsize,0)<0){
                printf("Send error ");
                break;
            }
            else{
                printf("send\n %d %d %d\n",t.type,t.code,t.value);
            }
        }
        else{
            break;
        }
        
    }

    if(close(new_fd)){
        printf("close device failed");
        exit(1);
    }
    close(sockfd);
    return 0;
    

}