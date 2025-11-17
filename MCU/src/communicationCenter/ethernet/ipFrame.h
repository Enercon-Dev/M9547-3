// **********************************************************************
// **********************************************************************
// Copyright 2011 by Avi Owshanko. All rights reserved.
// **********************************************************************
// **********************************************************************

#ifndef IP_FRAME_H
#define IP_FRAME_H

#include <stdint.h>
#include "frame.h"
#include "ethernetCommon.h"

// predeclaration
class InFrame;
class OutFrame;
class Layer3;
class CommunicationCenter;
class Arp;

// *******************************************************************
// A class for decoding the IP header
// *******************************************************************
class IPHeader
{
protected:
  uint32_t mSrcIP;
  uint32_t mDstIP;
  unsigned int mHeaderLen;
  unsigned int mDataLen;
  IPFrameType mFrameType;
  const char* mHeaderBase;

  IPHeader(InFrame& inFrame, Arp&);

private:
  bool isToMe( uint32_t IP);
  bool readLength(InFrame& inFrame);
  bool readIPs(InFrame& inFrame);
  void decodeOptions(InFrame& inFrame);
  void decodeFrameType(uint16_t protocol);
};

// *******************************************************************
// A class for holding and accessing the IP data
// *******************************************************************
class IPFrame : protected IPHeader
{
public:
  IPFrame(InFrame& inFrame, Arp& arp);

  IPFrameType getMsgType() const
    {return mFrameType;}
  uint32_t getSrcIP() const
    {return mSrcIP;}
  uint32_t getDstIP() const
    {return mDstIP;}
  unsigned int getMsgLen() const
    {return mDataLen - mOffset;}
  const char* getCurDataPtr() const
    {return (mBase + mOffset);}
  const char* getHeaderBase() const
    {return mHeaderBase;}

private:
  const char* mBase;
  unsigned int mOffset;
};

// *******************************************************************
// A class for sending an ip message
// *******************************************************************
class OutIPMsg : protected OutFrame
{
public:
  OutIPMsg(uint32_t targetIP, IPFrameType frameTypes, CommunicationCenter&);
  OutIPMsg(uint32_t targetIP,
           unsigned int optionsLen,         // this is a ping reply - IP_ICMP_TYPE
           const char* optionsBase,
           CommunicationCenter&);

  int sendMsg();

  char* getCurPtr();
  int writeHostU16(uint16_t val);
  int appendData(const char* blockBase, unsigned int blockSize);
  int appendData(const Buffer& src, unsigned int blockSize);
  int appendData(const Buffer& src, unsigned int bufferOffset, unsigned int blockSize);

private:
  void updateIPMsgLen();
  void writeIPHeader(uint32_t targetIP,
                     IPFrameType frameType,
                     unsigned int optionsLen,
                     const char* optionsBase);
  void writeFrameType(IPFrameType frameType);

  void parseOptions(char* optionsBase, unsigned int optionsLen);
  void updateRecordRoute(char* optionsBase, unsigned int optionsLen, unsigned int offset);
  void updateInternetTimestamp(char* optionsBase, unsigned int optionsLen, unsigned int offset);

  static uint16_t getIdentification();

  static uint16_t mIdentification;
  char* mIPHeaderBase;
};

#endif // IP_FRAME_H