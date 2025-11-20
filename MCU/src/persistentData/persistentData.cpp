// **********************************************************************
// **********************************************************************
// Copyright 2011 by Avi Owshanko. All rights reserved.
// **********************************************************************
// **********************************************************************

#include "persistentData.h"
#include "persistentCommon.h"
#include "../general/genericClasses.h"
#include "stm32f10x_flash.h"
#include "globalInterface.h"
#include "stm32Interface.h"
//#include "mac.h"
#include <string.h>

// **********************************************************************
// **********************************************************************
// ***********************   Persistent Data     ************************
// **********************************************************************
// **********************************************************************


// Dummy Flash implementation - YakirZ
#pragma location=PERSISTENT_SECTION_BASE
#ifdef __GNUC__
 static char  flashMemory[PERSISTENT_SECTION_SIZE];
#elif defined ( __ICCARM__ )
__no_init static char  flashMemory[PERSISTENT_SECTION_SIZE];
#endif

#pragma location=MAGIC_NUM_LOCATION flashMemory[PERSISTENT_SECTION_SIZE];
const char MagicNum[] = PERSISTENT_MAGIC_NUM;

int readDummyFlash()
{
  const char copyright[] = "Copyright 2011 by Avi Owshanko. All rights reserved.";
  int i, retVal = 0;
  for (i=0; i<sizeof(copyright); i++) {
    retVal += copyright[i];
  }
  retVal += *(const int*)flashMemory;
  for (i=0; i<sizeof(MagicNum); i++) {
    retVal += MagicNum[i];
  }


  return retVal;
}


// **************************************************************
// constructor
// **************************************************************
PersistentData::PersistentData() :
  mMacAddr()
{

}

// **************************************************************
// initialization
// **************************************************************
void PersistentData::loadPersistentData()
{
  const char* staticSection = (const char*)PERSISTENT_BLOCK_BASE(PERSISTENT_DATA_STATIC_BLOCK_A);
  int firstInit = strcmp(staticSection, PERSISTENT_MAGIC_NUM);
  if (firstInit) {
    initPersistentData();
  }
  if (mPersistentSystemData.CheckMemory() == ERROR)
    mPersistentSystemData.Initialize();

  char mac_address[MAC_ADDRESS_LENGTH];
  setMacAddr(mac_address);
  mMacAddr = MACAddr(mac_address);

  //
  readDummyFlash();
mPersistentSystemData.CheckMemory();
  /*int i;
  for (i=0; i < SERIAL_PORT_COUNT; i++) {
    mPersistentDataPort[i].loadPersistentObject();
  }*/ // Don't need it. See header file
  //mPersistentSystemData.loadPersistentObject();  // Don't need it. See header file
  //mPersistentSystemStats.loadPersistentObject(); // Don't need it. See header file
  //mPersistentEthernetSettings.loadPersistentObject();
}

// **************************************************************
// Initializes all the persistent data.
// Will reboot immediately afterwards.
// **************************************************************
void PersistentData::initPersistentData()
{
  if (resetPersistentMemory() == ERROR)
	  enterLiveLoop();

  initializeStaticMem();


  // finalize magic number.
  char tempString[0x20];
  memset(tempString, 0, 0x20);
  strcpy (tempString, PERSISTENT_MAGIC_NUM);
  writePersistentStaticData(MAGIC_NUM_OFFSET, tempString, 0x20);

  systemResetNoStats();
}

// **************************************************************
// Writes the persistent data to relevant location.
// Will always write first to block B, and then to block A.
// **************************************************************
void PersistentData::writePersistentStaticData(int offset,
                                               const char* data,
                                               int dataLen)
{
  FlashUnlock unlocker;

  if ((offset < 0) || (dataLen < 0) || ((offset + dataLen) > FLASH_BLOCK_SIZE)) {
    enterLiveLoop();
  }

  const uint32_t baseAddrA = PERSISTENT_BLOCK_BASE(PERSISTENT_DATA_STATIC_BLOCK_A) + offset;

  int i;
  for (i=0; i<dataLen-1; i+=2) {
    uint16_t thisWord = *(uint16_t*)(data + i);
    if (FLASH_ProgramHalfWord(baseAddrA+i, thisWord) != FLASH_COMPLETE)
    {
      enterLiveLoop();
    }
  }

  if (i < dataLen) {
    // Little ENDIAN, so we have the low byte comes first.
    // Second byte will be set to '0'
    uint16_t thisWord = *(const unsigned char*)(data + i);
    if (FLASH_ProgramHalfWord(baseAddrA+i, thisWord) != FLASH_COMPLETE)
    {
      enterLiveLoop();
    }
  }
}

// **************************************************************
// Resets all the persistent memory
// **************************************************************
ErrorStatus PersistentData::resetPersistentMemory(bool resetAll)
{
  int i;

  if (mPersistentSystemData.GetMemory().EraseEEprom() == ERROR)
    return ERROR;
  if (!resetAll)
    return SUCCESS;
  for (i=0; i<PERSISTENT_BLOCK_COUNT; i++) {
    FlashUnlock unlocker;
    uint32_t addr = PERSISTENT_BLOCK_BASE(i);

    if (FLASH_ErasePage(addr) != FLASH_COMPLETE) {
      enterLiveLoop();
    }
  }
  return SUCCESS;
  
}

ErrorStatus PersistentData::FactoryReset()
{
  if (mPersistentSystemData.GetMemory().EraseEEprom() == ERROR)
    return ERROR;
  return mPersistentSystemData.Initialize();

}

// **************************************************************
// Initializes the static memory data
// **************************************************************
void PersistentData::initializeStaticMem()
{
  char tempString[0x20];

  // This is the CHIP ID address.
  memset(tempString, 0, 0x20);
  memcpy (tempString, (char*)0x1FFFF7E9, 12);
  writePersistentStaticData(CHIP_ID_OFFSET, tempString, 0x20);

  memset(tempString, 0, 0x20);
  strcpy (tempString, PERSISTENT_VERSION);
  writePersistentStaticData(VERSION_OFFSET, tempString, 0x20);

}

// **************************************************************
// initialize MAC address through installation
// **************************************************************
void PersistentData::setMacAddr(char* buffer)
{
  while (getMacAddress(buffer) != 0) {
    wait(1000);
  }
}

int PersistentData::setSerialNumber(const char* SN)
{
  char tempString[0x20];
  memset(tempString, 0xFF, 0x20);
  if (memcmp( getSerialNumber() , tempString, 0x20) != 0) return -1;
  writePersistentStaticData(SN_OFFSET, SN, strlen(SN)+1);
  return 0;
}

const char* PersistentData::getSerialNumber() const
{
  return (char*)(PERSISTENT_BLOCK_BASE(PERSISTENT_DATA_STATIC_BLOCK_A) + SN_OFFSET);

}


// **************************************************************
// Generates the next 16 quasy random sequence number
// **************************************************************
uint16_t PersistentData::getNext16RandomNumber()
{
  return mPersistentSystemStats.getNext16RandomNumber();
}

// **************************************************************
// Generates the next 32 bit quasy random sequence number
// **************************************************************
uint32_t PersistentData::getNext32RandomNumber()
{
  uint32_t retVal = getNext16RandomNumber();
  retVal = retVal << 16;
  retVal += getNext16RandomNumber();
  return retVal;
}

// **************************************************************
// Returns the MAC address
// **************************************************************
const MACAddr& PersistentData::getMacAddr() const
{
  return mMacAddr;
}


// **************************************************************
// Returns the system version
// **************************************************************
const Version_T& PersistentData::getSystemVersion()
{
  static Version_T version = {{0,0,0},0,0,{0,0,0},PERSISTENT_VERSION};
  Boot_Memory_Type *mem = (Boot_Memory_Type *)BOOT_LOADER_BASE;
  if (mem->active_version_file[0] != 0)
    version.SW_Version_Raw = mem->active_version_file;
  version.SW_Version[0] = (version.SW_Version_Raw[9]-0x30)*100 + (version.SW_Version_Raw[10]-0x30)*10 + (version.SW_Version_Raw[11]-0x30);
  version.SW_Version[1] = (version.SW_Version_Raw[13]-0x30)*100 + (version.SW_Version_Raw[14]-0x30)*10 + (version.SW_Version_Raw[15]-0x30);
  version.SW_Version[2] = (version.SW_Version_Raw[17]-0x30)*100 + (version.SW_Version_Raw[18]-0x30)*10 + (version.SW_Version_Raw[19]-0x30);
  version.FW_Version_Raw = mem->FPGA_version_file;
  version.FW_Version[0] = (version.FW_Version_Raw[9]-0x30)*100 + (version.FW_Version_Raw[10]-0x30)*10 + (version.FW_Version_Raw[11]-0x30);
  version.FW_Version[1] = (version.FW_Version_Raw[13]-0x30)*100 + (version.FW_Version_Raw[14]-0x30)*10 + (version.FW_Version_Raw[15]-0x30);
  version.FW_Version[2] = (version.FW_Version_Raw[17]-0x30)*100 + (version.FW_Version_Raw[18]-0x30)*10 + (version.FW_Version_Raw[19]-0x30);
  
  return version;
}

/* ---------------- */
/* Network settings */
/* ---------------- */

/*
PURPOSE:
  Read IP data stored at the Eeprom and fill the
PARAMETERS:
RETURN VALUE:
  If IP data was read successfully, return Enable, and Disable otherwise.
ALGORITHM:
OTHERS:
*/
ErrorStatus PersistentData::Get_IpSetting(IpSettings_t &ip)
{
  return mPersistentSystemData.Get_IpSetting(ip);
}

/*
PURPOSE:
  Write IP data to eeprom
PARAMETERS:
  Mode, Ip, Mask - paramaters need to be loaded to the eeprom
RETURN VALUE:
  SUCCESS
ALGORITHM:
OTHERS:
*/
ErrorStatus PersistentData::Set_IpSetting(uint8_t Mode, uint32_t Ip, uint32_t Mask)
{
  return mPersistentSystemData.Set_IpSetting(Mode,Ip,Mask);
}

///*
//PURPOSE:
//PARAMETERS:
//RETURN VALUE:
//ALGORITHM:
//OTHERS:
//*/
//ErrorStatus PersistentData::Get_CommLossSetting(uint16_t *Timeout, uint8_t *Channels, uint8_t *Mask)
//{
//  return ErrorStatus::SUCCESS;
//}
//
///*
//PURPOSE:
//PARAMETERS:
//RETURN VALUE:
//ALGORITHM:
//OTHERS:
//*/
//ErrorStatus PersistentData::Set_CommLossSetting(uint16_t Timeout, uint8_t Channels, uint8_t Mask)
//{
//  return ErrorStatus::SUCCESS;
//}

/* ----------- */
/* CB settings */
/* ----------- */

/*
PURPOSE:
  Read CB configuration from eeprom
PARAMETERS:
  CbNum - CB identifier
  pData - CB configuration will be stored here
RETURN VALUE:
  SUCCESS if reading is valid
ALGORITHM:
OTHERS:
*/
ErrorStatus PersistentData::Get_CBSetting(int CbNum, Raw_Config_t* pData)
{
    return mPersistentSystemData.Get_CBSetting(CbNum, pData);
}

/*
PURPOSE:
PARAMETERS:
RETURN VALUE:
ALGORITHM:
OTHERS:
*/
ErrorStatus PersistentData::Set_CBSetting(int CbNum, const Raw_Config_t* pData)
{
  return mPersistentSystemData.Set_CBSetting(CbNum,pData);
}

/* ----------------- */
/* Hardware settings */
/* ----------------- */

ErrorStatus PersistentData::Get_PorSetting(uint8_t *OrderSetting, uint16_t *DelaySetting)
{
  return mPersistentSystemData.Get_PorSetting(OrderSetting, DelaySetting);
}
ErrorStatus PersistentData::Set_PorSetting(const uint8_t *OrderSetting, const uint16_t *DelaySetting)
{
 return mPersistentSystemData.Set_PorSetting(OrderSetting, DelaySetting);
}
 ErrorStatus PersistentData::Get_DCISetting(uint8_t *OrderSetting, uint8_t *ActionSetting, uint16_t *DelaySetting)
{
  return mPersistentSystemData.Get_DCISetting(OrderSetting, ActionSetting, DelaySetting);
}
 ErrorStatus PersistentData::Set_DCISetting(const uint8_t *OrderSetting, const uint8_t *ActionSetting, const uint16_t *DelaySetting)
{
  return mPersistentSystemData.Set_DCISetting(OrderSetting, ActionSetting, DelaySetting);
}

 ErrorStatus PersistentData::Get_VinSetting(uint16_t *Vmin)
{
  return mPersistentSystemData.Get_VinSetting(Vmin);
}
 ErrorStatus PersistentData::Set_VinSetting(uint16_t Vmin)
{
  return mPersistentSystemData.Set_VinSetting(Vmin);
}

int PersistentData::getResetCounter() const
{
  return 0;
}

void PersistentData::handleGetConfig(Buffer& DataOut)
{
  mPersistentSystemData.handleGetConfig(DataOut);
}
