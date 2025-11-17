// **********************************************************************
// **********************************************************************
// Copyright 2011 by Avi Owshanko. All rights reserved.
// **********************************************************************
// **********************************************************************

#ifndef LAYER3_H
#define LAYER3_H

#include "ethernetCommon.h"
#include "layer3Connection.h"

// predeclaration
class IPFrame;
class CommunicationCenter;

// *******************************************************************
// a pair of socket and connection (virtual base)
// *******************************************************************
struct Layer3SocketConnection
{
  uint16_t mSocket; 
  Layer3Connection* mConnection;

  uint32_t mArpLease;
  uint32_t mArpTimeout;
};

// *******************************************************************
// A singleton for handling a Layer3 messages (UDP / TCP)
// *******************************************************************
class Layer3
{
public:
  Layer3(CommunicationCenter&);

  int handleFrame(const IPFrame& ipFrame);
  void periodicCall();

  bool hasConnection(uint16_t targetSocket);

  bool addListener(uint16_t socket, Layer3Connection* connection);
  bool removeListener(uint16_t socket);

  CommunicationCenter& getCommunicationCenter() {return mCommunicationCenter;}

private:
  int getSocketIndex(uint16_t targetSocket);
  void initArpData(Layer3SocketConnection&);
  //void refreshArpEntry(Layer3SocketConnection&);

  int mPeriodicIndex;
  Layer3SocketConnection mListener[PORT_TYPES_COUNT];
  CommunicationCenter& mCommunicationCenter;
};









#endif // LAYER3_H