#ifndef _HARDWARE_CENTER_H
#define _HARDWARE_CENTER_H

#include "ADC.h"
#include "GPIO.h"
#include "CB.h"
#include "Macro.h"
#include "stm32f10x.h"
#include "communicationCenter\handlers\handlersCommon.h"
#include "communicationCenter/buffers/buffers.h"
#include "constants.h"
#include "systemManagement.h"


void ChangeOutputWrapper(uint8_t output, uint8_t operation);

class HardwareCenter{
public:
  static HardwareCenter& GetInstance()
  { 
    if (mSingelton == NULL)
      mSingelton = new HardwareCenter;
    return *mSingelton; 
  }
  inline ErrorStatus Init(){
    for (int i=8 ; i<39 ; i++)
      *(((uint8_t *)(&LastStatus))+i) = 0;
    AlertAcknoledge();
    SystemManagement::getPersistentDataCenter().Get_VinSetting(&VinMinAlert);
    Macro.eventSetOutput = ChangeOutputWrapper;
    return CB_Int::Init();
  }


  ErrorStatus GetStatus(Status_Msg_T& Status);
  FlagStatus GetAlert() const;
  void AlertAcknoledge();
    
  void periodicCall();
  
  void            handleSetOutput(uint8_t output, uint8_t operation);
  COMMAND_SUCCESS handleSetOutput(Buffer& DataIn);
  COMMAND_SUCCESS handleSetGroup(Buffer& DataIn);
  COMMAND_SUCCESS handleTripRest(Buffer& DataIn);
  COMMAND_SUCCESS handleSetBattle(Buffer& DataIn);
  COMMAND_SUCCESS handleSetLimit(Buffer& DataIn);
  void handleSetVinLimit(Buffer& DataIn,Buffer& DataOut);
  void handelGetOutsTemp(Buffer& DataOut);


  uint8_t CommStat;
  FunctionalState OverTemp;
  FunctionalState TempWarning;

  COMMAND_SUCCESS IsValidOutputNumber(uint8_t group);
private:
  HardwareCenter();
  ~HardwareCenter(){}
    static HardwareCenter* mSingelton ;

  uint8_t state;

  uint16_t VinMinAlert;
  uint32_t mOutOnTime[NUM_OF_CB];
  Status_Msg_T StatusAlert;
  Status_Msg_T LastStatus;
  GPIO& IOs;
  ADC_DRV& ADC;
  EEprom memory;
  Macro_O Macro;
  

};






#endif
