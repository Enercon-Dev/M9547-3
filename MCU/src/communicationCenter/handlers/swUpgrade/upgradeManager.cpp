
#include "upgradeManager.h"
#include "persistentCommon.h"
#include "genericClasses.h"
#include "stm32f10x_flash.h"
//#include "Hardware/FPGA_Update.h"
#include <string.h>

upgradeManager* upgradeManager::mSingleton = NULL;

upgradeManager::upgradeManager(Layer3& udpLayer):
  mFTPSWClient(FTP_CLIENT_PORT, UDP_PORT_FTP ,udpLayer,upgradeManager::On_StartSWUpgrade, upgradeManager::On_DataReceived),
  //mFTPFWClient(FTP_HW_CLIENT_PORT, UDP_PORT_FTP_FW, udpLayer,upgradeManager::On_StartFWUpgrade , upgradeManager::On_FWDataReceived),
  mSWStatus(UPDATE_READY)//,
  //mFWStatus(UPDATE_READY)
{
  if (mSingleton == NULL)
    mSingleton = this;
}

COMMAND_SUCCESS upgradeManager::handleSetSoftwareUpdate(Buffer& DataIn)
{
  if (DataIn.readShort() != 40)
    return NACK;
  uint32_t ip;
  char filename[33];
  DataIn.readBlock((char *)&ip,4);
  DataIn.advanceRead(4);
  DataIn.readBlock(filename,33);
  DataIn.advanceRead(33);
  mSWStatus = UPDATE_BUSY;
  if (mFTPSWClient.RequestFile(ip,filename) < 0)
    return NACK;
  return ACK;
  
}

void upgradeManager::handleGetSoftwareInfo(Buffer& DataOut)
{
  if ( mSWStatus == UPDATE_BUSY ){
    switch(mFTPSWClient.mLastError)
    {
    case FTP_ERRCODE_DISK_FULL:
      mSWStatus = UPDATE_TOO_LARGE;
      break;
    default:
      mSWStatus = UPDATE_BUSY;
      break;
    }  
  }
  
  DataOut.writeChar(CONTROL_OPCODE_SW_INFO);
  DataOut.writeChar(0);
  DataOut.writeChar(82);
  DataOut.writeBlock(&(mFTPSWClient.mServerIP), 4);
  DataOut.writeBlock(&(mFTPSWClient.mFileName), 33);
  DataOut.writeChar(mSWStatus);
  DataOut.writeBlock(&(mFTPSWClient.mBytesReceived), 4);
  Boot_Memory_Type *mem = (Boot_Memory_Type *)BOOT_LOADER_BASE;
  DataOut.writeBlock(mem->active_version_file, 33);
  DataOut.writeBlock(&(mem->active_version_length), 4);
}
 
/*
COMMAND_SUCCESS upgradeManager::handleSetFirmwareUpdate(Buffer& DataIn)
{
  if (DataIn.readShort() != 40)
    return NACK;
  uint32_t ip;
  char filename[33];
  DataIn.readBlock((char *)&ip,4);
  DataIn.advanceRead(4);
  DataIn.readBlock(filename,33);
  DataIn.advanceRead(33);
*/
  /*FPGA_Update fpga_update;
  if (fpga_update.Erase_Flash() < 0){
    mFWStatus = UPDATE_FAIL;
    return NACK;
  } */ 
/*
  mFWStatus = UPDATE_BUSY;
  if (mFTPFWClient.RequestFile(ip,filename) < 0)
    return NACK;
  
  return ACK;
  
}

void upgradeManager::handleGetFirmwareInfo(Buffer& DataOut)
{
  if ( mFWStatus == UPDATE_BUSY ){
    switch(mFTPFWClient.mLastError)
    {
    case FTP_ERRCODE_DISK_FULL:
      mFWStatus = UPDATE_TOO_LARGE;
      break;
    default:
      mFWStatus = UPDATE_BUSY;
      break;
    }  
  }
  
  DataOut.writeChar(CONTROL_OPCODE_FW_INFO);
  DataOut.writeChar(0);
  DataOut.writeChar(82);
  DataOut.writeBlock(&(mFTPFWClient.mServerIP), 4);
  DataOut.writeBlock(&(mFTPFWClient.mFileName), 33);
  DataOut.writeChar(mFWStatus);
  DataOut.writeBlock(&(mFTPFWClient.mBytesReceived), 4);
  Boot_Memory_Type *mem = (Boot_Memory_Type *)BOOT_LOADER_BASE;
  DataOut.writeBlock(mem->FPGA_version_file, 33);
  DataOut.writeBlock(&(mem->FPGA_version_length), 4);
}
*/
void upgradeManager::On_StartSWUpgrade(uint32_t len)
{
    mSingleton->StartSWUpgrade(len);
}

/*
void upgradeManager::On_StartFWUpgrade(uint32_t len)
{
  if (FPGA_Update::finishUpdate(len) != 0){
    mSingleton->mFWStatus = UPDATE_FAIL;
    return;
  }
  mSingleton->mFWStatus = UPDATE_SUCCESSFUL;
    
  Boot_Memory_Type Mem;
  FlashUnlock unlocker;
  memcpy(&Mem, (void *)BOOT_LOADER_BASE, sizeof(Mem));
  if (FLASH_ErasePage(BOOT_LOADER_BASE) != FLASH_COMPLETE)
      return ;
  Mem.FPGA_version_length = len;
  memcpy(Mem.FPGA_version_file, mSingleton->mFTPFWClient.mFileName, 33);
  uint8_t *pMem = (uint8_t *)&Mem;
  for (uint32_t offset = 0; offset < sizeof(Mem); offset+=4){
    if (FLASH_ProgramWord(offset + BOOT_LOADER_BASE, *(uint32_t *)(pMem +offset)) != FLASH_COMPLETE)
      return ;
  }
    
    
}
*/

void upgradeManager::StartSWUpgrade(uint32_t len)
{
  if (Check_New_Version(len) != 0)
  {
    mSWStatus = UPDATE_VERIFY_FAIL;
    return;
  }
  if (Set_Root_Location(VERSION_DOWNLOAD_BASE, len) != 0)
  {
    mSWStatus = UPDATE_FAIL;
    Set_Root_Location(ACTIVE_VERSION_BASE, 0);
    return;
  }
  mSWStatus = UPDATE_SUCCESSFUL;

}
  
int upgradeManager::On_DataReceived(Buffer& buf, uint16_t block)
{
  return mSingleton->SaveData(buf, block);
}

/*
int upgradeManager::On_FWDataReceived(Buffer& buf, uint16_t block)
{
  int res = FPGA_Update::savePage(buf,block);
  switch (res)
  {
  case -1:
    mSingleton->mFWStatus = UPDATE_GENERAL_FAIL;
    return -1;
  case -2:
     mSingleton->mFWStatus = UPDATE_VERIFY_FAIL;
    return -1;
  default:
    return res;
    
  }
  
}
*/

int upgradeManager::SaveData(Buffer& buf, uint16_t block)
{
  uint32_t word;
  int rc = 0;
  
  FlashUnlock unlocker;
  int i = 0;
  while (buf.getDataSize() > 4){
    if ( ((VERSION_DOWNLOAD_BASE+ (block-1)*FTP_BLOCK_SIZE + i) & (FLASH_BLOCK_SIZE -1 ))  == 0)
      if (FLASH_ErasePage(VERSION_DOWNLOAD_BASE+ (block-1)*FTP_BLOCK_SIZE + i) != FLASH_COMPLETE)
        return -2;
    buf.readBlock((char *)&word,4);
    buf.advanceRead(4);
    if (FLASH_ProgramWord( VERSION_DOWNLOAD_BASE+ (block-1)*FTP_BLOCK_SIZE + i, word ) != FLASH_COMPLETE)
      return -3;
    i+=4;
  }
  word = 0xFFFFFFFF;
  buf.readBlock((char *)&word,buf.getDataSize());
  rc = buf.getDataSize() + i;
  if (FLASH_ProgramWord( VERSION_DOWNLOAD_BASE+ (block-1)*FTP_BLOCK_SIZE + i, word ) != FLASH_COMPLETE)
    return -3;
  return rc;
  
}

int upgradeManager::Check_New_Version(uint32_t len)
{
  if (strcmp((const char*)MAGIC_NUM_LOCATION, (const char*)(VERSION_DOWNLOAD_BASE + 0x150)) != 0)
    return -1;
  uint16_t crc = calcCRC((const unsigned char*)(VERSION_DOWNLOAD_BASE), len -2);
  if (crc != ntohs( *(uint16_t*)(VERSION_DOWNLOAD_BASE + len -2)))
    return -2;
  return 0;
}
int upgradeManager::Copy_New_Version(uint32_t len) // Copy and compare
{
  FlashUnlock unlocker;
  for (uint32_t add = ACTIVE_VERSION_BASE; add < VERSION_DOWNLOAD_BASE; add +=0x800){
    if (FLASH_ErasePage(add) != FLASH_COMPLETE)
      return -1;    
  }
  for (uint32_t offset = 0; offset < (len+3); offset+=4){
    if (FLASH_ProgramWord(offset + ACTIVE_VERSION_BASE, *(uint32_t *)(VERSION_DOWNLOAD_BASE + offset)) != FLASH_COMPLETE)
      return -2;
  }
  
  return 0;
  
}
int upgradeManager::Set_Root_Location(uint32_t Address, uint32_t length) 
{
  Boot_Memory_Type Mem;
  FlashUnlock unlocker;
  memcpy(&Mem, (void *)BOOT_LOADER_BASE, sizeof(Mem));
  if (FLASH_ErasePage(BOOT_LOADER_BASE) != FLASH_COMPLETE)
      return -1;
  Mem.bootLocation = Address;
  Mem.new_version_length = length;
  memcpy(Mem.new_version_file, mFTPSWClient.mFileName, 33);
  uint8_t *pMem = (uint8_t *)&Mem;
  for (uint32_t offset = 0; offset < sizeof(Mem); offset+=4){
    if (FLASH_ProgramWord(offset + BOOT_LOADER_BASE, *(uint32_t *)(pMem +offset)) != FLASH_COMPLETE)
      return -2;
  }

  return 0;
}
  




