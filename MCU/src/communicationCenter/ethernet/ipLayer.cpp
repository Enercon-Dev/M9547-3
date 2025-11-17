// **********************************************************************
// **********************************************************************
// Copyright 2011 by Avi Owshanko. All rights reserved.
// **********************************************************************
// **********************************************************************

#include "ipLayer.h"
#include "ipFrame.h"
#include "frame.h"
#include "globalInterface.h"
#include "arp.h"
#include "Layer3.h"
#include "timeAcc.h"
#include "../../persistentData/persistentDataCenter.h"
#include "../communicationCenter.h"
#include "mac.h"
#include "systemManagement.h"
#include <string.h>

// **********************************************************************
// **********************************************************************
// **************************   IP Layer      ***************************
// **********************************************************************
// **********************************************************************

// **************************************************************
// Constructor
// **************************************************************
IPLayer::IPLayer(Layer3& tcpLayer,
                 Layer3& udpLayer,
                 CommunicationCenter& communicationCenter) :
  mArp(),
  mTCPLayer(tcpLayer),
  mUDPLayer(udpLayer),
  mCommunicationCenter(communicationCenter)
{
}

// **************************************************************
// Receives layer 2 packet, and processes it if IP / ARP message.
// **************************************************************
int IPLayer::handleFrame(InFrame& inFrame)
{
  switch(inFrame.getType()) {
  case FRAME_IP_TYPE:
    if (inFrame.isBroadcast()) {
      //return -1;
    }
    return handleIPFrame(inFrame);
  case FRAME_ARP_TYPE:
    return mArp.handleArpFrame(inFrame);
  default:
    break;
  }

  return -2;
}

// **************************************************************
// This method does the actual IP layer processing.
// First parses the IP header, and then either process ICMP messages,
// or passes message on to the upper layers.
// **************************************************************
int IPLayer::handleIPFrame(InFrame& inFrame)
{
  const IPFrame ipFrame(inFrame, mArp);
  
  switch (ipFrame.getMsgType()) {
  case IP_ICMP_TYPE:
    return handleICMPFrame(ipFrame);
  case IP_TCP_TYPE:
    return mTCPLayer.handleFrame(ipFrame);
  case IP_UDP_TYPE:
    return mUDPLayer.handleFrame(ipFrame);
  }

  // If we reached this point, we had header decoding problems
  // or just an irrelevant incoming message.
  return -1;
}

// **************************************************************
// Processes an ICMP message.
// **************************************************************
int IPLayer::handleICMPFrame(const IPFrame& ipFrame)
{
  unsigned int dataLen = ipFrame.getMsgLen();
  // read message type, and skip over crc field.
  const char* dataBase = ipFrame.getCurDataPtr();
  if (mCommunicationCenter.getIPAddr() == 0) return -2;
  uint16_t icmpType = hGetU16(dataBase, 0);
  // We handle only echo (ping) requests currently.
  if (icmpType != 0x0008) {
    return -2;
  }

  const char* optionsBase = ipFrame.getHeaderBase() + 20;
  unsigned int optionLen = dataBase - optionsBase;
  
  uint32_t targetIP = ipFrame.getSrcIP();
  OutIPMsg outMsg(targetIP, optionLen, optionsBase, mCommunicationCenter);
  outMsg.writeHostU16(0x0000);
  char* checksumPtr = outMsg.getCurPtr();
  outMsg.writeHostU16(0x0000);
  outMsg.appendData(dataBase+4, dataLen-4);

  // We need to update the 'ping' message checksum.
  *(uint16_t*)(checksumPtr) = calcChecksum(checksumPtr-2, dataLen, 0, 0);

  return outMsg.sendMsg();
}

// **************************************************************
// periodic call
// **************************************************************
void IPLayer::periodicCall()
{
  mArp.periodicCall();
}

// **************************************************************
// mediators
// **************************************************************
const MACAddr& IPLayer::getMacAddr(uint32_t ipAddr)
{
  return mArp.getMacAddr(ipAddr);
}
