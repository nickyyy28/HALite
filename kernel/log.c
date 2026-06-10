#include "log.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

const static char log_level_str[][8] = {"DEBUG", "INFO", "WARNING", "ERROR", "FATAL"};
const static char log_level_color[][8] = {CONSOLE_COLOR_BLUE, CONSOLE_COLOR_GREEN, CONSOLE_COLOR_YELLOW, CONSOLE_COLOR_RED, CONSOLE_COLOR_PURPLE};



void SHELL_LOG_LEVEL(_LOG_LEVEL level, const char* fmt, ...)
{
	static char log_buffer[250] = {0}, raw_buffer[200] = {0};
	memset(log_buffer, 0, sizeof(log_buffer));
	memset(raw_buffer, 0, sizeof(raw_buffer));
	va_list args;
    va_start(args, fmt);
	
	const char *level_str = log_level_str[level - 1];
    
    vsprintf(raw_buffer, fmt, args);
	uint32_t cur_tick = HAL_GetTick();
	uint32_t hours = 0, minutes = 0, seconds = 0;
	hours = cur_tick / (1000 * 60 * 60);
	cur_tick %= (1000 * 60 * 60);
	minutes = cur_tick / (1000 * 60);
	cur_tick %= (1000 * 60);
	seconds = cur_tick / 1000;
	sprintf(log_buffer, "\r\n[%02lu:%02lu:%02lu][%s]: %s", hours, minutes, seconds, level_str, raw_buffer);
    
    va_end(args);
	SET_CONSOLE_COLOR(log_level_color[level - 1]);
	LOG_TRANSMIT((uint8_t*)log_buffer, strlen(log_buffer));
	RESUME_CONSOLE_COLOR();
}

void SHELL_PRINTF(const char* fmt, ...)
{
	char print_buffer[128] = {0};
	
	va_list args;
    va_start(args, fmt);

	vsprintf(print_buffer, fmt, args);

	va_end(args);

	LOG_TRANSMIT((uint8_t*)print_buffer, strlen(print_buffer));
}