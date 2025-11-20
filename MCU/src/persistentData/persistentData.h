// **********************************************************************
// **********************************************************************
// Copyright 2011 by Avi Owshanko. All rights reserved.
// **********************************************************************
// **********************************************************************

#ifndef PERSISTENT_DATA_H
#define PERSISTENT_DATA_H

#include "persistentCommon.h"
//#include "persistentSerialSettings.h"
//#include "genericClasses.h"
#include "persistentDataCenter.h"
#include "persistentSystemData.h"
#include "persistentSystemStats.h"
//#include "persistentEthernetSettings.h"
#include "communicationCenter\ethernet\mac.h"
#include "eeprom.h"

#define MAC_ADDRESS_LENGTH 6
// predeclarations
//class IPData;


// *******************************************************************
// A class for loading persistent data
// *******************************************************************
class PersistentData : public PersistentDataCenter
{
public:
  PersistentData();
  
  void loadPersistentData();
  virtual ErrorStatus resetPersistentMemory(bool resetAll = true);
  virtual ErrorStatus FactoryReset();
  
  // static settings
  virtual const MACAddr& getMacAddr() const;
  virtual const Version_T& getSystemVersion();
  virtual int setSerialNumber(const char* SN);
  virtual const char* getSerialNumber() const;
  
  //Network Setings
  virtual ErrorStatus Get_IpSetting(IpSettings_t &ip);
  virtual ErrorStatus Set_IpSetting(uint8_t Mode, uint32_t Ip, uint32_t Mask);
  
  //CB settings
  virtual ErrorStatus Get_CBSetting(int CbNum, Raw_Config_t* pData);
  virtual ErrorStatus Set_CBSetting(int CbNum, const Raw_Config_t* pData);
  
  //Hardware settings
  virtual ErrorStatus Get_PorSetting(uint8_t *OrderSetting, uint16_t *DelaySetting); 
  virtual ErrorStatus Set_PorSetting(const uint8_t *OrderSetting, const uint16_t *DelaySetting);
  virtual ErrorStatus Get_DCISetting(uint8_t *OrderSetting, uint8_t *ActionSetting, uint16_t *DelaySetting);
  virtual ErrorStatus Set_DCISetting(const uint8_t *OrderSetting, const uint8_t *ActionSetting, const uint16_t *DelaySetting);
  
  virtual ErrorStatus Get_VinSetting(uint16_t *Vmin);
  virtual ErrorStatus Set_VinSetting(uint16_t Vmin);

  virtual void handleGetConfig(Buffer& DataOut);
    
  // statistics
  virtual uint32_t getNext32RandomNumber();
  virtual uint16_t getNext16RandomNumber();
  virtual int getResetCounter() const;
  
  
  
  
private:
  void initPersistentData();
  void writePersistentStaticData(int offset, const char* data, int dataLen);
  void initializeStaticMem();
  
  void setMacAddr(char* buffer);
  
  PersistentSystemStats mPersistentSystemStats;  // random number. need it for random numbers
  PersistentSystemData mPersistentSystemData;
  MACAddr mMacAddr;
};





#endif // PERSISTENT_DATA_H
