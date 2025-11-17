// **********************************************************************
// **********************************************************************
// Copyright 2011 by Avi Owshanko. All rights reserved.
// **********************************************************************
// **********************************************************************

#ifndef LAYER3_CONNECTION_H
#define LAYER3_CONNECTION_H

#include <stdint.h>
#define MAX_LAYER3_MESSAGE (512)

// *******************************************************************
// virtual connection base class
// *******************************************************************
class Layer3Connection
{
public:
  virtual int handleIncomingMsg(uint32_t srcIPAddr, uint32_t dstIPAddr,
                                const unsigned char* msgBase,
                                unsigned int msgLen) = 0;
  virtual void periodicCall() = 0;
  virtual bool isClosed() const = 0;
  virtual uint32_t getOtherIP() const = 0;

  static uint32_t calcBaseChecksum(uint32_t srcIP, uint32_t dstIP, bool isTCP);
};


#endif // LAYER3_CONNECTION_H