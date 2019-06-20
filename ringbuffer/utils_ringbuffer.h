/******************************************************************************
file name: utils_ringbuffer.h
author   : huangkaiqin
usage    : define the data type
history  :
		 2019.05.31 : create
******************************************************************************/

#ifndef __UTILS_RINGBUFFER_H__
#define __UTILS_RINGBUFFER_H__

#ifdef __cplusplus
extern "C"
{
#endif

/* include ******************************************************************/

#include "as_typedef.h"
#include <pthread.h>

/* macro definition *********************************************************/

/* enum definition **********************************************************/

/* struct definition ********************************************************/

/* typedef definition *******************************************************/

/* global variable definition ***********************************************/

/* extern declaration *******************************************************/

/* static variable definition ***********************************************/

/* function definition ******************************************************/

//判断x是否是2的次方
#define is_power_of_2(x) ((x) != 0 && (((x) & ((x) - 1)) == 0))
//取a和b中最小值
#define min(a, b) (((a) < (b)) ? (a) : (b))
    
typedef enum {
    RING_BUF_SUCCESS = 0,
    RING_BUF_ERR_EMPTY,
    RING_BUF_ERR_MALLOC,
    RING_BUF_ERR_SIZE,
    RING_BUF_ERR_SYNC,  
} ring_buffer_return_t;

typedef struct 
{
    void             *buffer;     //缓冲区
    as_uint32         size;       //大小
    as_uint32         in;         //入口位置
    as_uint32         out;        //出口位置
    pthread_mutex_t  *mutex;    //互斥锁
} ring_buffer_t;
    
    
ring_buffer_return_t utils_ring_buffer_init(ring_buffer_t *ring_buf, as_uint32 size);
ring_buffer_return_t utils_ring_buffer_deinit(ring_buffer_t *ring_buf);
ring_buffer_return_t utils_ring_buffer_clear(ring_buffer_t* ring_buf);
as_uint32 utils_ring_buffer_put(ring_buffer_t *ring_buf, void *buffer, as_uint32 size);
as_uint32 utils_ring_buffer_get(ring_buffer_t *ring_buf, void *buffer, as_uint32 size);
as_int32 utils_ring_buffer_used(const ring_buffer_t *ring_buf);
as_int32 utils_ring_buffer_available(ring_buffer_t* ring_buf);

#ifdef __cplusplus
}
#endif

#endif //__UTILS_RINGBUFFER_H__

