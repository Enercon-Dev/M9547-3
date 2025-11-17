// **********************************************************************
// **********************************************************************
// Copyright 2011 by Avi Owshanko. All rights reserved.
// **********************************************************************
// **********************************************************************

#ifndef UDP_MESSAGE_H
#define UDP_MESSAGE_H

#include "ipFrame.h"

#define UDP_HEADER_LEN (8)

// *******************************************************************
// A class for holding tcp header data
// This is actually a structure with a constructor, and some helper methods.
// Data is valid only if myPort is not '0'
// *******************************************************************
class UDPHeaderData
{
public:
  UDPHeaderData(const unsigned char* msgBase, 
                uint32_t srcIP, uint32_t dstIP,
                unsigned int msgLen);
  UDPHeaderData(uint16_t sourceSocket,
                uint16_t targetSocket);

  uint16_t mSourceSocket;
  uint16_t mTargetSocket;
  unsigned int mMsgLen;

private:
  UDPHeaderData() {}
};

// *******************************************************************
// A class for sending a udp message
// *******************************************************************
class OutUDPMsg : protected OutIPMsg
{
public:
  OutUDPMsg(uint32_t targetIP,
            const UDPHeaderData& udpHeaderData,
            CommunicationCenter&);

  void setBad()   {OutIPMsg::setBad();}
  int appendData(const char* blockBase, unsigned int blockSize)
                  {return OutIPMsg::appendData(blockBase, blockSize);}
  int resetMsg();

  int sendMsg();
  int appendData(const Buffer& src, unsigned int blockSize);
  char* getCurPtr() {return OutIPMsg::getCurPtr();}

private:
  void updateChecksum();
  void writeUDPHeader(const UDPHeaderData& udpHeaderData);

  uint32_t mVirtualChecksum;
  char* mUDPHeaderBase;
};





#endif // UDP_MESSAGE_H