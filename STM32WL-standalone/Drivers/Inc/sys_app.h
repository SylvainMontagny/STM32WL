
#ifndef __SYS_APP_H__
#define __SYS_APP_H__

#ifdef __cplusplus
extern "C" {
#endif


#include "stdint.h"
#include "sys_conf.h"
#include "stm32_adv_trace.h"
#include "General_Setup.h"


#define APP_PPRINTF(...)  do{ } while( UTIL_ADV_TRACE_OK \
                              != UTIL_ADV_TRACE_COND_FSend(VLEVEL_ALWAYS, T_REG_OFF, TS_OFF, __VA_ARGS__) ) /* Polling Mode */
#define APP_TPRINTF(...)   do{ {UTIL_ADV_TRACE_COND_FSend(VLEVEL_ALWAYS, T_REG_OFF, TS_ON, __VA_ARGS__);} }while(0); /* with timestamp */
#define APP_PRINTF(...)   do{ {UTIL_ADV_TRACE_COND_FSend(VLEVEL_ALWAYS, T_REG_OFF, TS_OFF, __VA_ARGS__);} }while(0);

#if defined (APP_LOG_ENABLED) && (APP_LOG_ENABLED == 1)
#define APP_LOG(TS,VL,...)   do{ {UTIL_ADV_TRACE_COND_FSend(VL, T_REG_OFF, TS, __VA_ARGS__);} }while(0);
#elif defined (APP_LOG_ENABLED) && (APP_LOG_ENABLED == 0) /* APP_LOG disabled */
#define APP_LOG(TS,VL,...)
#else
#error "APP_LOG_ENABLED not defined or out of range <0,1>"
#endif /* APP_LOG_ENABLED */
#if (TERMINAL_COLORS == true)
	#define APP_LOG_COLOR(COLOR)	do{ {UTIL_ADV_TRACE_COND_FSend(1, T_REG_OFF, 0,COLOR);} }while(0);
#else
#define APP_LOG_COLOR(COLOR)
#endif


void SystemApp_Init(void);
uint8_t GetBatteryLevel(void);
uint16_t GetTemperatureLevel(void);
void GetUniqueId(uint8_t *id);
uint32_t GetDevAddr(void);

// Edit Sacha, originally static function
void TimestampNow(uint8_t *buff, uint16_t *size);


#ifdef __cplusplus
}
#endif

#endif /* __SYS_APP_H__ */


