/******************************************************************************
file name: utils_ringbuffer.c
author   : huangkaiqin
usage    : define the data type
history  :
        2019.05.31 : create
******************************************************************************/

/* include ******************************************************************/

/* macro definition *********************************************************/

/* enum definition **********************************************************/

/* struct definition ********************************************************/

/* typedef definition *******************************************************/

/* global variable definition ***********************************************/

/* extern declaration *******************************************************/

/* static variable definition ***********************************************/

/* function definition ******************************************************/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include "as_log.h"
#include "utils_ringbuffer.h"

//判断x是否是2的次方
#define is_power_of_2(x) ((x) != 0 && (((x) & ((x) - 1)) == 0))
//取a和b中最小值
#define min(a, b) (((a) < (b)) ? (a) : (b))

typedef struct 
{
    void             *buffer;     //缓冲区
    as_uint32         size;       //大小
    as_uint32         in;         //入口位置
    as_uint32         out;        //出口位置
    pthread_mutex_t   mutex;    //互斥锁
} ring_buffer_t;

static as_uint32 roundUpToPowerOfTwo(as_uint32 i) 
{
    i--; // If input is a power of two, shift its high-order bit right.

    // "Smear" the high-order bit all the way to the right.
    i |= i >> 1;
    i |= i >> 2;
    i |= i >> 4;
    i |= i >> 8;
    i |= i >> 16;

    return i + 1;
}

//初始化缓冲区
as_int32 utils_ringbuffer_create(as_handle *phandle, as_uint32 size)
{    
    ring_buffer_t *ring_buf = NULL;

    if (NULL == phandle)
    {
        AS_ERROR("The params is null!\n");
        goto ERROR;
    }

    if (!is_power_of_2(size))
    {
        if(size > 0x80000000)                //max size 2G
        {  
            AS_ERROR("Size overflow!\n");
            goto ERROR;
        }
        size = roundUpToPowerOfTwo(size);
    }

    ring_buf = (ring_buffer_t *)malloc(sizeof(ring_buffer_t));
    if(!ring_buf)
    {        
        AS_ERROR("Malloc ring_buf error!\n");
        goto ERROR;
    }
    
    ring_buf->buffer = (void *)malloc(size);
    if (!ring_buf->buffer)
    {
        AS_ERROR("Malloc ring_buf buffer error!\n");
        goto ERROR1;
    }
    memset(ring_buf->buffer, 0, size);

    if(pthread_mutex_init(&ring_buf->mutex, NULL) != 0)
    {
        AS_ERROR("pthread_mutex_init error!\n");
        goto ERROR1;
    }
    ring_buf->size   = size;
    ring_buf->in     = 0;
    ring_buf->out    = 0;
    
    *phandle = (as_handle)ring_buf;
    
    return 0;
    
ERROR1:
    free(ring_buf);
    ring_buf = NULL;
ERROR:
    return -1;
}

//释放缓冲区
as_int32 utils_ringbuffer_destroy(as_handle *phandle)
{
    ring_buffer_t *ring_buf = (ring_buffer_t *)phandle;

    if (ring_buf->buffer)
    {
        free(ring_buf->buffer);
        ring_buf->buffer = NULL;
    }

    pthread_mutex_destroy(&ring_buf->mutex);
    free(ring_buf);
    ring_buf = NULL;
    
    return 0;
}

as_uint32 __ring_buffer_put(ring_buffer_t *ring_buf, void *buffer, as_uint32 size)
{
    as_uint32 len = 0;

    size = min(size, ring_buf->size - ring_buf->in + ring_buf->out);

     __sync_synchronize();
     
    /* first put the data starting from fifo->in to buffer end */
    len = min(size, ring_buf->size - (ring_buf->in & (ring_buf->size - 1)));
    memcpy(ring_buf->buffer + (ring_buf->in & (ring_buf->size - 1)), buffer, len);
    /* then put the rest (if any) at the beginning of the buffer */
    memcpy(ring_buf->buffer, buffer + len, size - len);

     __sync_synchronize();
     
    ring_buf->in += size;

    return size;
}
//向缓冲区中存放数据
as_int32 utils_ringbuffer_set_data(as_handle *phandle, as_char *buffer, as_uint32 size)
{
    ring_buffer_t *ring_buf = (ring_buffer_t *)*phandle;

    as_uint32 ret = 0;
    pthread_mutex_lock(&ring_buf->mutex);
    if( (ring_buf->size - ring_buf->in + ring_buf->out) <= sizeof(size) )
    {
        return 0;
    }
    ret = __ring_buffer_put(ring_buf, &size, sizeof(size));
    if(sizeof(size) == ret)
    {
        ret = __ring_buffer_put(ring_buf, buffer, size);
    }
    else
    {
        ret = 0;
    }

    pthread_mutex_unlock(&ring_buf->mutex);
    return ret;
}

as_uint32 __ring_buffer_get(ring_buffer_t *ring_buf, void *buffer, as_uint32 size)
{    
    as_uint32 len = 0;

    size = min(size, ring_buf->in - ring_buf->out);       
    __sync_synchronize();
    
    /* first get the data from fifo->out until the end of the buffer */
    len = min(size, ring_buf->size - (ring_buf->out & (ring_buf->size - 1)));
    memcpy(buffer, ring_buf->buffer + (ring_buf->out & (ring_buf->size - 1)), len);
    /* then get the rest (if any) from the beginning of the buffer */
    memcpy(buffer + len, ring_buf->buffer, size - len);

    __sync_synchronize();
    
    ring_buf->out += size;

    return size;
}
//从缓冲区中取数据
as_int32 utils_ringbuffer_get_data(as_handle *phandle, as_char *buffer, as_uint32 *buffer_size)
{
    ring_buffer_t *ring_buf = (ring_buffer_t *)*phandle;
    as_uint32 size = *buffer_size;
    as_uint32 ret = 0;
    as_uint32 len = 0;
    pthread_mutex_lock(&ring_buf->mutex);

    ret = __ring_buffer_get(ring_buf, &len, sizeof(size));
    if(sizeof(size) == ret)
    {
        ret = __ring_buffer_get(ring_buf, buffer, (as_uint32)len);
    }
    else
    {
        ret = 0;
    }

    pthread_mutex_unlock(&ring_buf->mutex);
    
    *buffer_size = ret;
    return 0;
}

as_void utils_ring_buffer_clear(as_handle *phandle)
{
    ring_buffer_t *ring_buf = (ring_buffer_t *)*phandle;

    pthread_mutex_lock(&ring_buf->mutex);
    
    ring_buf->in   = 0;
    ring_buf->out   = 0;

    pthread_mutex_unlock(&ring_buf->mutex);
}

as_int32 utils_ring_buffer_used(as_handle *phandle)
{
    ring_buffer_t *ring_buf = (ring_buffer_t *)*phandle;
    return (ring_buf->in - ring_buf->out);
}

as_int32 utils_ring_buffer_available(as_handle *phandle)
{
    ring_buffer_t *ring_buf = (ring_buffer_t *)*phandle;
    return (ring_buf->size - ring_buf->in + ring_buf->out);
}


