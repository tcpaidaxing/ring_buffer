/**@brief ring buffer测试程序，?�建两� * ?�产?�每?�?秒?�?uffer�?
 *@atuher Kenny  date:2019-06-18
 *gcc -o ring_buffer_test ring_buffer_test.c  ring_buffer.c -I./ -lpthread
 * */
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include "utils_ringbuffer.h"
#include "as_log.h"

#define BUFFER_SIZE  (1024 * 1024 +1)
as_handle ring_buf = 0;
pthread_t consumer_tid;
pthread_t producer_tid;


as_void * consumer_proc(as_void *arg)
{
    as_handle consumer_ringbuf = (as_handle)arg;
    
    as_char data[100] = {'0'};
    as_uint32 size = 0;
    while(1)
    {
        sleep(2);
        AS_DEBUG("-------------------------------------------------------------------------\n");
        memset(data, 0, 100);
        utils_ringbuffer_get_data(&consumer_ringbuf, (as_char *)data, &size);
        AS_DEBUG("GET:%d, ring buffer length: %d  available:%d\n",size, utils_ring_buffer_used(&consumer_ringbuf), utils_ring_buffer_available(&consumer_ringbuf));
        AS_DEBUG("data:%s\n",data);
        AS_DEBUG("-------------------------------------------------------------------------\n");
    }
    return NULL;
}

as_void * producer_proc(as_void *arg)
{
    as_handle producer_ringbuf = (as_handle)arg;
    as_uint32 ret = 0;
    as_char *data = "Hello World!12345678901234567#";
    while(1)
    {
        AS_DEBUG("******************************************strlen(data):%ld***data:%s\n",strlen(data),data);
        
        ret = utils_ringbuffer_set_data(&producer_ringbuf, (as_char *)data, strlen(data));
        AS_DEBUG("PUT:%d, ring buffer length: %d  available:%d\n", ret, utils_ring_buffer_used(&producer_ringbuf), utils_ring_buffer_available(&producer_ringbuf));
        AS_DEBUG("******************************************\n");
        sleep(1);
    }
    return NULL;
}

as_void * producer_proc1(as_void *arg)
{
    as_handle producer_ringbuf = (as_handle)arg;
    
    as_uint32 ret = 0;
    as_char *data = "abscefgsdedghdfeadfabcdefghijkidhnfdlabscdabcdefghijklmnopqrstuvwxxyz!";
    while(1)
    {
        AS_DEBUG("###########################################strlen(data):%ld###data:%s\n",strlen(data),data);

        ret = utils_ringbuffer_set_data(&producer_ringbuf, (as_char *)data,  strlen(data));
        AS_DEBUG("PUT:%d, ring buffer length: %d  available:%d\n", ret, utils_ring_buffer_used(&producer_ringbuf), utils_ring_buffer_available(&producer_ringbuf));
        AS_DEBUG("###########################################\n");
        sleep(3);
    }
    return NULL;
}

pthread_t consumer_thread(as_void *arg)
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
pthread_t producer_thread(as_void *arg)
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
pthread_t producer_thread1(as_void *arg)
{
    int err;
   
    err = pthread_create(&producer_tid, NULL, producer_proc1, arg);
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
    pthread_t consume_pid, produce_pid, produce_pid1;

    utils_ringbuffer_create(&ring_buf, size);

    AS_DEBUG("multi thread test.......\n");
    produce_pid  = producer_thread((as_void *)ring_buf);
    produce_pid1  = producer_thread1((as_void *)ring_buf);
    consume_pid  = consumer_thread((as_void *)ring_buf);
    pthread_join(produce_pid, NULL);
    pthread_join(produce_pid1, NULL);
    pthread_join(consume_pid, NULL);
    utils_ringbuffer_destroy(&ring_buf);

    return 0;
}
