// **********************************************************************
// **********************************************************************
// Copyright 2011 by Avi Owshanko. All rights reserved.
// **********************************************************************
// **********************************************************************

#ifndef ARP_H
#define ARP_H

#include <stdint.h>
#include "mac.h"

#define ARP_ENTRY_COUNT  (7)
#define ARP_SEND_TIMEOUT (1000)
#define ARP_SEND_MAX_TIMEOUT (0x100000)

class InFrame;

// *******************************************************************
// A class for holding a single ARP entry
// *******************************************************************
class ArpEntry {
public:
  ArpEntry();

  void refreshEntry(uint32_t ip, const MACAddr& mac);
  void sendArpRequest();

  const MACAddr& mac() const {return mMacAddr;}
  uint32_t ip() const {return mIPAddress;}
  int32_t getThisLeaseTime() const;

private:
  uint32_t mIPAddress;
  MACAddr mMacAddr;

  uint32_t mLeaseTime;
  uint32_t mSendTimeout;
};

bool operator==(const ArpEntry& me, uint32_t ip);

// *******************************************************************
// A class for holding the whole ARP map
// *******************************************************************
class Arp {
public:
  Arp();

  void updateArpEntry(uint32_t ip, const MACAddr& mac);
  const MACAddr& getMacAddr(uint32_t ip);
  int handleArpFrame(InFrame& inFrame);

  int getNextFreeEntry();
  void periodicCall();

  static void sendArpHelloMsg();
  static int sendArpMessage(uint32_t targetIP, const MACAddr& targetMAC, bool bAnswer);

private:
  // keep one extra entry for padding.
  ArpEntry mNormalEntries[ARP_ENTRY_COUNT+1];
};

#endif // ARP_H