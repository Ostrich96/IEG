# IEG-TCPBench
基于epoll做了一个TCP压力测试

## 主要内容：
1.创建一个epoll，监听最大10240个句柄
2.读入argv -c 后客户端参数与 -n 后请求总数
3.循环将请求总数个事件注册到epollfd中，epollfd获得已经准备好的描述符事件后，根据描述符的类型和事件类型进行处理。如果事件可写，就发送"Hello TCP!"，记录时间戳,之后转换为可读事件添加进epollfd等待服务端回信。
4.收到服务端回信后，记录时间戳，关闭此request的fd
5.如果有client的request连接超时，则resetClient，将之前的事件删除、重新注册
6.完成规定数量的request或总运行时间超时，则返回最终结果，统计request两次时间戳的差，加入数组，排列数组，输出结果。

## 运行代码：
./benchmark -c 客户端数 -n 请求总数 -p 端口号

## 测试结果
![alt](https://github.com/Ostrich96/IEG/blob/main/result/result.png)
## 优化改进思路
正在进行epoll+多进程的改进，看能否提高最大并发数。尝试将发送与接受请求使用线程池进行并发处理。
