// **********************************************************************
// **********************************************************************
// Copyright 2011 by Avi Owshanko. All rights reserved.
// **********************************************************************
// **********************************************************************

#ifndef IP_LAYER_H
#define IP_LAYER_H

#include <stdint.h>
#include "arp.h"

// predeclaration
class InFrame;
class OutFrame;
class Layer3;
class CommunicationCenter;
class MACAddr;
class IPFrame;

// *******************************************************************
// A singleton for handling IP messages.
// *******************************************************************
class IPLayer
{
public:
  IPLayer(Layer3& tcpLayer, Layer3& udpLayer, CommunicationCenter&);
  void periodicCall();

  int handleFrame(InFrame& inFrame);
  const MACAddr& getMacAddr(uint32_t ipAddr);

private:
  int handleICMPFrame(const IPFrame& ipFrame);
  int handleIPFrame(InFrame& inFrame);

  Arp mArp;

  Layer3& mTCPLayer;
  Layer3& mUDPLayer;
  CommunicationCenter& mCommunicationCenter;
};



#endif // IP_LAYER_H