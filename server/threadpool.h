//
//  threadpool.h
//  
//
//  Created by lixiangfan on 2019/12/17.
//


#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_


#include <map>
#include "condition.h"

using namespace std;

//map the thread id with their in/out buffers
static map<int, char*> in_buffer_map;
static map<int, char*> out_buffer_map;

// task object that threads need to execute
typedef struct task
{
    void *(*run)(void *args);  // function pointer need to run
    void *arg;              // arguments 
    struct task *next;      // next task in task queue
}task_t;



typedef struct threadpool
{
    condition_t ready;    // condition variable
    task_t *first;       // first task in task queue
    task_t *last;        // last task in task queue
    int counter;         // active threads number in threadpool
    int idle;            // idle threads number in threadpool
    int max_threads;     // max threads number
    int quit;            // quit symbol
}threadpool_t;


//init threadpool
void threadpool_init(threadpool_t *pool, int threads);

//add task to threadpool
void threadpool_add_task(threadpool_t *pool, void *(*run)(void *arg), void *arg);

//destroy threadpool
void threadpool_destroy(threadpool_t *pool);

#endif
