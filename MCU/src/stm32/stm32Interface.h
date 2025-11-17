#ifndef __STM32_INTERFACE_H
#define __STM32_INTERFACE_H

#include "stm32f10x.h"

#ifdef __cplusplus
extern "C"
{
#endif

// constants declaration
// clock frequency is in HZ
#define SYSTEM_HCLK_FREQ     ((uint32_t)72000000)
#define SYSTEM_PCLK1_FREQ     (SYSTEM_HCLK_FREQ/2)
#define SYSTEM_PCLK2_FREQ     (SYSTEM_HCLK_FREQ)
// We set timer ticks to 1000 HZ frequency.
#define SYSTEM_TIMER_TICKS_FREQ  (1000)
  // allow 1 seconds for ethernet to start working properly
#define ETHERNET_WAKEUP_TIME (2.5 * SYSTEM_TIMER_TICKS_FREQ)

#define STM32_RAM_BASE       ((uint32_t)0x20000000)
#define STM32_FLASH_BASE     ((uint32_t)0x08000000)


// clock
void systemClockInit();
void systemClockDisable();

uint32_t getTimerTicks();
uint32_t getTimerMilliSecs(uint32_t timerRef);
uint32_t getTimerLease(uint32_t milliSec);
int32_t getLeaseTime(uint32_t timerRef);
void wait(uint32_t milliSec);

// initialization
int stm32SystemInit();
void ethernetMacInit();
int stm32EthernetInit();

// watchdog
void WatchdogReset();
int WatchdogInit(int timeoutMilliSeconds);

// miscl.
int getMacAddress(char* buffer);

#ifdef __cplusplus
}
#endif

#endif /* __STM32_INTERFACE_H */   