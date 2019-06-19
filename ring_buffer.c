/**@brief 仿照linux kfifo写的ring buffer
 *@atuher Kenny  date:2019-06-18
* ring_buffer.c
 * */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include <ring_buffer.h>

static uint32_t roundUpToPowerOfTwo(uint32_t i) 
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
ring_buffer_return_t ring_buffer_init(ring_buffer_t *ring_buf, uint32_t size)
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
ring_buffer_return_t ring_buffer_deinit(ring_buffer_t *ring_buf)
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

//向缓冲区中存放数据
uint32_t ring_buffer_put(ring_buffer_t *ring_buf, void *buffer, uint32_t size)
{
    assert(ring_buf || buffer);
    
    uint32_t len = 0;
    pthread_mutex_lock(ring_buf->mutex);

	size = min(size, ring_buf->size - ring_buf->in + ring_buf->out);
    /* first put the data starting from fifo->in to buffer end */
    len  = min(size, ring_buf->size - (ring_buf->in & (ring_buf->size - 1)));
    memcpy(ring_buf->buffer + (ring_buf->in & (ring_buf->size - 1)), buffer, len);
    /* then put the rest (if any) at the beginning of the buffer */
    memcpy(ring_buf->buffer, buffer + len, size - len);
    ring_buf->in += size;
	
    pthread_mutex_unlock(ring_buf->mutex);
    return size;
}

//从缓冲区中取数据
uint32_t ring_buffer_get(ring_buffer_t *ring_buf, void *buffer, uint32_t size)
{
    assert(ring_buf || buffer);
    
    uint32_t len = 0;
    pthread_mutex_lock(ring_buf->mutex);

    size  = min(size, ring_buf->in - ring_buf->out);        
    /* first get the data from fifo->out until the end of the buffer */
    len = min(size, ring_buf->size - (ring_buf->out & (ring_buf->size - 1)));
    memcpy(buffer, ring_buf->buffer + (ring_buf->out & (ring_buf->size - 1)), len);
    /* then get the rest (if any) from the beginning of the buffer */
    memcpy(buffer + len, ring_buf->buffer, size - len);
    ring_buf->out += size;
	
    pthread_mutex_unlock(ring_buf->mutex);
    return size;
}

ring_buffer_return_t ring_buffer_clear(ring_buffer_t* ring_buf)
{
    assert(ring_buf);
    pthread_mutex_lock(ring_buf->mutex);
    
    ring_buf->in   = 0;
    ring_buf->out   = 0;

    pthread_mutex_unlock(ring_buf->mutex);
    return RING_BUF_SUCCESS;
}

int32_t ring_buffer_used(const ring_buffer_t *ring_buf)
{
    assert(ring_buf);
	return (ring_buf->in - ring_buf->out);
}

int32_t ring_buffer_available(ring_buffer_t* ring_buf)
{
    assert(ring_buf);
    return (ring_buf->size - ring_buf->in + ring_buf->out);
}