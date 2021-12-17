# IEG-TCPBench
基于epoll做了一个TCP压力测试

## 主要内容：
1. 创建一个epoll，监听最大10240个句柄
2. 读入argv -c 后客户端参数与 -n 后请求总数
3. 循环将请求总数个事件注册到epollfd中，epollfd获得已经准备好的描述符事件后，根据描述符的类型和事件类型进行处理。如果事件可写，就发送"Hello TCP!"，记录时间戳,之后转换为可读事件添加进epollfd等待服务端回信。
4. 收到服务端回信后，记录时间戳，关闭此request的fd
5. 如果有client的request连接超时，则resetClient，将之前的事件删除、重新注册
6. 完成规定数量的request或总运行时间超时，则返回最终结果，统计request两次时间戳的差，加入数组，排列数组，输出结果。

## 运行代码：
./benchmark -c 客户端数 -n 请求总数 -p 端口号

## 测试结果
![alt](https://github.com/Ostrich96/IEG/blob/main/result/result.png)
## 优化改进思路
正在进行epoll+多进程的改进，看能否提高最大并发数。尝试将发送与接受请求使用线程池进行并发处理。


# ffmpeg_control
客户端远程操控服务端鼠标和键盘


## 内容及结构
### 1.客户端
#define SERV_PORT 8082
#define IPADDRESS "110.40.193.165"
#define KEY_DEV_PATH "/dev/input/event2"
#define MOU_DEV_PATH "/dev/input/event0"
#define FDSIZE 10
定义云服务器地址、请求端口号、鼠标与键盘设备输入路径与epoll监听事件数目

主线程进行TCP连接，connect成功后调用handle_connection()函数处理读event以及socket内容发送

handle_connection()创建一个epoll句柄,之后打开event，方式只读、无阻塞
kbfd = open(KEY_DEV_PATH, O_RDONLY | O_NONBLOCK);
msfd = open(MOU_DEV_PATH,O_RDONLY | O_NONBLOCK);
add_event(epollfd,kbfd,EPOLLIN);
add_event(epollfd,msfd,EPOLLIN);
之后进入epoll_wait，当有事件就绪时调用handle_events()函数

handle_events()轮询到事件为EPOLLIN的事件时，调用do_read()函数

do_read()主要调用函数read()从event的句柄中读出input_event事件，并且进行分类处理，当event的句柄是鼠标的event时，将其struct input_event.type +5，作为与keyboard的input_event区分
    if(fd == msfd){
        et.type += 5;
    }
之后调用send()函数进行TCP传输
为防止读出数据产生错误，先进行条件判断type是否==1、5、6、7、8，1对应来自keyboard的EV_KEY,5对应来自鼠标的EV_SYN，6对应来自鼠标的EV_KEY，7对应EV_REL相对位移，8对应EV_ABS绝对位移。因为是水平触发模式（LT），所以之后调用delete_event()与add_event()进行句柄的删除与再注册以避免一直返回事件就绪。

最后关闭文件、socket句柄、epoll句柄结束

### 2.服务端
#define SERV_PORT 6667
#define IPADDRESS "127.0.0.1"
#define KEY_DEV_PATH "/dev/input/event1"
#define MOU_DEV_PATH "/dev/input/event2"
#define FDSIZE 10
监听本地6667端口，以及定义本地鼠标与键盘event路径

主线程创建了一个Capture类，但也可以直接处理函数功能（只是想复习一下c++类）
listenfd = controlinput.socket_bind(IPADDRESS,SERV_PORT);
listen(listenfd,10);
controlinput.do_epoll(listenfd);
绑定socket端口，调用listen()将句柄转换为被动监听模式,之后调用do_epoll函数

do_epoll()函数创建epoll句柄，之后进入epoll_wait()，等待事件就绪后调用handle_events()函数

handle_event()函数处理epoll事件，当事件句柄是listenfd时调用handle_accpet()函数进行连接，创建新的socket句柄用来通信，当事件是EPOLLIN时调用do_read()函数进行input_event事件的写入

handle_accept()函数创建新的socket句柄用来通信，并调用add_event()将此句柄加入epoll事件

do_read()函数进行tcp传输的接收，之后将其写入鼠标event或键盘event,在写入之前进行判断，如果接收到的input_event的type值等于1，则写入键盘event，因为游戏比较简单，所以并没有传入EV_SYN同步，在写入键盘后自行再写入一个type=0，code=0，value=0的input_event事件（此处可再优化）,之后关闭键盘的IO再打开，防止数据写入缓存区没有写入磁盘（或可进行flush()，因为频繁关闭打开文件会占用大量内存）
如果接受到的input_event的type== 5、6、8则说明此input_evnt应将type-5后写入鼠标event。

最后关闭文件、socket句柄、epoll句柄结束


## 运行：
1.客户端监听本地鼠标和键盘输入，利用 cat /proc/bus/input/devices 命令查看对应的鼠标和键盘event，改成对应的路径，用sudo chmod 777 event0来放开event的权限，不然可能无法读取与写入。
2.服务端输入 ssh -T -R 8083:localhost:6666 ostrich@110.40.193.165来打开ssh反向隧道
3.服务端同样记得开放event权限与修改对应路径，不然可能无法写入。
