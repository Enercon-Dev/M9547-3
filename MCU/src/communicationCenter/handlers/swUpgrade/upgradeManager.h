#ifndef _UPGRADE_MANAGER_H
#define _UPGRADE_MANAGER_H

#include "globalInterface.h"
#include "FTP.h"
#include "../handlersCommon.h"
#include "persistentCommon.h"

typedef enum{ UPDATE_READY = 0x01, UPDATE_BUSY = 0x15, UPDATE_SUCCESSFUL = 0x1F, UPDATE_FAIL = 0x29, UPDATE_TOO_LARGE = 0x2A, UPDATE_GENERAL_FAIL = 0x2B, UPDATE_VERIFY_FAIL = 0x2C} UPDATE_STATUS; 


uint16_t calcCRC(const unsigned char* buffer, uint16_t buf_length);

class upgradeManager
{
public:
  upgradeManager(Layer3& udpLayer);
  
  // upper layers requests
  void periodicCall(){  /*mFTPFWClient.periodicCall();*/ mFTPSWClient.periodicCall();  }
  void closeSocket(){  /*mFTPFWClient.closeSocket();*/ mFTPSWClient.closeSocket();  }
  void openSocket(){  /*mFTPFWClient.openSocket();*/ mFTPSWClient.openSocket();  }
  bool isClosed() const{  return mFTPSWClient.isClosed() /*&& mFTPFWClient.isClosed()*/; }
  
  //COMMAND_SUCCESS       handleSetFirmwareUpdate(Buffer& DataIn);
  //void                  handleGetFirmwareInfo(Buffer& DataOut);
  
  COMMAND_SUCCESS       handleSetSoftwareUpdate(Buffer& DataIn);
  void                  handleGetSoftwareInfo(Buffer& DataOut);
  
private:
  static void On_StartSWUpgrade(uint32_t len);
   void StartSWUpgrade(uint32_t len);
   
   //static void On_StartFWUpgrade(uint32_t len);
   
   static int On_DataReceived(Buffer& buf, uint16_t block);
   //static int On_FWDataReceived(Buffer& buf, uint16_t block);
   int SaveData(Buffer& buf, uint16_t block);

   int Check_New_Version(uint32_t len);
   int Copy_New_Version(uint32_t len); // Copy and compare
   int Set_Root_Location(uint32_t Address, uint32_t len); 
  static upgradeManager* mSingleton;
  //upgradeManager(){};
  //UPDATE_STATUS mFWStatus;
  UPDATE_STATUS mSWStatus;
  
  FTPClient mFTPSWClient;
  //FTPClient mFTPFWClient;

};



#endif