#ifndef _GPIO_H
#define _GPIO_H

#include "stm32f10x_gpio.h"
#include <string.h>

/* GPIO Inputs */
#pragma pack(push)
#pragma pack(1)
typedef struct
{
  uint8_t WP_MCU        ;
  uint8_t DCI          : 1;
  uint8_t RAT          : 1;
  uint8_t SHDN         : 1;
  uint8_t DCI_L2H_Flag : 1;
  uint8_t RAT_L2H_Flag : 1;
  uint8_t SHDN_H2L_Flag : 1;
  } GPIO_Inputs;
#pragma pack(pop)


/* GPIO Ports */
#define IO_Port   GPIOC

/* GPIO Pins */
#define DCI_Pin   GPIO_Pin_4
#define RAT_Pin   GPIO_Pin_5
#define SHDN_Pin  GPIO_Pin_6
#define FAULT_Pin GPIO_Pin_9
#define WP_Pin    GPIO_Pin_12

class GPIO{
public:
  static GPIO& GetInstance()
  { 
    if (mSingelton == NULL)
      mSingelton = new GPIO;
    return *mSingelton; 
  }
  
  void periodicCall();
  GPIO_Inputs Get_GPIO_Inputs();
  void SetFault(FunctionalState enable);
  
private:
    GPIO();
  ~GPIO(){}
  static GPIO* mSingelton;
  
  GPIO_Inputs gpio_Inputs;
  uint8_t     filt_DCI;
  uint8_t     filt_RAT;
  uint8_t     filt_SHDN;
  uint8_t     filt_WP;
  uint8_t     acc_DCI;
  uint8_t     acc_RAT;
  uint8_t     acc_SHDN;
  uint8_t     acc_WP;
};


#endif
