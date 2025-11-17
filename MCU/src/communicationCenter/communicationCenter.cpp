// **********************************************************************
// **********************************************************************
// Copyright 2011 by Avi Owshanko. All rights reserved.
// **********************************************************************
// **********************************************************************

#include "communicationCenter.h"
#include "systemManagement.h"
#include "stm32Interface.h"


// **********************************************************************
// **********************************************************************
// *******************   Communication Center      **********************
// **********************************************************************
// **********************************************************************

// **************************************************************
// constructor
// **************************************************************
CommunicationCenter::CommunicationCenter() :
  mTCPLayer(*this),
  mUDPLayer(*this),
  mIPLayer(mTCPLayer, mUDPLayer, *this),
  LB_Address(),
  mIPData(),
  mDHCPClient(mIPData,mUDPLayer),
  mUpgradeManager(mUDPLayer),
  mController(mUDPLayer, *this)/*
  mUpgradeAgent(mUDPLayer, *this)*/
{
}

// **************************************************************
// constructor
// **************************************************************
void CommunicationCenter::openCenter()
{
  IpSettings_t Ips;
  SystemManagement::getPersistentDataCenter().Get_IpSetting(Ips); 
  if (Ips.IpType == 0)
    mDHCPClient.openSocket();
  else
  {
    mIPData.mAddr = Ips.IP;
    mIPData.mMask = Ips.IpMask;
  }
  mUpgradeManager.openSocket();
  mController.openSocket();
}

// **************************************************************
// Communication center periodic call
// **************************************************************
void CommunicationCenter::periodicCall(int priority)
{
  // let telnet server come first, as it may have commands for the serial ports.
  // telnet and http servers are lower priority
  if (priority >= 2) {
    mDHCPClient.periodicCall();
    if (mIPData.mAddr != 0){
      mUpgradeManager.periodicCall();
      mController.periodicCall();
    }
    return;
  }

  // Layer2 periodic call
  mIPLayer.periodicCall();

  // handle the tcp and udp layers last, so they will send current answers.
  mTCPLayer.periodicCall();
  mUDPLayer.periodicCall();
}

// **************************************************************
// mediators
// **************************************************************
int CommunicationCenter::handleLeyer2Frame(InFrame& frame)
{
  return mIPLayer.handleFrame(frame);
}

// **************************************************************
// mediators
// **************************************************************
const MACAddr& CommunicationCenter::getMacAddr(uint32_t ipAddr)
{
  return mIPLayer.getMacAddr(ipAddr);
}

// **************************************************************
// Checks if all connections are closed
// **************************************************************
bool CommunicationCenter::allAreClosed() const
{
    if (!mDHCPClient.isClosed()) {
    return false;
  }
  // do not test 'mUpgradeAgent' as it is always open.

  return true;
}

// **************************************************************
// Notifies all connections to close
// **************************************************************
void CommunicationCenter::closeCenter()
{
  mDHCPClient.closeSocket();
  
}

//Upgrade Manager
upgradeManager& CommunicationCenter::getUpgradeMan()
{
  return mUpgradeManager;
}
