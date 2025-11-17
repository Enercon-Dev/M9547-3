#include "stm32Interface.h"

#define WDG_BASE_TIMEOUT      (410)
#define WDG_MAX_COUNTER       ((uint16_t)(0xFFF))
#define WDG_MAX_PRESCALAR     (6)


#ifndef DEBUG
// **************************************************************
// prescalar is log_2('timeoutMilliSeconds'/205) rounded down
// **************************************************************
static uint8_t getWDGPrescalar(int timeoutMilliSeconds)
{
  if (timeoutMilliSeconds < 0) {
    return 0;
  }

  uint8_t prescalar = 0;
  unsigned int base = WDG_BASE_TIMEOUT;
  while (base < timeoutMilliSeconds) {
    prescalar++;
    base = base*2;
  }

  if (prescalar > WDG_MAX_PRESCALAR) {
    prescalar = WDG_MAX_PRESCALAR;
  }

  return prescalar;
}

// **************************************************************
// after discarding of the prescalar, we normalize counter to 0xFFF
// For prescalar value of '0', we have counter 0xFFF reach 410 milliseconds.
// **************************************************************
static uint16_t getWDGReload(int timeoutMilliSeconds, uint8_t prescalar)
{
  while (prescalar > 0) {
    timeoutMilliSeconds = timeoutMilliSeconds / 2;
    prescalar--;
  }
  if (timeoutMilliSeconds > WDG_BASE_TIMEOUT) {
    timeoutMilliSeconds = WDG_BASE_TIMEOUT;
  }
  uint32_t retVal = WDG_MAX_COUNTER;
  retVal = retVal * timeoutMilliSeconds;
  retVal = retVal / WDG_BASE_TIMEOUT;

  return (retVal & 0x0FFFF);
}

#endif // DEBUG

// **************************************************************
// **************************************************************
void WatchdogReset()
{
  IWDG->KR = ((uint16_t)0xAAAA);
}

// **************************************************************
// activates watchdog to match 'timeoutMilliSeconds'
// this is in the range of [-33%, +25%]
// we do not allow a watchdog under 100 milliseconds
// longest period set is 26 seconds (even if 'timeoutMilliSeconds' is greater)
// **************************************************************
int WatchdogInit(int timeoutMilliSeconds)
{
#ifndef DEBUG
  if (timeoutMilliSeconds < (WDG_BASE_TIMEOUT / 4)) {
    return -1;
  }

  // wait until last operation is complete - tests for both reload and prescalar together.
  while (1) {
    // we do not need to test flag on first initialization
    uint32_t srVal = IWDG->SR;
    if ((srVal & 0x0003) == 0x0000) {
      break;
    }
  }

  uint8_t prescalar = getWDGPrescalar(timeoutMilliSeconds);
  uint16_t reload = getWDGReload(timeoutMilliSeconds, prescalar);

  IWDG->KR = ((uint16_t)0x5555);            // enable params update
  IWDG->PR = prescalar;                     // sets prescalar
  IWDG->RLR = reload;                       // sets the reload value
  IWDG->KR = ((uint16_t)0x0000);            // disable params update

  // enable Watchdog
  IWDG->KR = ((uint16_t)0xCCCC);

#endif // DEBUG

  return timeoutMilliSeconds;
}

