以下两个是爬虫以及抽取链接关系的程序，平台不一样但是干的一件事。运行程序的时候ip和端口看实际，比如这次的ip是实验室服务器的，端口是80
#crawler:linux下没有打印信息的
#libevent_http:windows下有打印信息的（应该没错，有错的话就从crawler往windows上移，crawler一定没有错）
以下为pagerank的程序，都是linux下的，和windows差的不多所以就只放了linux下的

pagerankShouLian:linux下打印收敛值
带调试信息：pagerank在linux下带打印信息的，这个的pagerank版本可能有点老，不如外面的pagerankShouLian效率高

如何把程序从windows移到linux：
    爬虫的cpp:
	去掉：
	    #include <WinSock2.h>
	    include "queue.h"
	    WSADATA wsa_data;
	    WSAStartup(MAKEWORD(2, 2), &wsa_data);
        添加：
    	    linux中网络编程相关的相关的：
	    #include <unistd.h>
	    #include <arpa/inet.h>
	    #include <sys/socket.h>
	    #include <netinet/in.h>
            和strstr一些奇奇怪怪的函数相关的：
	    #include <string.h>
	更改：
            Sleep改成sleep.Sleep中1000是1s，小sleep中1是一秒
	那个#include "queue.h"也要去掉，在服务器上没有这个
    pagerank的cpp：
        只加上#include <unistd.h>和#include <string.h>就行，因为不涉及网络编程的，所以很好改的（有点记不清反正很好改，可以到时候自己试试）
在linux上编译方法：
    g++ -std=c++11 crawler.cpp -o crawler -levent
    g++ -std=c++11 pagerank.cpp -o pagerank
    g++ -std=c++11 pagerankShouLian.cpp -o pagerank

pagerank，linux下只能用在linux生成的url.txt，不能用windows下生成的url.txt!（因为linux下和windows下的换行符是不一样的）

注意：nginx的并发数设置也很重要，设置大一点能爬多一点