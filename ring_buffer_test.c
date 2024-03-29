/**@brief ring buffer测试程序，?�建两� * ?�产者每?�1秒?�buffer� *@atuher Kenny  date:2019-06-18
 *gcc -o ring_buffer_test ring_buffer_test.c  ring_buffer.c -I./ -lpthread
 * */
#include "ring_buffer.h"
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>

#define BUFFER_SIZE  (1024 * 1024 +1)

#define log(fmt, arg...) printf("LOG:%d " fmt,__LINE__, ##arg)

typedef struct student_info
{
    uint64_t stu_id;
    uint32_t age;
    uint32_t score;
}student_info;

pthread_t consumer_tid;
pthread_t producer_tid;

void print_student_info(const student_info *stu_info)
{
    assert(stu_info);
    log("id:%lu\t",stu_info->stu_id);
    log("age:%u\t",stu_info->age);
    log("score:%u\n",stu_info->score);
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
    ring_buffer_t *ring_buf = (ring_buffer_t *)arg;
    student_info stu_info; 
    while(1)
    {
        sleep(2);
        log("------------------------------------------\n");
        log("get a student info from ring buffer.\n");
        ring_buffer_get(ring_buf, (void *)&stu_info, sizeof(student_info));
        log("ring buffer length: %d------available:%d\n", ring_buffer_used(ring_buf), ring_buffer_available(ring_buf));
        print_student_info(&stu_info);
        log("------------------------------------------\n");
    }
    return (void *)ring_buf;
}

void * producer_proc(void *arg)
{
    time_t cur_time;
    ring_buffer_t *ring_buf = (ring_buffer_t *)arg;
    while(1)
    {
        time(&cur_time);
        srand(cur_time);
        int seed = rand() % 11111;
        log("******************************************\n");
        student_info *stu_info = get_student_info(cur_time + seed);
        log("put a student info to ring buffer.\n");
        ring_buffer_put(ring_buf, (void *)stu_info, sizeof(student_info));
        log("ring buffer length: %d------available:%d\n", ring_buffer_used(ring_buf), ring_buffer_available(ring_buf));
        log("******************************************\n");
        sleep(1);
    }
    return (void *)ring_buf;
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
    uint32_t size = BUFFER_SIZE;
    ring_buffer_t *ring_buf = NULL;
    pthread_t consume_pid, produce_pid;

    ring_buf = (ring_buffer_t *)malloc(sizeof(ring_buffer_t));
    ring_buffer_init(ring_buf, size);

#if 0
    student_info *stu_info = get_student_info(638946124);
    ring_buffer_put(ring_buf, (void *)stu_info, sizeof(student_info));
    stu_info = get_student_info(976686464);
    ring_buffer_put(ring_buf, (void *)stu_info, sizeof(student_info));
    ring_buffer_get(ring_buf, (void *)stu_info, sizeof(student_info));
    print_student_info(stu_info);
#else
    log("multi thread test.......\n");
    produce_pid  = producer_thread((void*)ring_buf);
    consume_pid  = consumer_thread((void*)ring_buf);
    pthread_join(produce_pid, NULL);
    pthread_join(consume_pid, NULL);
    ring_buffer_deinit(ring_buf);

#endif
    return 0;
}
