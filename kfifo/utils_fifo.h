/******************************************************************************
file name: utils_fifo.h
author   : huangkaiqin
usage    : the fifo interface
history  :
		 2019.05.17 : create
******************************************************************************/

#ifndef __UTILS_FIFO_H__
#define __UTILS_FIFO_H__

#ifdef __cplusplus
extern "C"
{
#endif

/* include ******************************************************************/

#include "as_typedef.h"

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

typedef struct 
{
    void             *buffer;     //缓冲区
    as_uint32         size;       //大小
    as_uint32         in;         //入口位置
    as_uint32         out;        //出口位置
} fifo_t;
    
    
fifo_t *utils_fifo_init(as_uint32 size);
as_void utils_fifo_deinit(fifo_t *ufifo);
as_void utils_fifo_clear(fifo_t* ufifo);
as_uint32 utils_fifo_put(fifo_t *ufifo, void *buffer, as_uint32 size);
as_uint32 utils_fifo_get(fifo_t *ufifo, void *buffer, as_uint32 size);
as_int32 utils_fifo_used(const fifo_t *ufifo);
as_int32 utils_fifo_available(fifo_t* ufifo);

#ifdef __cplusplus
}
#endif

#endif //__BSL_RECORDER_H__


