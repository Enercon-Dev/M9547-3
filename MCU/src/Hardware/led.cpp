#include "led.h"
#include "stm32f10x_gpio.h"
#include "stm32Interface.h"

Led* Led::mSingelton = NULL;

Led::Led(): mLedRedPeriod(0),  mLedGreenPeriod(0), mLedPrintPeriod(0)
{
  
  GPIO_InitTypeDef   GPIO_InitStructure;
  
  // external led -Green
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  // turn led off
  GPIO_WriteBit(GPIOC, GPIO_Pin_8, Bit_RESET);
  
  // external led -Red
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  //turn led off
  GPIO_WriteBit(GPIOC, GPIO_Pin_9, Bit_RESET);
  
  // on print led
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  // turn led off
  GPIO_WriteBit(GPIOB, GPIO_Pin_15, Bit_SET);
  
}

void Led::periodicCall(){
  if (mLedGreenPeriod == 0){
    GPIO_ResetBits(GPIOC, GPIO_Pin_8);
  }
  else if (mLedGreenPeriod == 0xFFFF){
    GPIO_SetBits(GPIOC, GPIO_Pin_8);
  }
  else if (getTimerTicks() % mLedGreenPeriod < mLedGreenPeriod/2){
    GPIO_SetBits(GPIOC, GPIO_Pin_8);
  }
  else {
    GPIO_ResetBits(GPIOC, GPIO_Pin_8);
  }
  
  if (mLedRedPeriod == 0){
    GPIO_ResetBits(GPIOC, GPIO_Pin_9);
  }
  else if (mLedRedPeriod == 0xFFFF){
    GPIO_SetBits(GPIOC, GPIO_Pin_9);
  }
  else if (getTimerTicks() % mLedRedPeriod >  mLedRedPeriod/2){
    GPIO_SetBits(GPIOC, GPIO_Pin_9);
  }
  else {
    GPIO_ResetBits(GPIOC, GPIO_Pin_9);
  }
    
  
  if (mLedPrintPeriod !=0 && (getTimerTicks() % mLedPrintPeriod   < mLedPrintPeriod/2))
    GPIO_ResetBits(GPIOB, GPIO_Pin_15);
  else
    GPIO_SetBits(GPIOB, GPIO_Pin_15);
  
}


void Led::SetLed(Led_Color_Type color, uint16_t period = 0){
  switch(color){
  case LED_COLOR_OFF:
    mLedGreenPeriod = 0;
    mLedRedPeriod = 0;
    break;
  case LED_COLOR_GREEN:
    mLedGreenPeriod = (period == 0) ? 0xFFFF : period;
    mLedRedPeriod = 0;
    break;
  case LED_COLOR_RED:
    mLedRedPeriod = (period == 0) ? 0xFFFF : period;
    mLedGreenPeriod = 0;
    break;
  case  LED_COLOR_RED_GREEN:
    mLedGreenPeriod = period;
    mLedRedPeriod = period;
    break;
  case LED_COLOR_PRINT:
    mLedPrintPeriod = period;
  }
  
}