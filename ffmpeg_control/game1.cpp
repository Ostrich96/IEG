#include<stdio.h>
#include<curses.h>
#include<signal.h>
#include<unistd.h>
#include<string.h>
#include<sys/time.h>
#include<time.h>

#define MESSAGE "O"
#define BLANK "   "

int row;
int col;
int dir;

int main(){
    int delay;
    int ndelay;
    int c;//user input
    void move_msg(int);//handler for timer
    int set_ticker(int);

    initscr();
    crmode();//read immediately without ENTER
    noecho();//no echo
    clear();

    row = 10;
    col = 0;
    dir = 1;
    delay = 200;// 200ms = 0.2s
    
    move(row,col);
    addstr(MESSAGE);
    signal(SIGALRM,move_msg);
    set_ticker(delay);

    while(1){
        ndelay = 0;
        c = getch();
        if(c == 'Q')break;
        if(c == ' ')dir = -dir;
        if(c == 'f'&&delay>2)ndelay = delay/2;
        if(c == 's')ndelay = delay *2;
        if(ndelay>0)
            set_ticker(delay = ndelay);
    }
    endwin();
    return 0;
}

void move_msg(int signum){
    signal(SIGALRM,move_msg);//reset the signal function;
    move(row, col);
    addstr(BLANK);
    col+= dir;
    move(row,col);
    addstr(MESSAGE);
    refresh();

    if(dir == -1 && col <=0)
        dir =1;
    else if(dir ==1&&col +strlen(MESSAGE)>=COLS)
        dir = -1;
}

int set_ticker(int n_msecs)
{
    struct itimerval new_timeset;
    long n_sec,n_usecs;

    n_sec = n_msecs/1000;
    n_usecs = (n_msecs%1000)*1000L;

    new_timeset.it_interval.tv_sec = n_sec;
    new_timeset.it_interval.tv_usec = n_usecs;
    new_timeset.it_value.tv_sec = n_sec;
    new_timeset.it_value.tv_usec = n_usecs;

    return setitimer(ITIMER_REAL,&new_timeset,NULL);
}