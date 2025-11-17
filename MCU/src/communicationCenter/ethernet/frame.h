// **********************************************************************
// **********************************************************************
// Copyright 2011 by Avi Owshanko. All rights reserved.
// **********************************************************************
// **********************************************************************

#ifndef FRAME_H
#define FRAME_H

#include <stdint.h>
#include "ethernetGlobal.h"
#include "mac.h"


typedef enum {FRAME_INVALID_TYPE, FRAME_IP_TYPE, FRAME_ARP_TYPE} FrameType;

// predeclaration
class IPFrame;
class Buffer;
class CommunicationCenter;

// *******************************************************************
// Class for processing incoming frames
// *******************************************************************
class InFrame
{
public:
  InFrame(const char* messageBase, unsigned int frameLen);

  bool isBroadcast() const;
  FrameType getType() const
    {return mFrameType;}
  unsigned int getDataLen() const
    {return mLen-mOffset;}
  const char* getCurDataPtr() const
    {return (mBase+mOffset);}
  const MACAddr& getSrcMAC() const
    {return mSrcMAC;}

  uint16_t readNextNetworkU16();
  uint32_t readNextNetworkU32();
  uint16_t readNextHostU16();
  uint32_t readNextHostU32();
  uint32_t readNextIP()
    {return readNextHostU32();}
  const char* readNextMAC();

private:
  const char* mBase;
  unsigned int mLen;
  unsigned int mOffset;
  const MACAddr mSrcMAC;
  FrameType mFrameType;
};

// *******************************************************************
// Class for sending a reply / data frame
// *******************************************************************
class OutFrame
{
public:
  OutFrame(FrameType type, const MACAddr& target);
  OutFrame(uint32_t targetIP, CommunicationCenter&);

  bool isGood() const {return (mThisFrame > 0);}
  bool isBad() const {return (mThisFrame == 0);}
  void setBad() {mThisFrame = 0;}
  unsigned int getSize() const {return mOffset;}
  unsigned int getFreeSpace() const
    {return (ETHERNET_MAX_MESSAGE_SIZE -  mOffset);}

  int writeNetworkU16(uint16_t val);
  int writeNetworkU32(uint32_t val);
  int writeHostU16(uint16_t val);
  int writeHostU32(uint32_t val);
  int writeIP(uint32_t ip)
    {return writeHostU32(ip);}
  int writeMAC(const MACAddr& mac);
  int appendData(const char* blockBase, unsigned int blockSize);
  int appendData(const Buffer& src, unsigned int blockSize);
  int appendData(const Buffer& src, unsigned int bufferOffset, unsigned int blockSize);

  // used for management message handling.
  void removeData(unsigned int dataSize);

  // we need these methods for all CRC and length fields.
  char* getCurPtr()
    {return (mBase + mOffset);}
  char* getBasePtr()
    {return mBase;}

  int sendMsg();

private:
  static uint32_t mOutFrameCounter;

  char* mBase;
  unsigned int mOffset;
  uint32_t mThisFrame;

  static uint32_t getAndIncFrameCtr();
};



#endif // FRAME_H