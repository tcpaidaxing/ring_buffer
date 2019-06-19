/**@brief 仿照linux kfifo写的ring buffer
 *@atuher Kenny  date:2019-06-18
* ring_buffer.h
 * */

#ifndef __KFIFO_HEADER_H__
#define __KFIFO_HEADER_H__

#include <inttypes.h>
#include <pthread.h>

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
    uint32_t         size;       //大小
    uint32_t         in;         //入口位置
    uint32_t         out;        //出口位置
    pthread_mutex_t  *mutex;    //互斥锁
} ring_buffer_t;


ring_buffer_return_t ring_buffer_init(ring_buffer_t *ring_buf, uint32_t size);
ring_buffer_return_t ring_buffer_deinit(ring_buffer_t *ring_buf);
ring_buffer_return_t ring_buffer_clear(ring_buffer_t* ring_buf);
uint32_t ring_buffer_put(ring_buffer_t *ring_buf, void *buffer, uint32_t size);
uint32_t ring_buffer_get(ring_buffer_t *ring_buf, void *buffer, uint32_t size);
int32_t ring_buffer_used(const ring_buffer_t *ring_buf);
int32_t ring_buffer_available(ring_buffer_t* ring_buf);

#endif  //__KFIFO_HEADER_H__