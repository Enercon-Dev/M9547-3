// **********************************************************************
// **********************************************************************
// Copyright 2011 by Avi Owshanko. All rights reserved.
// **********************************************************************
// **********************************************************************

#ifndef UDP_CONNECTION_H
#define UDP_CONNECTION_H

#include "Layer3Connection.h"

// predeclarations
class BidirectionalBuffer;
class Layer3;

// *******************************************************************
// A class for handling udp messages
// *******************************************************************
class UDPConnection : public Layer3Connection
{
public:
  UDPConnection(BidirectionalBuffer&, Layer3&);

  // 'Layer3Connection' implementation
  virtual int handleIncomingMsg(uint32_t srcIP, uint32_t dstIP,
                                const unsigned char* msgBase,
                                unsigned int msgLen);
  virtual void periodicCall();
  virtual bool isClosed() const;
  virtual uint32_t getOtherIP() const {return mTargetIP;}

  void closeObject();
  void openObject(uint16_t mySocket, uint16_t targetSocket, uint32_t targetIP);
  bool isObjectClosed() const {return !mObjectOpen;}
  void changeTargetSocket(uint16_t socket) { mTargetSocket = socket;}
  void changeTargetIP(uint32_t ip) { mTargetIP = ip;}
  uint16_t LastRecievedSocket;
  uint32_t LastRecievedIP;

private:
  void sendUDPMessage();

  uint16_t mMySocket;
  uint16_t mTargetSocket;
  uint32_t mTargetIP;

  Layer3& mUDPLayer;
  BidirectionalBuffer& mBuffers;
  bool mObjectOpen;
};








#endif // UDP_CONNECTION_H