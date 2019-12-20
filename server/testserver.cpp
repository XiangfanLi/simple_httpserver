//
//  testserver.cpp
//  
//
//  Created by lixiangfan on 2019/12/19.
//

#include "httpserver.h"
#define MAX_CLIENTS 10000
#define PORT 8888
#define MAX_THREADS 4
#define MEASURE_TIME true

int main()
{
    HttpServer* server = new HttpServer(MAX_CLIENTS, PORT, MAX_THREADS, MEASURE_TIME);
    server->run();
    return 0;
}
