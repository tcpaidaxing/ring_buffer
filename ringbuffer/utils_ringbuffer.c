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
#include <utils_ringbuffer.h>

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
ring_buffer_return_t utils_ring_buffer_init(ring_buffer_t *ring_buf, as_uint32 size)
{
    assert(ring_buf);
    
    if (!is_power_of_2(size))
    {
        if(size > 0x80000000)                //max size 2G
            return RING_BUF_ERR_SIZE;
        size = roundUpToPowerOfTwo(size);
    }
    ring_buf->buffer = (void *)malloc(size);
    if (!ring_buf)
    {
        fprintf(stderr,"Failed to malloc memory,errno:%u,reason:%s", errno, strerror(errno));
        return RING_BUF_ERR_MALLOC;
    }
    memset(ring_buf->buffer, 0, size);
    ring_buf->mutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
    if(pthread_mutex_init(ring_buf->mutex, NULL) != 0)
    {
        fprintf(stderr,"Failed to malloc memory,errno:%u,reason:%s", errno, strerror(errno));
        return RING_BUF_ERR_SYNC;
    }
    ring_buf->size   = size;
    ring_buf->in     = 0;
    ring_buf->out    = 0;
    
    return RING_BUF_SUCCESS;
}
//释放缓冲区
ring_buffer_return_t utils_ring_buffer_deinit(ring_buffer_t *ring_buf)
{
    assert(ring_buf);
    
    if (ring_buf->buffer)
    {
        free(ring_buf->buffer);
        ring_buf->buffer = NULL;
    }
    if(ring_buf->mutex) 
    {
        pthread_mutex_destroy(ring_buf->mutex);
        free(ring_buf->mutex);
        ring_buf->mutex = NULL;
    }
    return RING_BUF_SUCCESS;
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
as_uint32 utils_ring_buffer_put(ring_buffer_t *ring_buf, void *buffer, as_uint32 size)
{
    assert(ring_buf || buffer);
    
    as_uint32 ret;
    pthread_mutex_lock(ring_buf->mutex);

    ret = __ring_buffer_put(ring_buf, buffer, size);

    pthread_mutex_unlock(ring_buf->mutex);
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
as_uint32 utils_ring_buffer_get(ring_buffer_t *ring_buf, void *buffer, as_uint32 size)
{
    assert(ring_buf || buffer);
    
    as_uint32 ret = 0;
    pthread_mutex_lock(ring_buf->mutex);

    ret = __ring_buffer_get(ring_buf, buffer, size);
        
    /* 当 ring_buf->in == ring_buf->out时，buffer为空 */
    if(ring_buf->in == ring_buf->out)
    {
        utils_ring_buffer_clear(ring_buf);
    }

    pthread_mutex_unlock(ring_buf->mutex);
    
    return ret;
}

ring_buffer_return_t utils_ring_buffer_clear(ring_buffer_t* ring_buf)
{
    assert(ring_buf);
    pthread_mutex_lock(ring_buf->mutex);
    
    ring_buf->in   = 0;
    ring_buf->out   = 0;

    pthread_mutex_unlock(ring_buf->mutex);
    return RING_BUF_SUCCESS;
}

as_int32 utils_ring_buffer_used(const ring_buffer_t *ring_buf)
{
    assert(ring_buf);
    return (ring_buf->in - ring_buf->out);
}

as_int32 utils_ring_buffer_available(ring_buffer_t* ring_buf)
{
    assert(ring_buf);
    return (ring_buf->size - ring_buf->in + ring_buf->out);
}


