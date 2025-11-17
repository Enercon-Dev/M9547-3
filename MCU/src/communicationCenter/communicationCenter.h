// **********************************************************************
// **********************************************************************
// Copyright 2011 by Avi Owshanko. All rights reserved.
// **********************************************************************
// **********************************************************************

#ifndef COMMUNICATION_CENTER_H
#define COMMUNICATION_CENTER_H

//#include "handlers/telnet.h"
#include "handlers/DHCP.h"
#include "handlers/Control.h"
#include "handlers/neighbConnection.h"
#include "handlers/swUpgrade/upgradeManager.h"
#include "ethernet/ipLayer.h"
#include "ethernet/Layer3.h"
#include "ethernet/mac.h"
//#include "serialPort/serialCenter.h"
//#include "mgmtMsgUdpSingleton.h"

//class upgradeManager;
//class Controller;

// *******************************************************************
// A class for handling all system communication layer
// *******************************************************************
class CommunicationCenter
{
public:
  CommunicationCenter();

  // up interface
  void periodicCall(int priority);
  int handleLeyer2Frame(InFrame& frame);
  
  bool allAreClosed() const;
  void closeCenter();
  void openCenter();

  const MACAddr& getMacAddr(uint32_t ipAddr);
  uint32_t getIPAddr() const {return mIPData.mAddr; }
  const IPData& getIPData() const { return mIPData; }
  upgradeManager& getUpgradeMan();
    

private:
  Layer3 mTCPLayer;
  Layer3 mUDPLayer;
  IPLayer mIPLayer;
  ArpEntry LB_Address;

  int mPeriodicCallIndex;           // used for schedualing
  DHCPClient mDHCPClient;
  upgradeManager mUpgradeManager;
  Controller mController;

  IPData mIPData;
/*  TelnetServer mTelnetServer;
  HttpServer mHttpServer;
  SerialCenter mSerialCenter;
  MgmtMsgUdpSingleton mUpgradeAgent;*/
};
  


#endif // COMMUNICATION_CENTER_H