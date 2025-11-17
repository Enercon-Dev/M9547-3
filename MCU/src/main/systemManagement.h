// **********************************************************************
// **********************************************************************
// Copyright 2011 by Avi Owshanko. All rights reserved.
// **********************************************************************
// **********************************************************************

#ifndef SYSTEM_MANAGEMENT_H
#define SYSTEM_MANAGEMENT_H
#include "persistentData/persistentDataCenter.h"
#include "globalTypes.h"

#define SYSTEM_RESET_PENDING            (0x00000001)
#define SYSTEM_EXECUTE_ONESHOTS         (0x00000002)

#define SYSTEM_OBJECT_STATE_OPEN        (0x00000001)
#define SYSTEM_OBJECT_STATE_CLOSING     (0x00000002)
#define SYSTEM_OBJECT_STATE_CLOSED      (0x00000004)

#define PRIORITY_0_NICE (8)
#define PRIORITY_1_NICE (8)

// Following are in milliseconds
#define SYSTEM_SHUTDOWN_DELAY           (3000)

// predeclarations
class IPLayer;
class TCPLayer;
class InFrame;
class CommunicationCenter;
class MACAddr;
class DataString;
class SerialPhysicalStreamSettings;
class SerialBufferStreamSettings;
class EthernetStreamSettings;
class IPData;
class SwitchConnectionState;

// *******************************************************************
// A class for interfacing through the system
// This is actually a singlton, but constructed as a normal class.
// *******************************************************************
class SystemManagement
{
public:
  SystemManagement();

  static void init();
  
  static void periodicCall();

  static void registerReboot();
  static bool isShutingDown()
      {return ((mGlobalManagement.mState & SYSTEM_RESET_PENDING) != 0);}
  static bool oneshotsPending()
      {return ((mGlobalManagement.mState & SYSTEM_EXECUTE_ONESHOTS) != 0);}

// general actions
  static void resetPort(SerialPortNum port);

// ethernet interface
  static int handleLeyer2Frame(InFrame& frame);

// persistent objects interface
  static const SerialPhysicalStreamSettings& getSerialPhysicalStreamSettings(SerialPortNum port);
  static const SerialBufferStreamSettings& getSerialBufferStreamSettings(SerialPortNum port);
  static const EthernetStreamSettings& getEthernetStreamSettings(SerialPortNum port);
  static SwitchPortConnectionType getEthernetPortSettings(SwitchPortTypes port);
  static void updateSerialPhysicalStreamSettings(SerialPortNum port, const SerialPhysicalStreamSettings& newSettings);
  static void updateSerialBufferStreamSettings(SerialPortNum port, const SerialBufferStreamSettings& newSettings);
  static void updateEthernetStreamSettings(SerialPortNum port, const EthernetStreamSettings& newSettings);
  //static void updateEthernetPortSettings(SwitchPortTypes port, SwitchPortConnectionType);

// current state
  //static const SwitchConnectionState& getEthernetPortState(SwitchPortTypes port);

// static section
  static CommunicationCenter& getCommunicationCenter()
      {return mGlobalManagement.mCommunicationCenter;}
  static PersistentDataCenter& getPersistentDataCenter()
      {return mGlobalManagement.mPersistentDataCenter;}
 // static HardwareCenter& getHardwareCenter()
   //   {return *(mGlobalManagement.mHardwareCenter);}

private:
  void tryPriority0Actions();
  void tryPriority1Actions();
  void tryPriority2Actions();
  void doOneshots();

  bool allAreClosed() const;

  uint32_t mPriority0;
  uint32_t mPriority1;
  uint32_t mPriority2;
  unsigned char mPriority2Sequencer;
  uint32_t mRebootTimeLimit;
  uint32_t mState;

  CommunicationCenter& mCommunicationCenter;
  PersistentDataCenter& mPersistentDataCenter;
  //HardwareCenter*       mHardwareCenter;

// static section
  static SystemManagement mGlobalManagement;
};











#endif // SYSTEM_MANAGEMENT_H