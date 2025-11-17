#include "stm32Interface.h"
//#include "globalInterface.h"
#include "core_cm3.h"

#ifdef __cplusplus
extern "C" {
#endif

// system timer clock.
// The system timer can never be 0.
// set to 'SYSTEM_CLOCK_TICKS_FREQ / SYSTEM_TIMER_TICKS_PER_CLOCK_TICKS'
static volatile uint32_t timerTicksCount;

/* SysTick constants */
#define SYSTICK_ENABLE_MASK      (0x00000001)
#define SYSTICK_INT_ENABLE_MASK  (0x00000002)
#define SYSTICK_SOURCE_CLK_MASK  (0x00000004)


// **************************************************************
// Initializes the systick timer.
// Activates systick interrupt.
// **************************************************************
void systemClockInit()
{
  timerTicksCount = 1;

  u32 tickCount = SYSTEM_HCLK_FREQ / SYSTEM_TIMER_TICKS_FREQ;

  // set the systick counter, and load it for the first time.
  SysTick->LOAD  =  tickCount-1;
  NVIC_SetPriority(SysTick_IRQn, 0);
  SysTick->VAL = 0;

  // enable the systick, and activate interrupt.
  SysTick->CTRL = (SYSTICK_ENABLE_MASK | SYSTICK_INT_ENABLE_MASK | SYSTICK_SOURCE_CLK_MASK);
}

// **************************************************************
// Disable the systick timer.
// Disable systick interrupt.
// **************************************************************
void systemClockDisable()
{
  SysTick->CTRL = 0;
}

// **************************************************************
// Updates the system ticks counter.
// First updates the seconds counter, followed by the micro seconds.
// **************************************************************
void SysTick_Handler()
{
  uint32_t nextVal = timerTicksCount;
  if (nextVal == 0xFFFFFFFF) {
    nextVal = 0;
  }
  timerTicksCount = nextVal+1;
}

// **************************************************************
// Returns the timer systicks time for reference 
// **************************************************************
uint32_t getTimerTicks() {
  return timerTicksCount;
}

// **************************************************************
// Returns the time since 'timerRef' in milli seconds.
// **************************************************************
uint32_t getTimerMilliSecs(uint32_t timerRef)
{
  // note that this variable is unsigned.
  uint32_t timeOffset = (timerTicksCount - timerRef);

  // Timer overflow will happen only for timeouts greater than days...
  if (((SYSTEM_TIMER_TICKS_FREQ / 1000) * 1000) != SYSTEM_TIMER_TICKS_FREQ) {
    timeOffset *= 1000;
    // translate to milli seconds (frequency is in HZ) 
    // must be a seperate line, so that compiler will not be tempted to round factor.
    timeOffset /= SYSTEM_TIMER_TICKS_FREQ;
  }
  else {
    // 1000 is a factor of SYSTEM_TIMER_TICKS_FREQ
    timeOffset *= (SYSTEM_TIMER_TICKS_FREQ / 1000);
  }

  return timeOffset;
}

// **************************************************************
// Get a lease time.
// Can never be '0'.
// **************************************************************
uint32_t getTimerLease(uint32_t milliSec)
{
  // lease overflow will happen only for timeouts greater than days...
  uint32_t leaseTime = (milliSec * SYSTEM_TIMER_TICKS_FREQ) / 1000;
  leaseTime += getTimerTicks();
  if (leaseTime == 0) {
    leaseTime = 1;
  }

  return leaseTime;
}

// **************************************************************
// Returns lease time in milli seconds
// We never return '0' (turns to '1').
// **************************************************************
int32_t getLeaseTime(uint32_t timerRef)
{
  if (timerRef == 0) {
    return 0;
  }

  // note that we compare two unsigned, where the result is signed.
  int32_t leaseTime = (timerRef - timerTicksCount);
  // lease overflow will happen only for timeouts greater than days...
  leaseTime *= 1000;
  leaseTime /= SYSTEM_TIMER_TICKS_FREQ;
  if (leaseTime == 0) {
    leaseTime = 1;
  }

  return leaseTime;
}

// **************************************************************
// Blocks thread for 'milliseconds'.
// **************************************************************
void wait(uint32_t milliseconds)
{
  uint32_t baseTime = getTimerTicks();
  while (getTimerMilliSecs(baseTime) < milliseconds);
}

#ifdef __cplusplus
}
#endif