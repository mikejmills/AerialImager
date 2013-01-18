/* Doc- #################################
#######################################*/

#ifndef THREADPOOL
#define THREADPOOL

using namespace std;
#include <vector>
#include <queue>
#include <pthread.h>
#include <stdio.h>

class Thread_Pool
{
private:
    int num;
    vector<struct thread_data *> *thread_datas;
    pthread_attr_t  thread_attrs;
    
    queue<struct work *> jobs;
    pthread_mutex_t      jobsmtx;
    pthread_cond_t       jobcond;
    
public:    
    Thread_Pool( int thread_count );
    int add( void (*func)(void *ptr), void *data );
};

#endif

