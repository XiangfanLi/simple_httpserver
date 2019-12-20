//
//  httpclient.cpp
//  
//
//  Created by lixiangfan on 2019/12/17.
//


#include "httpclient.h"


HttpClient::HttpClient(const char* _ip, int _port, int _id, int _requests, bool _measure_time)
{
	ip = new char[30];
	strcpy(ip, _ip);
	port = _port;
	id = _id;
	requests = _requests;
	measure_time = _measure_time;
	in_buffer = new char[1025];
	out_buffer = new char[1025];
}

void HttpClient::run()
{
    struct hostent *ptrh;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
	 printf("\n Socket creation error \n");
	 return;
	}
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);
      

	ptrh = gethostbyname(ip);
	memcpy(&serv_addr.sin_addr,ptrh->h_addr,ptrh->h_length);
	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
	 printf("\nConnection Failed \n");
	 return;
	}
	int flags = fcntl(sock, F_GETFL, 0);
	fcntl(sock, F_SETFL, flags | O_NONBLOCK);
	send_finish = false;
	int ret = pthread_create(&clientReceiveThreadId, 0, client_receive, (void*)this);
	if (ret != 0)
	{
		perror("pthread create failed!");
		return;
	}
	string message = "GET http://lixiangfan.com/httpserver HTTP/1.1\r\nAccept: */*\r\nAccept-Language: zh-CN\r\nUser-Agent: Mozilla/4.0\r\nAccept-Encoding: gzip, deflate\r\nConnection: Keep-Alive\r\nHost: localhost:8888\r\n\r\n<h1>Hello from client!!!" + to_string(id) + "</h1> \r\n";

	strncpy(out_buffer, message.c_str(), 1024);
	for(int i = 0; i < requests; ++i)
	{
	sprintf(out_buffer, "%-1024s", out_buffer);
	send(sock , out_buffer , strlen(out_buffer) , 0 );
	}
	send_finish = true;
	pthread_join(clientReceiveThreadId, NULL);
}


void* client_receive(void* para)
{
	clock_t last_recv_time, now; // for judging when to exit the receive thread
	HttpClient* client = (HttpClient*)para;
	int valread;
	while(true)
	{
		if(client->send_finish)
			now = clock();
		if(client->send_finish && (double)(now-last_recv_time)/CLOCKS_PER_SEC > 0.5)	
		{
			break;
		}
		
		valread = recv( client->sock , client->in_buffer, 1024, MSG_DONTWAIT);
		if(valread > 0)
		{
			if(client->send_finish)
			last_recv_time = clock();
			if(!client->measure_time)
				printf("client receive message: %s\n", client->in_buffer);
		}
	}
	return NULL;
}

void* client_send(void* para)
{
    client_args_t* args = (client_args_t*)para;
    int id = args->id;
    int requests = args->requests;
    const char* server_ip = args->server_ip;
    int server_port = args->server_port;
    bool measure_time = args->measure_time;
	HttpClient* client = new HttpClient(server_ip, server_port, id, requests, measure_time);
	client->run();
	return NULL;
}


