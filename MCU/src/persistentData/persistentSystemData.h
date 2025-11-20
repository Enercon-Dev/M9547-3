// **********************************************************************
// **********************************************************************
// Copyright 2011 by Avi Owshanko. All rights reserved.
// **********************************************************************
// **********************************************************************

#ifndef PERSISTENT_SYSTEM_DATA_H
#define PERSISTENT_SYSTEM_DATA_H

#include "persistentCommon.h"
#include "communicationCenter/handlers/handlersCommon.h"
#include "eeprom.h"
#include "communicationCenter/buffers/buffers.h"
#include "communicationCenter/ethernet/mac.h"

#define SYSTEM_DATA_BASE_ADDRESS (0)

#pragma pack(push)
#pragma pack(1)
typedef struct {
  IpSettings_t IP_Settings{0x3300A8C0, 0x00FFFFFF, 1};  // 9B
	//  IpSettings_t IP_Settings{0x3300A8C0, 0x00FFFFFF, 0x00000000, 1};  // 13B

  //CommLoss_t   CommLoss{1000, 0, 0};                    // 4B
  Raw_Config_t ChannelConfig[NUM_OF_CB] = {                     // 7B * 16 = 112B
    {(uint16_t)30*16, 125*16, 1000, 255},
    {(uint16_t)30*16, 125*16, 1000, 255},
    {(uint16_t)15*16  , 80*16 , 1000, 255},
    {(uint16_t)30*16, 125*16, 1000, 255},
    {(uint16_t)15*16  , 80*16 , 1000, 255},
    {(uint16_t)15*16  , 80*16 , 1000, 255},
    {(uint16_t)15*16  , 80*16 , 1000, 255},
    {(uint16_t)15*16  , 80*16 , 1000, 255},
    {(uint16_t)15*16  , 80*16 , 1000, 255},
    {(uint16_t)15*16  , 80*16 , 1000, 255},
    {(uint16_t)(7.5*16)  , 40*16 , 1000, 255},
    {(uint16_t)15*16  , 80*16 , 1000, 255},
    {(uint16_t)30*16, 125*16, 1000, 255},
    {(uint16_t)15*16  , 80*16 , 1000, 255},
    {(uint16_t)30*16, 125*16, 1000, 255},
    {(uint16_t)30*16, 125*16, 1000, 255},
  };
  Por_Order_t Por_Order[NUM_OF_CB] = {                          // 3B * 16 = 48B
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 0},
 };
  DCI_Order_t Dci_Order[NUM_OF_CB] = {                          // 4B * 16 = 64B
    {0,0,0},
    {0,0,0},
    {0,0,0},
    {0,0,0},
    {0,0,0},
    {0,0,0},
    {0,0,0},
    {0,0,0},
    {0,0,0},
    {0,0,0},
    {0,0,0},
    {0,0,0},
    {0,0,0},
    {0,0,0},
    {0,0,0},
    {0,0,0},
  };
  uint16_t VinVoltAlert = (23*16);                                //2B
  	  	  	  	  	  	  	  	  	  	  	  	  	  	  // SUB TOTAL = 205B
  char magicNumber[20] = PERSISTENT_MAGIC_NUM;          // 20B
} SystemSettings_t;                                     // TOTAL = 225B
#pragma pack(pop)


// **************************************************************
// The Serial persistent object
// **************************************************************
class PersistentSystemData
{
public:
  PersistentSystemData();
  ErrorStatus CheckMemory();
  ErrorStatus Initialize();
  //ErrorStatus FullBufferWrite(Buffer& DataIn);
  //ErrorStatus FullBufferRead(Buffer& DataOut);
  
  //Network Settings
   ErrorStatus Get_IpSetting(IpSettings_t &ip);
   ErrorStatus Set_IpSetting(uint8_t Mode, uint32_t Ip, uint32_t Mask);
   
  //CB settings
   ErrorStatus Get_CBSetting(int CbNum, Raw_Config_t* pData);
   ErrorStatus Set_CBSetting(int CbNum, const Raw_Config_t* pData);
  
  //Hardware settings
   ErrorStatus Get_PorSetting(uint8_t *OrderSetting, uint16_t *DelaySetting); 
   ErrorStatus Set_PorSetting(const uint8_t *OrderSetting, const uint16_t *DelaySetting); 
   ErrorStatus Get_DCISetting(uint8_t *OrderSetting, uint8_t *ActionSetting, uint16_t *DelaySetting); 
   ErrorStatus Set_DCISetting(const uint8_t *OrderSetting, const uint8_t *ActionSetting, const uint16_t *DelaySetting); 
  
   ErrorStatus Get_VinSetting(uint16_t *Vmin);
   ErrorStatus Set_VinSetting(uint16_t Vmin);

  void handleGetConfig(Buffer& DataOut);
  
  EEprom&      GetMemory(void);
private:
  ErrorStatus UpdateMemory(const SystemSettings_t* nSystemSettings);
  EEprom      Memory;
  ErrorStatus MemoryOk;

};








#endif // PERSISTENT_SYSTEM_DATA_H
