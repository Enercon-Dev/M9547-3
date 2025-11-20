#ifndef PERSISTENT_DATA_CENTER_H
#define PERSISTENT_DATA_CENTER_H

#include "globalTypes.h"
#include "ethernet/mac.h"
#include "genericClasses.h"
#include "stm32f10x.h"
#include "buffers/buffers.h"

// predeclarations
class SerialPhysicalStreamSettings;
class SerialBufferStreamSettings;
class EthernetStreamSettings;
//class MACAddr;
class IPData;
class DataString;

typedef struct {
  char FW_Version[3];
  const char *FW_Version_Raw;
  const char* FW_Date;
  char SW_Version[3];
  const char *SW_Version_Raw;
} Version_T;


// *******************************************************************
// A class for reading persistent data.
// *******************************************************************
class PersistentDataCenter
{
public:
  virtual ErrorStatus resetPersistentMemory(bool resetAll = true) = 0;
  virtual ErrorStatus FactoryReset() = 0;
  
  // static settings
  virtual const MACAddr& getMacAddr() const = 0;
  virtual const Version_T& getSystemVersion() = 0;
  virtual int setSerialNumber(const char* SN) = 0;
  virtual const char* getSerialNumber() const = 0;
  
  //Network Settings
  virtual ErrorStatus Get_IpSetting(IpSettings_t &ip) = 0;
  virtual ErrorStatus Set_IpSetting(uint8_t Mode, uint32_t Ip, uint32_t Mask) = 0;

  
  //CB settings
  virtual ErrorStatus Get_CBSetting(int CbNum, Raw_Config_t* pData) = 0;
  virtual ErrorStatus Set_CBSetting(int CbNum, const Raw_Config_t* pData) = 0;
  
  //Hardware settings
  virtual ErrorStatus Get_PorSetting(uint8_t *OrderSetting, uint16_t *DelaySetting) = 0; 
  virtual ErrorStatus Set_PorSetting(const uint8_t *OrderSetting, const uint16_t *DelaySetting) = 0; 
  virtual ErrorStatus Get_DCISetting(uint8_t *OrderSetting, uint8_t *ActionSetting, uint16_t *DelaySetting) = 0; 
  virtual ErrorStatus Set_DCISetting(const uint8_t *OrderSetting, const uint8_t *ActionSetting, const uint16_t *DelaySetting) = 0; 
  
  virtual ErrorStatus Get_VinSetting(uint16_t *Vmin) = 0;
  virtual ErrorStatus Set_VinSetting(uint16_t Vmin) = 0;

  virtual void handleGetConfig(Buffer& DataOut) = 0;
  
  // statistics
  virtual uint32_t getNext32RandomNumber() = 0;
  virtual uint16_t getNext16RandomNumber() = 0;
  virtual int getResetCounter() const = 0;
  
};


#endif // PERSISTENT_DATA_CENTER_H
