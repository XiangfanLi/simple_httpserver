//
//  HttpServer.h
//  
//
//  Created by lixiangfan on 2019/12/17.
//

#ifndef _HTTPSERVER_H_
#define _HTTPSERVER_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <pthread.h>
#include <map>
#include <iostream>
#include <unordered_map>
#include <sys/time.h>
#include <memory.h>
#include <ctime>
#include <fcntl.h>

#include "threadpool.h"
#include "condition.h"





using namespace std;

const char message[] = "HTTP/1.1 200 OK\r\n Content-Type: text/html; charset=utf-8 \r\nContent-Language: zh-CN\r\nServer: WebSphere Application Server/6.1\r\n\r\n <h1>Hello from Server !!!</h1> \r\n";

void* pthread_handle_message(void *arg);

class HttpServer{
    int epoll_fd;	// the epoll file descriptor, for listening events that happened on all the sockets
    int new_socket;
    struct epoll_event master_event;// for storing socket descriptor and listening events of the master socket
    struct epoll_event* client_events;// for storing socket descriptor and listening events of the clients
    int max_clients;
	int max_threads;
    int port;
    int opt;
    int addrlen;
    int valread;
    int sd;
    bool measure_time;
    pthread_attr_t attr;
    fd_set readfds;
    unordered_map<int, int> client_sd_map; // for building indexs for socket descriptor and index in client_events 
    struct sockaddr_in address;
    struct epoll_event* events; // for storing events happened on all the socket
    threadpool_t pool;  
public:
    HttpServer(int _max_clients, int _port, int _max_threads, bool _measure_time);
    void run();
};

#endif
