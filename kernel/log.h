#ifndef __LOG_H_
#define __LOG_H_

#include "st_uart.h"
#include "usart.h"

#define SHELL_USART					huart1
#define SHELL_Transmit(src, len)	HAL_UART_Transmit(&SHELL_USART, src, len, 100)
#define CONSOLE_COLOR_ENABLE 		1


#define LOG_LEVEL_DEBUG 	1
#define LOG_LEVEL_INFO 		2
#define LOG_LEVEL_WARNING 	3
#define LOG_LEVEL_ERROR 	4
#define LOG_LEVEL_FATAL 	5

#define LOG_LEVEL	LOG_LEVEL_DEBUG

#define CONSOLE_COLOR_BLACK  "\033[30m"
#define CONSOLE_COLOR_RED  "\033[31m"
#define CONSOLE_COLOR_GREEN  "\033[32m"
#define CONSOLE_COLOR_YELLOW  "\033[33m"
#define CONSOLE_COLOR_BLUE  "\033[34m"
#define CONSOLE_COLOR_PURPLE  "\033[35m"
#define CONSOLE_COLOR_DEEP  "\033[36m"
#define CONSOLE_COLOR_WHITE  "\033[37m"

#define SHELL_DELAY osDelay

#if CONSOLE_COLOR_ENABLE == 1

//设置控制台字体颜色
#define SET_CONSOLE_COLOR(__color__) SHELL_Transmit((uint8_t*)__color__, strlen(__color__))
//恢复控制台字体颜色
#define RESUME_CONSOLE_COLOR() SET_CONSOLE_COLOR(CONSOLE_COLOR_WHITE)

#else

#define SET_CONSOLE_COLOR(__color__) 
#define RESUME_CONSOLE_COLOR()

#endif

typedef enum{
	LOG_DEBUG = 1,
	LOG_INFO,
	LOG_WARNING,
	LOG_ERROR,
	LOG_FATAL
}_LOG_LEVEL;

/**
 * @brief 	shell发送日志信息
 * 
 * @param level 日志等级
 * @param fmt  调试信息格式
 * @param ...  可变参数
 */
void SHELL_LOG_LEVEL(_LOG_LEVEL level, const char* fmt, ...);

/**
 * @brief 	shell发送数据
 * 
 * @param fmt 	数据格式
 * @param ...  	可变参数
 */
void SHELL_PRINTF(const char* fmt, ...);


#if LOG_LEVEL <= LOG_LEVEL_DEBUG
#define	LOG_DEBUG(__fmt__, ...) SHELL_LOG_LEVEL(LOG_DEBUG, __fmt__, ##__VA_ARGS__)
#else
#define	LOG_DEBUG(...)
#endif

#if LOG_LEVEL <= LOG_LEVEL_INFO
#define	LOG_INFO(__fmt__, ...) SHELL_LOG_LEVEL(LOG_INFO, __fmt__, ##__VA_ARGS__)
#else
#define	LOG_INFO(__VA_ARGS__)
#endif

#if LOG_LEVEL <= LOG_LEVEL_WARNING
#define	LOG_WARNING(__fmt__, ...) SHELL_LOG_LEVEL(LOG_WARNING, __fmt__, ##__VA_ARGS__)
#else
#define	LOG_WARNING(__VA_ARGS__)
#endif

#if LOG_LEVEL <= LOG_LEVEL_ERROR
#define	LOG_ERROR(__fmt__, ...) SHELL_LOG_LEVEL(LOG_ERROR, __fmt__, ##__VA_ARGS__)
#else
#define	LOG_ERROR(__VA_ARGS__)
#endif

#if LOG_LEVEL <= LOG_LEVEL_FATAL
#define	LOG_FATAL(__fmt__, ...) SHELL_LOG_LEVEL(LOG_FATAL, __fmt__, ##__VA_ARGS__)
#else
#define	LOG_FATAL(__VA_ARGS__)
#endif

#define LOG_ASSERT(__cond__, __then__, __fmt__, ...)	\
	do {						\
		if (__cond__) {			\
			LOG_ERROR("<%s:%d>" __fmt__, __FILE__, __LINE__, ##__VA_ARGS__);			\
			{ __then__; }	\
		}						\
	} while(0)

#define RETURN_ASSERT(__cond__, __fmt__, ...)	\
	LOG_ASSERT(__cond__, return, __fmt__, ##__VA_ARGS__)

#define CRASH_ASSERT(__cond__, __fmt__, ...)	\
	LOG_ASSERT(__cond__, volatile int i = 1 / 0;, __fmt__, ##__VA_ARGS__)


#endif
