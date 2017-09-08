#ifndef PLATFORM_H
#define PLATFORM_H

#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"

//#include "esp_timer.h"
typedef struct RtosConnType RtosConnType;
typedef RtosConnType* ConnTypePtr;
typedef TimerHandle_t HttpdPlatTimerHandle;

#define httpd_printf(fmt, ...) printf(fmt, ##__VA_ARGS__)

#endif
