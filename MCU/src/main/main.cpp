#include "systemManagement.h"
#include "stm32Interface.h"
#include "stm32f10x_gpio.h"


int main()
{
  //---------------------PROGRAM INITIALIZATION-------------------------------
  // we can start watchdog at this point as it is self contained
  //WatchdogInit(6500);
  SystemManagement::init();
  //WatchdogInit(400);
  //---------------------PROGRAM LOOP-------------------------------
  while (1) 
  {
    //WatchdogReset();
    SystemManagement::periodicCall();
  }
}
