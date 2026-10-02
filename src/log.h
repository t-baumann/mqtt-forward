#ifndef MQTT_FORWARD_LOG_H
#define MQTT_FORWARD_LOG_H

#include <stdbool.h>
#include "compat_win32.h"
#ifndef _WIN32
#include <syslog.h>
#endif

extern bool server_mode;

void mqtt_forward_set_log_level(int level);
int mqtt_forward_get_log_level(void);
void mqtt_forward_log(int level, const char *fmt, ...);

#define LOG(level, ...) \
	do { \
		mqtt_forward_log((level), __VA_ARGS__); \
	} while (0)

#endif
