//
//  testclient.cpp
//  
//
//  Created by lixiangfan on 2019/12/19.
//


#include "httpclient.h"


#define CONCURRENT_CLIENTS 4
#define REQUESTS_PER_CLIENT 10000000
#define SERVER_IP "183.173.62.240"
#define SERVER_PORT 8888
#define MEASURE_TIME true




struct timeval tv_begin, tv_end; // just for measuring programme time
pthread_t threadIds[CONCURRENT_CLIENTS];
int main()
{
	gettimeofday(&tv_begin, NULL);
    for(int i = 0; i < CONCURRENT_CLIENTS; ++i)
    {
    	client_args_t* arg = (client_args_t*)malloc(sizeof(client_args_t));
        arg->id = i;
        arg->server_port = SERVER_PORT;
        arg->requests = REQUESTS_PER_CLIENT;
        arg->server_ip = new char[30];
        strcpy(arg->server_ip, SERVER_IP);
        arg->measure_time = MEASURE_TIME;
    
    	int ret = pthread_create(&threadIds[i], 0, client_send, arg);
	if (ret != 0)
        {
            perror("pthread create failed!");
            return(-1);
        }
    }
    for(int i = 0; i < CONCURRENT_CLIENTS; ++i)
    {
    	pthread_join(threadIds[i], NULL);
    }
	gettimeofday(&tv_end, NULL);
    double totaltime = double(tv_end.tv_sec - tv_begin.tv_sec)*1000 + double(tv_end.tv_usec - tv_begin.tv_usec)/1000;
	cout<<"Total request: "<<CONCURRENT_CLIENTS*REQUESTS_PER_CLIENT<<", Total time:"<<totaltime<<"ms"<<endl;	//ms为单位
    printf("clients finish");
    return 0;
}
