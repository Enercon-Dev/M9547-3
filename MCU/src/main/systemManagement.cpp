// **********************************************************************
// **********************************************************************
// Copyright 2011 by Avi Owshanko. All rights reserved.
// **********************************************************************
// **********************************************************************

#include "systemManagement.h"
#include "stm32Interface.h"
#include "systemObjects.h"
#include "../communicationCenter/buffers/buffers.h"
#include "persistentData/persistentDataCenter.h"
#include "../Hardware/HardwareCenter.h"
#include "globalInterface.h"
#include "stm32f10x_gpio.h" // TODO: Remove


// **********************************************************************
// **********************************************************************
// ******************   System Management Factory     *******************
// **********************************************************************
// **********************************************************************

SystemManagement SystemManagement::mGlobalManagement;

// **********************************************************************
// **********************************************************************
// *********************   System Management      ***********************
// **********************************************************************
// **********************************************************************

SystemManagement::SystemManagement() :
  mPriority0(0),
  mPriority1(0),
  mPriority2(0),
  mRebootTimeLimit(0),
  mState(SYSTEM_EXECUTE_ONESHOTS),
  mCommunicationCenter(SystemObjects::getSingleton().mCommunicationCenter),
  mPersistentDataCenter(SystemObjects::getSingleton().mPersistentData)
 // mHardwareCenter(0)
{
}

// **************************************************************
// This is the system initialization
// **************************************************************
void SystemManagement::init()
{

  stm32SystemInit();
 
  // persistent data must be loaded after the stm32 initialization, but before 
  // all the other actions.
  SystemObjects::getSingleton().mPersistentData.loadPersistentData();
  ethernetMacInit();
  ethernetMACConfig();
 
  HardwareCenter::GetInstance().Init();
  SystemObjects::getSingleton().mCommunicationCenter.openCenter();

  mGlobalManagement.mPriority0 = 0;
  mGlobalManagement.mPriority1 = 0;
  mGlobalManagement.mPriority2 = 0;
}

// **************************************************************
// This is the system periodic call
// **************************************************************
void SystemManagement::periodicCall()
{
  // call priorities in reverse, so that we will not have two
  // consequtive non priority '0' actions.
  mGlobalManagement.tryPriority2Actions();
  mGlobalManagement.tryPriority1Actions();
  mGlobalManagement.tryPriority0Actions();
}

// **************************************************************
// prioirty 0 periodic call
// tries to handle as many as 10 incoming packets (if pending)
// **************************************************************
void SystemManagement::tryPriority0Actions()
{
  mPriority0++;
  handleIncomingPackets();
}

// **************************************************************
// prioirty 1 periodic call
// **************************************************************
void SystemManagement::tryPriority1Actions()
{
  if (mPriority0 < PRIORITY_0_NICE) {
    return;
  }
  mPriority0 = 0;
  mPriority1++;

  SystemObjects::getSingleton().mCommunicationCenter.periodicCall(1);
}

// **************************************************************
// prioirty 2 periodic call
// **************************************************************
void SystemManagement::tryPriority2Actions()
{
  if (mPriority1 < PRIORITY_1_NICE) {
    return;
  }
  mPriority1 = 0;
  mPriority2++;

  if (mState & SYSTEM_EXECUTE_ONESHOTS) {
    uint32_t time = getTimerTicks();
    if (time > ETHERNET_WAKEUP_TIME) {
      doOneshots();
      mState &= ~SYSTEM_EXECUTE_ONESHOTS;
    }
    //return;
  }

  switch (mPriority2Sequencer) {
  case 0:
    //SystemObjects::getSingleton().mSwitchManager.periodicCall();
    HardwareCenter::GetInstance().periodicCall();
    break;
  default:
    SystemObjects::getSingleton().mCommunicationCenter.periodicCall(2);
    if (SystemObjects::getSingleton().mCommunicationCenter.getIPAddr() == 0)
      HardwareCenter::GetInstance().CommStat = 0;
    else
      HardwareCenter::GetInstance().CommStat = 1;
    break;
  }
  mPriority2Sequencer = (mPriority2Sequencer + 1) % 2;

  // If reset is pending, try a soft reset, buf for a reboot anyway after timeout.
  if ((mState & SYSTEM_RESET_PENDING) != 0) {
    if (allAreClosed() || (getLeaseTime(mRebootTimeLimit) < 0)) {
      systemReset();
    }
  }  
 
}

// **************************************************************
// Tests if we received any 'keep alive' notification in the past cycle.
// **************************************************************
bool SystemManagement::allAreClosed() const
{
  return SystemObjects::getSingleton().mCommunicationCenter.allAreClosed();
}

// **************************************************************
// Actions we do once after initialization.
// **************************************************************
void SystemManagement::doOneshots()
{

}

// **************************************************************
// Marks system as closing
// **************************************************************
void SystemManagement::registerReboot()
{
  if ((mGlobalManagement.mState & SYSTEM_RESET_PENDING) != 0) {
    return;
  }

  mGlobalManagement.mState |= SYSTEM_RESET_PENDING;
  mGlobalManagement.mRebootTimeLimit = getTimerLease(SYSTEM_SHUTDOWN_DELAY);
  SystemObjects::getSingleton().mCommunicationCenter.closeCenter();
}

// **************************************************************
// mediators
// **************************************************************
/*void SystemManagement::resetPort(SerialPortNum port)
{
  return mGlobalManagement.mCommunicationCenter.resetPort(port);
}
*/
// **************************************************************
// mediators
// **************************************************************
int SystemManagement::handleLeyer2Frame(InFrame& frame)
{
  return mGlobalManagement.mCommunicationCenter.handleLeyer2Frame(frame);
}
/*
// **************************************************************
// mediators
// **************************************************************
const SerialPhysicalStreamSettings& SystemManagement::getSerialPhysicalStreamSettings(SerialPortNum port)
{
  return SystemObjects::getSingleton().mPersistentData.readSerialPhysicalStreamSettings(port);
}

// **************************************************************
// mediators
// **************************************************************
const SerialBufferStreamSettings& SystemManagement::getSerialBufferStreamSettings(SerialPortNum port)
{
  return SystemObjects::getSingleton().mPersistentData.readSerialBufferStreamSettings(port);
}

// **************************************************************
// mediators
// **************************************************************
const EthernetStreamSettings& SystemManagement::getEthernetStreamSettings(SerialPortNum port)
{
  return SystemObjects::getSingleton().mPersistentData.readEthernetStreamSettings(port);
}

// **************************************************************
// mediators
// **************************************************************
SwitchPortConnectionType SystemManagement::getEthernetPortSettings(SwitchPortTypes port)
{
  return SystemObjects::getSingleton().mPersistentData.readEthernetPortSettings(port);
}

// **************************************************************
// mediators
// **************************************************************
void SystemManagement::updateSerialPhysicalStreamSettings(SerialPortNum port,
                                                          const SerialPhysicalStreamSettings& newSettings)
{
  return SystemObjects::getSingleton().mPersistentData.writeSerialPhysicalStreamSettings(port, newSettings);
}

// **************************************************************
// mediators
// **************************************************************
void SystemManagement::updateSerialBufferStreamSettings(SerialPortNum port,
                                                        const SerialBufferStreamSettings& newSettings)
{
  return SystemObjects::getSingleton().mPersistentData.writeSerialBufferStreamSettings(port, newSettings);
}

// **************************************************************
// mediators
// **************************************************************
void SystemManagement::updateEthernetStreamSettings(SerialPortNum port,
                                                    const EthernetStreamSettings& newSettings)
{
  return SystemObjects::getSingleton().mPersistentData.writeEthernetStreamSettings(port, newSettings);
}

// **************************************************************
// mediators
// **************************************************************
void SystemManagement::updateEthernetPortSettings(SwitchPortTypes port,
                                                  SwitchPortConnectionType newSettings)
{
  return SystemObjects::getSingleton().mPersistentData.writeEthernetPortSettings(port, newSettings);
}

// **************************************************************
// mediators
// **************************************************************
const SwitchConnectionState& SystemManagement::getEthernetPortState(SwitchPortTypes port)
{
  return SystemObjects::getSingleton().mSwitchManager.getPortState(port);
}
*/
