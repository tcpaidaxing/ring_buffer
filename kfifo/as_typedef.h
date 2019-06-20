/******************************************************************************
file name: as_typedef.h
author   : huangkaiqin
usage    : define the data type
history  :
		 2019.05.17 : create
******************************************************************************/
#ifndef __AS_TYPEDEF_H__
#define __AS_TYPEDEF_H__

#ifdef __cplusplus
extern "C"
{
#endif

/* include ******************************************************************/

/* macro definition *********************************************************/

#define AS_FALSE             (0)
#define AS_TRUE              (1)

/* enum definition **********************************************************/

/* struct definition ********************************************************/

/* typedef definition *******************************************************/

typedef char                 as_char;
typedef signed char          as_int8;
typedef unsigned char        as_uint8;

typedef signed short         as_int16;
typedef unsigned short       as_uint16;

typedef signed int           as_int32;
typedef unsigned int         as_uint32;

typedef signed long          as_long;
typedef unsigned long        as_ulong;

typedef signed long long     as_int64;
typedef unsigned long long   as_uint64;

typedef float                as_float;
typedef double               as_double;
typedef void                 as_void;

typedef signed int           as_bool;
typedef unsigned long        as_handle;

/* global variable definition ***********************************************/

/* extern declaration *******************************************************/

/* static variable definition ***********************************************/

/* function definition ******************************************************/

#ifdef __cplusplus
}
#endif

#endif //__AS_TYPEDEF_H__