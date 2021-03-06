#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include "queue_cas.h"

using namespace std;

#define FOR_LOOP_NUM 10000000  //队列push和pop操作函数中for循环的次数

static int push_count;         //队列总共push的次数
static int pop_count;          //队列总共pop的次数

static Queue<int> _queue;

typedef void *(*thread_func_t)(void *arg);

void *queue_push(void *arg)
{
    for(int i = 0; i < FOR_LOOP_NUM; ++i)
    {
        _queue.push(i);
        push_count++;
    }
    return NULL;
}

void *queue_pop(void *arg)
{
    while(true)
    {
        _queue.pop();
        pop_count++;
        
        if(pop_count >= FOR_LOOP_NUM)
            break;
    }
    return NULL;
}

void test_queue(thread_func_t push_func, thread_func_t pop_func, void *arg)
{
    clock_t start = clock();
    
    pthread_t push_tid;
    if(pthread_create(&push_tid, NULL, push_func, arg) != 0)
    {
        perror("pthread_create");
    }

    pthread_t pop_tid;
    if(pthread_create(&pop_tid, NULL, pop_func, arg) != 0)
    {
        perror("pthread_create");
    }

    pthread_join(push_tid, NULL);
    pthread_join(pop_tid, NULL);

    clock_t end = clock();

    printf("spend clock: %ld\n", (end - start) / CLOCKS_PER_SEC);
}

int main()
{
    push_count = 0;
    pop_count = 0;

    test_queue(queue_push, queue_pop, NULL);

    printf("push_count:%d, pop_count:%d\n", push_count, pop_count);
    return 0;
}

