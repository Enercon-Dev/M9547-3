// **********************************************************************
// **********************************************************************
// Copyright 2011 by Avi Owshanko. All rights reserved.
// **********************************************************************
// **********************************************************************

#include "arp.h"
#include "globalInterface.h"
#include "frame.h"
#include "stm32Interface.h"
#include "systemManagement.h"
#include "persistentDataCenter.h"
#include "../communicationCenter.h"
#include "genericClasses.h"
#include <string.h>

// **********************************************************************
// **********************************************************************
// **************************   ARP Entry     ***************************
// **********************************************************************
// **********************************************************************

// **************************************************************
// Constructor
// **************************************************************
ArpEntry::ArpEntry() :
  mIPAddress(0),
  mMacAddr()
{
}

// **************************************************************
// Comparator operator
// **************************************************************
bool operator==(const ArpEntry& me, uint32_t ip)
{
  if (me.ip() == ip) {
    return true;
  }
  return false;
}

// **************************************************************
// **************************************************************
void ArpEntry::refreshEntry(uint32_t ip, const MACAddr& mac)
{
  if (ip != mIPAddress) {
    // let lease expire if we changed the target IP
    mLeaseTime = getTimerLease(0);
  }

  mIPAddress = ip;
  mMacAddr = mac;
  mSendTimeout = ARP_SEND_TIMEOUT;

  if ((ip != 0) && (!(mac.isValid()))) {
    sendArpRequest();
  }
}

// **************************************************************
// This function checks whether it is time to send another ARP message.
// **************************************************************
void ArpEntry::sendArpRequest()
{
  if (getLeaseTime(mLeaseTime) > 1) {
    // We are still waiting for the previous arp reply to return
    return;
  }
  if (mIPAddress == 0) {
    return;
  }

  mLeaseTime = getTimerLease(mSendTimeout);
  if (mSendTimeout < ARP_SEND_MAX_TIMEOUT) {
    // longest timeout - about 20 minutes.
    mSendTimeout = 2*mSendTimeout;
  }

  Arp::sendArpMessage(mIPAddress, MACAddr(), false);
}

// **************************************************************
// This function checks whether it is time to send another ARP message.
// **************************************************************
int32_t ArpEntry::getThisLeaseTime() const
{
  return getLeaseTime(mLeaseTime);
}

// **********************************************************************
// **********************************************************************
// *****************************   ARP    *******************************
// **********************************************************************
// **********************************************************************

// **************************************************************
// **************************************************************
Arp::Arp() :  broadCastAddr("\xFF\xFF\xFF\xFF\xFF\xFF")
{
  int i;
  for (i=0; i<ARP_ENTRY_COUNT ; i++) {
    mNormalEntries[i] = ArpEntry();
  }

}

// **************************************************************
// **************************************************************
void Arp::updateArpEntry(uint32_t ip, const MACAddr& mac)
{
  int i;
  // Try to look if 'ip' already present in table
  for (i=0; i<ARP_ENTRY_COUNT; i++) {
    if (mNormalEntries[i] == ip) {
      mNormalEntries[i].refreshEntry(ip, mac);
      return;
    }
  }

  int freeEntry = getNextFreeEntry();
  if (freeEntry < ARP_ENTRY_COUNT) {
    mNormalEntries[freeEntry].refreshEntry(ip, mac);
  }
}

// **************************************************************
// looks for the mac address of 'ip'
// if address not valid, then will send an ARP query to find IP.
// if 'ip' is not in table, will add 'ip' to table.
// **************************************************************
const MACAddr& Arp::getMacAddr(uint32_t ip)
{
  int i;
  uint32_t mask = SystemManagement::getCommunicationCenter().getIPData().mMask;
  if ((ip | mask) == (0xFFFFFFFF | mask))
    return broadCastAddr;
  for (i=0; i<ARP_ENTRY_COUNT; i++) {
    if (mNormalEntries[i] == ip) {
      if (!(mNormalEntries[i].mac().isValid())) {
        mNormalEntries[i].sendArpRequest();
      }
      return mNormalEntries[i].mac();
    }
  }

  int freeEntry = getNextFreeEntry();
  if (freeEntry < ARP_ENTRY_COUNT) {
    // adds a new entry to table. will also send arp query.
    MACAddr dummy;
    mNormalEntries[freeEntry].refreshEntry(ip, dummy);

    return mNormalEntries[freeEntry].mac();
  }

  // should never reach this point, but just in case.
  // last entry is always invalid.
  return mNormalEntries[ARP_ENTRY_COUNT].mac();
}

// **************************************************************
// Processes an incoming ARP message.
// **************************************************************
int Arp::handleArpFrame(InFrame& inFrame)
{
  if (inFrame.getDataLen() < 28) {
    return -1;
  }

  // Verify that the message is for ethernet
  if (inFrame.readNextHostU16() != 0x0100) {
    return -2;
  }

  // Verify that the message is for IP
  if (inFrame.readNextHostU16() != 0x0008) {
    return -3;
  }

  // Verify that the message is for IPV4, and MAC6
  if (inFrame.readNextHostU16() != 0x0406) {
    return -4;
  }

  // Verify that this is either an ARP request, or an ARP answer
  uint16_t arpRequest = inFrame.readNextHostU16();
  if ((arpRequest != 0x0100) && (arpRequest != 0x0200)) {
    return false;
  }
  MACAddr srcMAC(inFrame.readNextMAC());
  uint32_t srcIP = inFrame.readNextIP();

  // Check that target MAC is either empty, or my mac address.
  MACAddr targetMAC(inFrame.readNextMAC());
  if ((!targetMAC.isEmpty()) &&
      (targetMAC != SystemManagement::getPersistentDataCenter().getMacAddr()))
  {
    //return -5;  Test
  }

  // verify that target IP is mine
  if (inFrame.readNextIP() != SystemManagement::getCommunicationCenter().getIPAddr()) {
    return -6;
  }

  updateArpEntry(srcIP, srcMAC);

  if (arpRequest == 0x0100) {
    Arp::sendArpMessage(srcIP, srcMAC, true);
  }

  return 0;
}

// **************************************************************
// place holder for future entry aging
// **************************************************************
void Arp::periodicCall()
{
}

// **************************************************************
// Looks for the oldest non open entry in array (or the first empty one)
// if none found (error), returns 'ARP_ENTRY_COUNT'
// **************************************************************
int Arp::getNextFreeEntry()
{
  int32_t oldestLease = ARP_SEND_MAX_TIMEOUT;
  int retVal = ARP_ENTRY_COUNT;

  int i;
  for (i=0; i<ARP_ENTRY_COUNT; i++) {
    if (mNormalEntries[i].ip() == 0) {
      return i;
    }

    int32_t lease = mNormalEntries[i].getThisLeaseTime();
    if (lease < oldestLease) {
      oldestLease = lease;
      retVal = i;
    }
  }

  return retVal;
}

// **************************************************************
// Sends an ARP message.
// This message can be either an ARP request, and an ARP answer
// **************************************************************
int Arp::sendArpMessage(uint32_t targetIP,
                        const MACAddr& targetMAC,
                        bool bAnswer)
{
  OutFrame arpMessage(FRAME_ARP_TYPE, targetMAC);

  if (!arpMessage.isGood()) {
    // need to protect against a busy system.
    return -1;
  }

  // An ethernet message
  arpMessage.writeHostU16(0x0100);
  // IP protocol
  arpMessage.writeHostU16(0x0008);
  // IPV4, and MAC6
  arpMessage.writeHostU16(0x0406);
  // Fill in the 'request' field
  uint16_t request = bAnswer ? 0x0200 : 0x0100;
  arpMessage.writeHostU16(request);

  // write my IP/MAC data
  arpMessage.writeMAC(SystemManagement::getPersistentDataCenter().getMacAddr());
  arpMessage.writeIP(SystemManagement::getCommunicationCenter().getIPAddr());

  // enter target IP/MAC data
  if (targetMAC.isValid()) {
    arpMessage.writeMAC(targetMAC);
  }
  else {
    // this is an ARP request - target MAC unknown.
    char emptyAddr[6];
    memset(emptyAddr, 0, 6);
    MACAddr emptyMAC(emptyAddr);
    arpMessage.writeMAC(emptyMAC);
  }
  arpMessage.writeIP(targetIP);

  return arpMessage.sendMsg();
}

// **************************************************************
// An ARP method for sending ARP initial message after startup
// **************************************************************
void Arp::sendArpHelloMsg()
{
  uint32_t myIP = SystemManagement::getCommunicationCenter().getIPAddr();
  char emptyAddr[6];
  memset(emptyAddr, 255, 6);
  MACAddr brdcstMAC(emptyAddr);               // sets MAC invalid for broadcast
  sendArpMessage(myIP, brdcstMAC, false);
}

// **************************************************************
// translator
// **************************************************************
void sendArpGratuatiousMsg()
{
  Arp::sendArpHelloMsg();
}
