//
//  HttpServer.cpp
//  
//
//  Created by lixiangfan on 2019/12/17.
//

#include "httpserver.h"


void* pthread_handle_message(void *arg)
{
    char* in_buffer = in_buffer_map[(int)pthread_self()];
    char* out_buffer = out_buffer_map[(int)pthread_self()];
    int sd = ((int*)arg)[0];
    int measure_time = ((int*)arg)[1];
    int valread = recv( sd , in_buffer, 1024, MSG_DONTWAIT);
	if(valread > 0)
	{
		//set the string terminating NULL byte on the end
		//of the data read
		in_buffer[valread] = '\0';
		if(!measure_time)
			printf("receive message: \n%s\n", in_buffer);
		sprintf(out_buffer,"%-1024s", message); // pad the message to 1024 bytes to avoid sticky packets
		send(sd , out_buffer , strlen(out_buffer) , MSG_DONTWAIT);
	}
    free(arg);
    return NULL;
}

HttpServer::HttpServer(int _max_clients, int _port, int _max_threads, bool _measure_time)
{
    max_clients = _max_clients;
    port = _port;
    max_threads = _max_threads;
    measure_time = _measure_time;
    opt = true;
    pthread_attr_init(&attr); 
    //pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM); 
    /* 设置线程为分离属性*/ 
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    client_events = (struct epoll_event*)malloc(sizeof(struct epoll_event)*max_clients);
	events = (struct epoll_event*)malloc(sizeof(struct epoll_event)*max_clients);
    threadpool_init(&pool, max_threads);
}

void HttpServer::run()
{
    int epoll_fd = epoll_create1(0);
    if(epoll_fd == -1)
	{
		fprintf(stderr, "Failed to create epoll file descriptor\n");
		return;
	}
		
        
    for(int i = 0; i < max_clients; ++i)
    {
        client_events[i].data.fd = -1;
    }
    //create a master socket
    master_event.events = EPOLLIN;
    if( (master_event.data.fd = socket(AF_INET , SOCK_STREAM , 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    //set master socket to allow multiple connections ,
    //this is just a good habit, it will work without this
    if( setsockopt(master_event.data.fd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,
        sizeof(opt)) < 0 )
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    //type of socket created
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    //bind the socket to localhost port 8888
    if (bind(master_event.data.fd, (struct sockaddr *)&address, sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    printf("Listener on port %d \n", port);
    //try to specify maximum of 10000 pending connections for the master socket
    if (listen(master_event.data.fd, max_clients) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, master_event.data.fd, &master_event))
	{
		fprintf(stderr, "Failed to add file descriptor to epoll\n");
		close(epoll_fd);
		return;
	}
    
    
    //accept the incoming connection
    addrlen = sizeof(address);
    puts("Waiting for connections ...");
    while(true)
    {
        
        int event_num = epoll_wait(epoll_fd, events, max_clients, 0);
        if(event_num < 0)
        {
        	perror("epoll_wait error");
        	return;
        }
        else if(event_num == 0)
        {
        	continue;
        }
        for(int i = 0; i < event_num; ++i)
        {
        	int event_sd = events[i].data.fd;
        	if(event_sd == master_event.data.fd)
        	{
        		if ((new_socket = accept(event_sd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
	            {
	                perror("accept");
	                exit(EXIT_FAILURE);
	            } 
	            int flags = fcntl(new_socket, F_GETFL, 0);
  	 			fcntl(new_socket, F_SETFL, flags | O_NONBLOCK);
	            
        		printf("New connection , socket fd is %d , ip is : %s , port : %d \n" , new_socket , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
           
	            //add new socket to array of sockets
	            for (int j = 0; j < max_clients; j++)
	            {
	                //if position is empty
	                if( client_events[j].data.fd == -1 )
	                {
	                    client_events[j].data.fd = new_socket;
	                    client_events[j].events = EPOLLIN|EPOLLRDHUP; // listen read and hang up events 
	                    client_sd_map[new_socket] = j;
	                    if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, new_socket, &client_events[j]))
						{
							fprintf(stderr, "Failed to add file descriptor to epoll\n");
							close(epoll_fd);
							return;
						}
	                    printf("Adding to list of sockets as %d\n" , j);
	                    break;
	                }
	            } 
        	}
        	
        	else{
	            if(events[i].events & EPOLLRDHUP) // some client disconnect
	            {
	            	printf("disconnected\n");
	            	getpeername(event_sd , (struct sockaddr*)&address , (socklen_t*)&addrlen);
					printf("Host disconnected , ip %s , port %d \n", inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
					//Close the socket and mark as -1 in list for reuse
					close(event_sd);
					client_events[client_sd_map[event_sd]].data.fd = -1;
	            }
	            else
	            {
	            	int* arg = (int*)malloc(sizeof(int)*2);
					arg[0] = event_sd;
					arg[1] = measure_time;
					threadpool_add_task(&pool, pthread_handle_message, arg); // ready to read from some client, add this event to threadpool 	
	            }
        	}        
        } 
    }
}


