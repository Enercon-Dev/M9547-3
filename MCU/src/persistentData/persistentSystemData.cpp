// **********************************************************************
// **********************************************************************
// Copyright 2011 by Avi Owshanko. All rights reserved.
// **********************************************************************
// **********************************************************************

#include "persistentSystemData.h"
#include "../general/genericClasses.h"
#include "stm32f10x_flash.h"
#include "globalInterface.h"
#include "constants.h"
#include <string.h>


// **********************************************************************
// **********************************************************************
// ******************   Persistent System Settings     ******************
// **********************************************************************
// **********************************************************************

PersistentSystemData::PersistentSystemData() : MemoryOk(ErrorStatus::ERROR)
{
}

ErrorStatus PersistentSystemData::Initialize()
{
  SystemSettings_t nSettings;
  MemoryOk = UpdateMemory(&nSettings);
  return MemoryOk;
}

ErrorStatus PersistentSystemData::CheckMemory()
{
  char magic[20];
  Memory.ReadFromEEprom((&((SystemSettings_t *)SYSTEM_DATA_BASE_ADDRESS)->magicNumber),20,magic);
  int firstInit = strcmp(magic, PERSISTENT_MAGIC_NUM);
  MemoryOk = firstInit ? ERROR : SUCCESS;
  return MemoryOk;
}

/* ---------------- */
/* Network settings */
/* ---------------- */

ErrorStatus PersistentSystemData::Get_IpSetting(IpSettings_t &ip)
{
  int rc = (int) SUCCESS;
  IpSettings_t ip_get;
  if (MemoryOk == ERROR)
  {
  SystemSettings_t st;
  ip_get = st.IP_Settings;  
  rc &= ERROR;
  }
  else
    rc &= Memory.ReadFromEEprom((&((SystemSettings_t *)SYSTEM_DATA_BASE_ADDRESS)->IP_Settings), sizeof(IpSettings_t),&ip_get);  
  ip.IP     = ip_get.IP;
  ip.IpMask = ip_get.IpMask;
  ip.IpType = ip_get.IpType;
  return (ErrorStatus)rc;
}

ErrorStatus PersistentSystemData::Set_IpSetting(uint8_t Mode, uint32_t Ip, uint32_t Mask)
{
  if (MemoryOk == ERROR)
    return ERROR;
  if (Ip == 0)
    return ERROR;
  IpSettings_t ip_set;
  ip_set.IP     = Ip;
  ip_set.IpMask = Mask;
  ip_set.IpType = Mode;
  return Memory.WriteToEEprom((&((SystemSettings_t *)SYSTEM_DATA_BASE_ADDRESS)->IP_Settings), sizeof(IpSettings_t),&ip_set);
}


/* ----------- */
/* CB settings */
/* ----------- */

ErrorStatus PersistentSystemData::Get_CBSetting(int CbNum, Raw_Config_t* pData)
{
  int rc = (int) SUCCESS;
  if (MemoryOk == ERROR)
  {
    SystemSettings_t st;
    *pData = st.ChannelConfig[CbNum];  
    rc &= ERROR;
  }
  else
    rc &= Memory.ReadFromEEprom(((SystemSettings_t *)SYSTEM_DATA_BASE_ADDRESS)->ChannelConfig + CbNum , sizeof(Raw_Config_t), pData);
  return (ErrorStatus)rc;
}

ErrorStatus PersistentSystemData::Set_CBSetting(int CbNum, const Raw_Config_t* pData)
{
  if (MemoryOk == ERROR)
    return ERROR;
  int rc = (int) SUCCESS;
  rc &= Memory.WriteToEEprom(((SystemSettings_t *)SYSTEM_DATA_BASE_ADDRESS)->ChannelConfig + CbNum, sizeof(Raw_Config_t), pData);
  return (ErrorStatus) rc;
}

/* ----------------- */
/* Hardware settings */
/* ----------------- */

ErrorStatus PersistentSystemData::Get_PorSetting(uint8_t *OrderSetting, uint16_t *DelaySetting)
{
  int rc = (int) SUCCESS;
  Por_Order_t por_get;
  if (MemoryOk == ERROR)
  {
    SystemSettings_t st;
    for (int i=0; i<NUM_OF_CB; i++)
    {
      por_get = st.Por_Order[i];
      OrderSetting[i] = por_get.Output;
      DelaySetting[i] = por_get.Delay;
    }
    rc &= ERROR;
  }
  else
    for (int i=0; i<NUM_OF_CB; i++)
    {
      rc &= Memory.ReadFromEEprom(((SystemSettings_t *)SYSTEM_DATA_BASE_ADDRESS)->Por_Order + i, sizeof(Por_Order_t),&por_get);
      OrderSetting[i] = por_get.Output;
      DelaySetting[i] = por_get.Delay;
    }
  return (ErrorStatus) rc;
}

ErrorStatus PersistentSystemData::Set_PorSetting(const uint8_t *OrderSetting, const uint16_t *DelaySetting)
{
  if (MemoryOk == ERROR)
    return ERROR;
  int rc = (int) SUCCESS;
  Por_Order_t por_set[NUM_OF_CB];
  for(uint8_t i = 0; i < NUM_OF_CB; ++i)
  {
    por_set[i].Delay  = DelaySetting[i];
    por_set[i].Output = OrderSetting[i];
  }
  rc &= Memory.WriteToEEprom(((SystemSettings_t *)SYSTEM_DATA_BASE_ADDRESS)->Por_Order, sizeof(Por_Order_t) *NUM_OF_CB ,&por_set);
  return (ErrorStatus) rc;
}

ErrorStatus PersistentSystemData::Get_DCISetting(uint8_t *OrderSetting, uint8_t *ActionSetting, uint16_t *DelaySetting)
{
  int rc = (int) SUCCESS;
  DCI_Order_t dci_get;
  if (MemoryOk == ERROR)
  {
    SystemSettings_t st;
    for (int i=0; i<NUM_OF_CB; i++)
    {
      dci_get = st.Dci_Order[i];
      OrderSetting[i]  = dci_get.Output;
      ActionSetting[i] = dci_get.Action;
      DelaySetting[i]  = dci_get.Delay;
    }
    rc &= ERROR;
  }
  else
    for (int i=0; i<NUM_OF_CB; i++){
      rc &= Memory.ReadFromEEprom(((SystemSettings_t *)SYSTEM_DATA_BASE_ADDRESS)->Dci_Order +i, sizeof(DCI_Order_t),&dci_get);
      OrderSetting[i]  = dci_get.Output;
      ActionSetting[i] = dci_get.Action;
      DelaySetting[i]  = dci_get.Delay;
    }
  return (ErrorStatus) rc;
}

ErrorStatus PersistentSystemData::Set_DCISetting(const uint8_t *OrderSetting, const uint8_t *ActionSetting, const uint16_t *DelaySetting)
{
  if (MemoryOk == ERROR)
    return ERROR;
  int rc = (int) SUCCESS;
  DCI_Order_t dci_set[NUM_OF_CB];
  for(uint8_t i = 0; i < NUM_OF_CB; ++i)
  {
    dci_set[i].Action  = ActionSetting[i];
    dci_set[i].Delay   = DelaySetting[i];
    dci_set[i].Output  = OrderSetting[i];
  }
  rc &= Memory.WriteToEEprom(((SystemSettings_t *)SYSTEM_DATA_BASE_ADDRESS)->Dci_Order, sizeof(DCI_Order_t) *NUM_OF_CB ,&dci_set);
  return (ErrorStatus) rc;
}

ErrorStatus PersistentSystemData::UpdateMemory(const SystemSettings_t* nSystemSettings)
{
  uint8_t dataLen = sizeof(SystemSettings_t);

  //return Memory.WriteToEEprom((SystemSettings_t *)SYSTEM_DATA_BASE_ADDRESS, dataLen ,nSystemSettings); // YakirZ - Minus 20 in order to exclude MAGIC_NUM, otherwise we get eeprom error
  Memory.WriteToEEprom((&((SystemSettings_t *)SYSTEM_DATA_BASE_ADDRESS)->magicNumber),20,nSystemSettings->magicNumber);
  return Memory.WriteToEEprom((SystemSettings_t *)SYSTEM_DATA_BASE_ADDRESS, (sizeof(SystemSettings_t)-20) ,nSystemSettings); // YakirZ - Minus 20 in order to exclude MAGIC_NUM, otherwise we get eeprom error
}

/*
ErrorStatus PersistentSystemData::FullBufferWrite(Buffer& DataIn){  
  SystemSettings_t nSettings;
  for (int i=0; i< sizeof(SystemSettings_t); i++)
  {
    *((uint8_t *)(&nSettings) + i) = DataIn.readChar();
  }
  
  return UpdateMemory(&nSettings);
}

ErrorStatus PersistentSystemData::FullBufferRead(Buffer& DataOut)
{
  if (DataOut.writeBlock(&SystemSettings,sizeof(SystemSettings_t)) < 0)
    return ERROR;
  return SUCCESS;
}
*/

void PersistentSystemData::handleGetConfig(Buffer& DataOut)
{
  DataOut.writeShort(4+sizeof(SystemSettings_t)-20);
  SystemSettings_t setting;
  if(Memory.ReadFromEEprom(SYSTEM_DATA_BASE_ADDRESS, sizeof(SystemSettings_t), &setting) == ERROR)
      DataOut.writeChar(1);
  else if (strcmp(setting.magicNumber, PERSISTENT_MAGIC_NUM))
      DataOut.writeChar(1);
  else
      DataOut.writeChar(0);
  DataOut.writeBlock(&(setting.IP_Settings), 9);
  for (int i=0; i< NUM_OF_CB; ++i)
    DataOut.writeShort(setting.ChannelConfig[i].Over_Load_Limit);
  for (int i=0; i< NUM_OF_CB; ++i)
    DataOut.writeShort(setting.ChannelConfig[i].Current_Limit);
  for (int i=0; i< NUM_OF_CB; ++i)
    DataOut.writeShort(setting.ChannelConfig[i].Thermal_Const);
  for (int i=0; i< NUM_OF_CB; ++i)
    DataOut.writeChar(setting.ChannelConfig[i].GroupNum);
  for (int i=0; i< NUM_OF_CB; ++i)
    DataOut.writeChar(setting.Por_Order[i].Output);
  for (int i=0; i< NUM_OF_CB; ++i)
    DataOut.writeShort(setting.Por_Order[i].Delay);
  for (int i=0; i< NUM_OF_CB; ++i)
    DataOut.writeChar(setting.Dci_Order[i].Output);
  for (int i=0; i< NUM_OF_CB; ++i)
    DataOut.writeChar(setting.Dci_Order[i].Action);
  for (int i=0; i< NUM_OF_CB; ++i)
    DataOut.writeShort(setting.Dci_Order[i].Delay);  
}

EEprom& PersistentSystemData::GetMemory(void)
{
    return Memory;
}

