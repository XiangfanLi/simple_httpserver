//
//  HttpClient.h
//  
//
//  Created by lixiangfan on 2019/12/17.
//

#ifndef _HTTPCLIENT_H_
#define _HTTPCLIENT_H_

#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/time.h>
#include<netdb.h>
#include<sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <memory.h>
#include <ctime>
#include <iostream>
#include <fcntl.h>
#include<netinet/in.h>

using namespace std;

typedef struct client_args{
    char* server_ip;
    int server_port;
    int id;
    int requests;
    bool measure_time;
} client_args_t;




class HttpClient{
	int id; // just for identify client
    int port;
    int sock;
    int valread;
    struct sockaddr_in serv_addr;
    char* hello;
    char* in_buffer; // read message from server
    char* out_buffer; // write message to server
    pthread_t clientReceiveThreadId;
    char* ip;
    bool send_finish;
    int requests;
    bool measure_time;
public:
    HttpClient(const char* _ip, int _port, int _id, int _requests, bool _measure_time);
    void run();
    friend void* client_receive(void* para);
};

void* client_receive(void* para);

void* client_send(void* para);

#endif
