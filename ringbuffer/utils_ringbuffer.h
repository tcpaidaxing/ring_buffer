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
    
as_int32 utils_ringbuffer_create(as_handle *phandle, as_uint32 size);
as_int32 utils_ringbuffer_destroy(as_handle *phandle);
as_int32 utils_ringbuffer_get_data(as_handle *phandle, as_char *buffer, as_uint32 *size);
as_int32 utils_ringbuffer_set_data(as_handle *phandle, as_char *buffer, as_uint32 size);
as_void utils_ring_buffer_clear(as_handle *phandle);
as_int32 utils_ring_buffer_used(as_handle *phandle);
as_int32 utils_ring_buffer_available(as_handle *phandle);


#ifdef __cplusplus
}
#endif

#endif //__UTILS_RINGBUFFER_H__

