/******************************************************************************
file name: utils_fifo.c
author   : huangkaiqin
usage    : the fifo interface
history  :
		 2019.05.17 : create
******************************************************************************/

/* include ******************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <errno.h>
#include "as_log.h"
#include "utils_fifo.h"


/* macro definition *********************************************************/

/* enum definition **********************************************************/

/* struct definition ********************************************************/

/* typedef definition *******************************************************/

/* global variable definition ***********************************************/

/* extern declaration *******************************************************/

/* static variable definition ***********************************************/

/* function definition ******************************************************/

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
fifo_t *utils_fifo_init(as_uint32 size)
{
    fifo_t *fifo;

    if (!is_power_of_2(size))
    {
        if(size > 0x80000000)                //max size 2G
            return NULL;
        size = roundUpToPowerOfTwo(size);
    }

    fifo = (fifo_t *)malloc(sizeof(fifo_t));
    if(!fifo)
    {
        return NULL;
    }
    
    fifo->buffer = (as_void *)malloc(size);
    if (!fifo)
    {
        free(fifo);
        return NULL;
    }
    memset(fifo->buffer, 0, size);

    fifo->size   = size;
    fifo->in     = 0;
    fifo->out    = 0;
    
    return fifo;
}
//释放缓冲区
as_void utils_fifo_deinit(fifo_t *fifo)
{    
    if(fifo)
    {
        if (fifo->buffer)
        {
            free(fifo->buffer);
            fifo->buffer = NULL;
        }
        free(fifo);
        fifo = NULL;
    }
}

//向缓冲区中存放数据
as_uint32 utils_fifo_put(fifo_t *fifo, void *buffer, as_uint32 size)
{
    assert(fifo || buffer);
    
    as_uint32 len = 0;

    size = min(size, fifo->size - fifo->in + fifo->out);

    /*
    * Ensure that we sample the fifo->out index -before- we
    * start putting bytes into the fifo.
    */
    __sync_synchronize();
    
    /* first put the data starting from fifo->in to buffer end */
    len  = min(size, fifo->size - (fifo->in & (fifo->size - 1)));
    memcpy(fifo->buffer + (fifo->in & (fifo->size - 1)), buffer, len);
    /* then put the rest (if any) at the beginning of the buffer */
    memcpy(fifo->buffer, buffer + len, size - len);

    /*
    * Ensure that we add the bytes to the kfifo -before-
    * we update the fifo->in index.
    */
    __sync_synchronize();
    
    fifo->in += size;

    return size;
}

//从缓冲区中取数据
as_uint32 utils_fifo_get(fifo_t *fifo, void *buffer, as_uint32 size)
{
    assert(fifo || buffer);
    
    as_uint32 len = 0;

    size  = min(size, fifo->in - fifo->out);

    /*
    * Ensure that we sample the fifo->in index -before- we
    * start removing bytes from the fifo.
    */
    __sync_synchronize();
  
    /* first get the data from fifo->out until the end of the buffer */
    len = min(size, fifo->size - (fifo->out & (fifo->size - 1)));
    memcpy(buffer, fifo->buffer + (fifo->out & (fifo->size - 1)), len);
    /* then get the rest (if any) from the beginning of the buffer */
    memcpy(buffer + len, fifo->buffer, size - len);

    /*
    * Ensure that we remove the bytes from the kfifo -before-
    * we update the fifo->out index.
    */
    __sync_synchronize();
  
    fifo->out += size;

    return size;
}

as_void utils_fifo_clear(fifo_t* fifo)
{
    assert(fifo);
    
    fifo->in   = 0;
    fifo->out   = 0;
}

as_int32 utils_fifo_used(const fifo_t *fifo)
{
    assert(fifo);
    return (fifo->in - fifo->out);
}

as_int32 utils_fifo_available(fifo_t* fifo)
{
    assert(fifo);
    return (fifo->size - fifo->in + fifo->out);
}


