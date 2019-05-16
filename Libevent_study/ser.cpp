//
// Created by yanpan on 2019/4/26.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <event.h>
using namespace std;

//#define address 127.0.0.1
//#define port 8888


void OnRead(struct bufferevent *bev, void *arg)
{
    cout<<"bufferent_event_ok happened ....."<<endl;
    evutil_socket_t sfd = bufferevent_getfd(bev);
    int recv_size = 0;
    char buf[100];
    memset(buf,0,sizeof(char)*100);
    std::cout<<"enter buf==="<<buf<<std::endl;
    if ((recv_size = bufferevent_read(bev, buf, 100)) > 0) // 读取前100位的数据
    {
    }
    std::cout<<"get buf==="<<buf<<std::endl;
    std::string str_recv(buf, 5); // 字符串截取前 5位数据
    std::cout<<"str_recv=="<<str_recv<<std::endl;
 
    /*实现回显功能*/
    int write_num = bufferevent_write(bev, "ok", 2);
    cout << write_num << " characters written" << endl;
}


void bufferent_event_erro3(struct bufferevent *bev, short event, void *arg)
{
    evutil_socket_t sfd = bufferevent_getfd(bev);
    bufferevent_free(bev);
    if (event & BEV_EVENT_TIMEOUT)
    {
        cout<<"bufferent_event_erro3 happened 1....."<<endl;
    }
    else if (event & BEV_EVENT_EOF)
    {
        cout<<"bufferent_event_erro3 happened 2....."<<endl;
        int error_code = EVUTIL_SOCKET_ERROR();
        cout<<"on_accept_bak happened error_code....."<<error_code<<", description ="<< evutil_socket_error_to_string(error_code)<<endl;
    }
    else if (event & BEV_EVENT_ERROR)
    {
        int error_code = EVUTIL_SOCKET_ERROR();
        cout<<"on_accept_bak happened error_code....."<<error_code<<", description ="<< evutil_socket_error_to_string(error_code)<<endl;
    }
}

void OnAccept(int sockfd, short iEvent, void *arg)
{
    cout<< "OnAccept happended >>>>> "  <<endl;
    struct sockaddr_in cli;
    socklen_t len = sizeof(cli);
    evutil_socket_t fd = accept(sockfd, (struct sockaddr*)&cli, &len);
    if(-1 == fd)
    {
        cout<< "fd error" <<endl;
        return;
    }
    struct event_base* base = static_cast<event_base*> (arg);

    int flag = EV_READ | EV_PERSIST;
    evutil_make_socket_nonblocking(fd);
    struct bufferevent *bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
    bufferevent_setcb(bev, OnRead, NULL, bufferent_event_erro3, NULL);
    bufferevent_enable(bev, flag);
}
int main()
{
    int sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (-1 == sockfd)
        return 0;

    struct sockaddr_in ser;
    ser.sin_family = PF_INET;
    ser.sin_addr.s_addr = inet_addr("127.0.0.1");
    ser.sin_port = htons(8888);

    int ret = bind(sockfd, (struct sockaddr*)&ser, sizeof(ser));
    if(-1 == ret)
        return 0;
    int listenfd = listen(sockfd, 5);
    if(-1 == listenfd)
        return 0;
    //初始化base
    struct event_base* base = event_base_new();

    struct event *even_listen = event_new(base, sockfd, EV_READ|EV_PERSIST, OnAccept, (void*)base);


    //添加事件
    event_add(even_listen, NULL);

    //事件循环
    event_base_dispatch(base);
    return 0;
}
