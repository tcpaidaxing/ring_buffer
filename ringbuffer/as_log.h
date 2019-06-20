/******************************************************************************
file name: as_log.h
author   : huangkaiqin
usage    : the log
history  :
		 2019.05.17 : create
******************************************************************************/

#ifndef __AS_LOG_H__
#define __AS_LOG_H__


#ifdef __cplusplus
extern "C"
{
#endif

/* include ******************************************************************/

#include <stdio.h>

/* macro definition *********************************************************/

#ifndef NDEBUG
#define AS_DEBUG(fmt, args...)  	           do{printf("Debug:%s[%s:%d]:" fmt, __FILE__, __FUNCTION__, __LINE__, ##args);}while(0)
#define AS_INFO(fmt, args...)		           do{printf("Info%s[%s:%d]:" fmt, __FILE__, __FUNCTION__, __LINE__, ##args);}while(0)
#define AS_NOTICE(fmt, args...)                do{printf("Notice:%s[%s:%d]:" fmt, __FILE__, __FUNCTION__, __LINE__, ##args);}while(0)
#define AS_WARNING(fmt, args...)               do{printf("Warning:%s[%s:%d]:" fmt, __FILE__, __FUNCTION__, __LINE__, ##args);}while(0)
#define AS_ERROR(fmt, args...)  	           do{printf("Error:%s[%s:%d]:" fmt, __FILE__, __FUNCTION__, __LINE__, ##args);}while(0)
#define AS_FATAL(fmt, args...)  	           do{printf("Fatal:%s[%s:%d]:" fmt, __FILE__, __FUNCTION__, __LINE__, ##args);}while(0)
#define AS_MODULE_INFO(module, auth, version)  do { \
printf("\n\n"  \
     "******************************************************************************\n"  \
     "MODULE:%s,AUTH:%s,VER:%s,DATE:%s,TIME:%s.\n"   \
     "******************************************************************************\n"  \
     "\n\n", module, auth, version, __DATE__, __TIME__);  \
}while(0);

#else
#define AS_DEBUG(fmt, args...)  	
#define AS_INFO(fmt, args...) 		
#define AS_NOTICE(fmt, args...)    
#define AS_WARNING(fmt, args...) 	
#define AS_ERROR(fmt, args...)  	
#define AS_FATAL(fmt, args...)
#define AS_MODULE_INFO(module, auth, version)

#endif
/* enum definition **********************************************************/

/* struct definition ********************************************************/

/* typedef definition *******************************************************/

/* global variable definition ***********************************************/

/* extern declaration *******************************************************/

/* static variable definition ***********************************************/

/* function definition ******************************************************/

#ifdef __cplusplus
}
#endif

#endif  //__AS_LOG_H__