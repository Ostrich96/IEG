#include <stdio.h>
#include <stdlib.h>
#include <linux/input.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

static void show_event(struct input_event* event)
{
        printf("%d %d %d\n", event->type, event->code, event->value);

        return;
}

int main(int argc,char** argv)
{
        struct input_event event = {{0}, 0};
        const char* file_name = argc == 2 ? argv[1] : "/dev/input/event1";
        int fd = 0;
        if((fd = open(file_name, O_RDWR))<0){
                printf("open file error");
                exit(1);
        }

        if(fd > 0)
        {

                while(1)
                {
                        int ret = read(fd, &event, sizeof(event));
                        if(ret == sizeof(event))
                        {
                                show_event(&event);
                        }
                        else
                        {
                                break;
                        }
                }
                close(fd);
        }

        return 0;
}