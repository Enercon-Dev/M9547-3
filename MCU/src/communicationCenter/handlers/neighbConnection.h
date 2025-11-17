// **********************************************************************
// **********************************************************************
// Copyright 2011 by Avi Owshanko. All rights reserved.
// **********************************************************************
// **********************************************************************

#ifndef NEIGHB_CONNECTION_H
#define NEIGHB_CONNECTION_H

#include "../ethernet/Layer3Connection.h"

// predeclarations
class Layer3;

// *******************************************************************
// A class for handling udp messages
// *******************************************************************
class neighbConnection : public Layer3Connection
{
public:
  neighbConnection(Layer3&);

  // 'Layer3Connection' implementation
  virtual int handleIncomingMsg(uint32_t srcIP, uint32_t dstIP,
                                const unsigned char* msgBase,
                                unsigned int msgLen);
  virtual void periodicCall();
  virtual bool isClosed() const;
  virtual uint32_t getOtherIP() const {return 0;}

  void closeObject();
  void openObject();
  const uint64_t* getMACList() const {return FilterMACs;}

private:

  Layer3& mUDPLayer;
  void insertIP(uint32_t IP);
  uint64_t FilterMACs[64];
  uint8_t Next;
};


#endif // NEIGHB_CONNECTION_H