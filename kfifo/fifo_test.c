/**@brief ring bufferÊµãËØïÁ®ãÂ∫èÔºå?õÂª∫‰∏§‰∏ * ?ü‰∫ßËÄÖÊØè?î1Áßí?ëbuffer‰∏ *@atuher Kenny  date:2019-06-18
 *gcc -o fifo_test fifo_test.c  fifo.c -I./ -lpthread
 * */
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include "utils_fifo.h"
#include "as_log.h"

#define BUFFER_SIZE  (1024 * 1024 +1)

typedef struct student_info
{
    as_uint64 stu_id;
    as_uint32 age;
    as_uint32 score;
}student_info;

pthread_t consumer_tid;
pthread_t producer_tid;

void print_student_info(const student_info *stu_info)
{
    assert(stu_info);
    AS_DEBUG("id:%lu\t",stu_info->stu_id);
    AS_DEBUG("age:%u\t",stu_info->age);
    AS_DEBUG("score:%u\n",stu_info->score);
}

student_info * get_student_info(time_t timer)
{
    student_info *stu_info = (student_info *)malloc(sizeof(student_info));
    if (!stu_info)
    {
        fprintf(stderr, "Failed to malloc memory.\n");
        return NULL;
    }
    srand(timer);
    stu_info->stu_id = 10000 + rand() % 9999;
    stu_info->age = rand() % 30;
    stu_info->score = rand() % 101;
    print_student_info(stu_info);
    return stu_info;
}

void * consumer_proc(void *arg)
{
    fifo_t *ufifo = (fifo_t *)arg;
    student_info stu_info; 
    while(1)
    {
        sleep(2);
        AS_DEBUG("------------------------------------------\n");
        AS_DEBUG("get a student info from ring buffer.\n");
        utils_fifo_get(ufifo, (void *)&stu_info, sizeof(student_info));
        AS_DEBUG("ring buffer length: %d------available:%d\n", utils_fifo_used(ufifo), utils_fifo_available(ufifo));
        print_student_info(&stu_info);
        AS_DEBUG("------------------------------------------\n");
    }
    return (void *)ufifo;
}

void * producer_proc(void *arg)
{
    time_t cur_time;
    fifo_t *ufifo = (fifo_t *)arg;
    while(1)
    {
        time(&cur_time);
        srand(cur_time);
        int seed = rand() % 11111;
        AS_DEBUG("******************************************\n");
        student_info *stu_info = get_student_info(cur_time + seed);
        AS_DEBUG("put a student info to ring buffer.\n");
        utils_fifo_put(ufifo, (void *)stu_info, sizeof(student_info));
        AS_DEBUG("ring buffer length: %d------available:%d\n", utils_fifo_used(ufifo), utils_fifo_available(ufifo));
        AS_DEBUG("******************************************\n");
        sleep(1);
    }
    return (void *)ufifo;
}

pthread_t consumer_thread(void *arg)
{
    int err;
    
    err = pthread_create(&consumer_tid, NULL, consumer_proc, arg);
    if (err != 0)
    {
        fprintf(stderr, "Failed to create consumer thread.errno:%u, reason:%s\n",
        errno, strerror(errno));
        return -1;
    }
    return consumer_tid;
}
pthread_t producer_thread(void *arg)
{
    int err;
   
    err = pthread_create(&producer_tid, NULL, producer_proc, arg);
    if (err != 0)
    {
        fprintf(stderr, "Failed to create consumer thread.errno:%u, reason:%s\n",
        errno, strerror(errno));
        return -1;
    }
    return producer_tid;
}


int main()
{
    as_uint32 size = BUFFER_SIZE;
    fifo_t *ufifo = NULL;
    pthread_t consume_pid, produce_pid;

    ufifo = utils_fifo_init(size);

#if 0
    student_info *stu_info = get_student_info(638946124);
    fifo_put(ufifo, (void *)stu_info, sizeof(student_info));
    stu_info = get_student_info(976686464);
    fifo_put(ufifo, (void *)stu_info, sizeof(student_info));
    fifo_get(ufifo, (void *)stu_info, sizeof(student_info));
    print_student_info(stu_info);
#else
    AS_DEBUG("multi thread test.......\n");
    produce_pid  = producer_thread((void*)ufifo);
    consume_pid  = consumer_thread((void*)ufifo);
    pthread_join(produce_pid, NULL);
    pthread_join(consume_pid, NULL);
    utils_fifo_deinit(ufifo);

#endif
    return 0;
}
