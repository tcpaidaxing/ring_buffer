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
as_int32 utils_fifo_create(as_handle *fifo_handler, as_uint32 size);
as_int32 utils_fifo_delete(as_handle fifo_handler);
as_uint32 utils_fifo_push(as_handle fifo_handler, as_uint8 *buffer, as_uint32 size);
as_uint32 utils_fifo_pop(as_handle fifo_handler, as_uint8 *buffer, as_uint32 size);
as_void utils_fifo_clear(as_handle fifo_handler);
as_int32 utils_fifo_used(as_handle fifo_handler);
as_int32 utils_fifo_available(as_handle fifo_handler);


#ifdef __cplusplus
}
#endif

#endif //__BSL_RECORDER_H__


