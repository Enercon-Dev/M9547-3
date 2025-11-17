#ifndef _LED_H
#define _LED_H

#include <string.h>
#include "stm32f10x_gpio.h"

typedef enum {LED_COLOR_OFF, LED_COLOR_GREEN, LED_COLOR_RED, LED_COLOR_RED_GREEN, LED_COLOR_PRINT} Led_Color_Type;

class Led{
public:
  static Led& GetInstance()
  { 
    if (mSingelton == NULL)
      mSingelton = new Led;
    return *mSingelton; 
  }
  
  void periodicCall();
  void SetLed(Led_Color_Type color, uint16_t period);
  
  
private:
  Led();
  ~Led(){}
  static Led* mSingelton;
  
  uint16_t mLedRedPeriod;
  uint16_t mLedGreenPeriod;
  uint16_t mLedPrintPeriod;
};


#endif