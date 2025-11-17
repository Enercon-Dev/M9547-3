#ifndef _MACRO_H
#define _MACRO_H

#include "constants.h"
#include "stm32f10x.h"
#include "communicationCenter\handlers\handlersCommon.h"
#include "communicationCenter/buffers/buffers.h"

typedef enum
{
	POR = 0, 
	DCI = 1, 
	COMM_LOSS = 2
}MacroType;

class Macro_O{
public:
  Macro_O();

  ErrorStatus Start(MacroType macro);
  void periodicCall();
  void (*eventSetOutput)(uint8_t,uint8_t );

private:
  uint8_t PendingMacro; 
  uint8_t pNext;
  uint32_t NextOperationTime;

  uint8_t Channel[NUM_OF_CB];
  uint16_t Delay[NUM_OF_CB];
  uint8_t Operation[NUM_OF_CB];

};




#endif