#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define IPADRESS "127.0.0.1"
#define PORT 8081
#define MAXSIZE 1024
#define LISTENQ 5
#define FDSIZE 1000
#define EPOLLEVENTS 100

int socket_bind(const char *ip, int port);
void do_epoll(int listenfd);
void handle_events(int epollfd, struct epoll_event *events, int num,
                   int listenfd, char *buf);
void handle_accept(int epollfd, int listenfd);
void do_read(int epollfd, int fd, char *buf);
void do_write(int epollfd, int fd, char *buf);
void add_event(int epollfd, int fd, int state);
void modify_event(int epollfd, int fd, int state);
void delete_event(int epollfd, int fd, int state);

int main(int arge, char *argv[]) {
  int listenfd;
  listenfd = socket_bind(IPADRESS, PORT);
  listen(listenfd, LISTENQ);
  do_epoll(listenfd);
  return 0;
}