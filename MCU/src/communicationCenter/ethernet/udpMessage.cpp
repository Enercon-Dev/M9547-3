// **********************************************************************
// **********************************************************************
// Copyright 2011 by Avi Owshanko. All rights reserved.
// **********************************************************************
// **********************************************************************

#include "udpMessage.h"
#include "layer3Connection.h"
#include "globalInterface.h"
#include "../communicationCenter.h"
#include <string.h>

// **********************************************************************
// **********************************************************************
// **************************   TCP Header    ***************************
// **********************************************************************
// **********************************************************************

// **************************************************************
// UDP Header constructor
// Decodes an incoming UDP message header.
// Set 'mTargetSocket' to '0' on error
// **************************************************************
UDPHeaderData::UDPHeaderData(const unsigned char* msgBase,
                             uint32_t srcIP, uint32_t dstIP,
                             unsigned int msgLen) :
  mTargetSocket(0)
{
  if (msgLen < UDP_HEADER_LEN) {
    return;
  }

  mSourceSocket = hGetU16(msgBase, 0);
  uint16_t targetSocket = hGetU16(msgBase, 2);
  mMsgLen = hGetU16(msgBase, 4);
  mMsgLen = ntohs(mMsgLen);
  if (mMsgLen > msgLen) {
    return;
  }

  // UDP tests checksum only if field is not '+0'. Use '-0' instead.
  // this is opposed to TCP, as well as to IP protocols.
  uint16_t checksum = hGetU16(msgBase, 6);
  if (checksum != 0) {
    // note that we turn message offset to network order in the virtual message
    // so we do not need to apply 'htons' on 'msgOffset'.
    uint32_t virtualChecksum = Layer3Connection::calcBaseChecksum(srcIP, dstIP, false);
    virtualChecksum = virtualChecksum + mMsgLen;
    /*if (calcChecksum((const char*)msgBase, msgLen, virtualChecksum, 0) != 0) {
      return;
    }*/
  }

  mTargetSocket = targetSocket;
}

// **************************************************************
// UDP Header constructor
// Used for outbound messages.
// **************************************************************
UDPHeaderData::UDPHeaderData(uint16_t sourceSocket,
                             uint16_t targetSocket) :
  mSourceSocket(sourceSocket),
  mTargetSocket(targetSocket),
  mMsgLen(0)
{
}

// **********************************************************************
// **********************************************************************
// ********************** Outbound UDP Message    ***********************
// **********************************************************************
// **********************************************************************

// **************************************************************
// Initiates a TCP output message.
// Will initiate the IP header, and write the TCP header.
// TCP checksum is left empty, as it depends on message length.
// Checksums will be updates when sending message.
// **************************************************************
OutUDPMsg::OutUDPMsg(uint32_t targetIP,
                     const UDPHeaderData& udpHeaderData,
                     CommunicationCenter& communicationCenter) :
    OutIPMsg(targetIP, IP_UDP_TYPE, communicationCenter),
    mUDPHeaderBase(NULL)
{
  if (isBad()) {
    return;
  }

  // Mark where the TCP header starts.
  mUDPHeaderBase = OutIPMsg::getCurPtr();
  writeUDPHeader(udpHeaderData);

  mVirtualChecksum = Layer3Connection::calcBaseChecksum(targetIP, communicationCenter.getIPAddr() , false);
}

// **************************************************************
// Constructs a simple UDP outbound packet
// **************************************************************
void OutUDPMsg::writeUDPHeader(const UDPHeaderData& udpHeaderData)
{
  if (isBad()) {
    return;
  }

  writeHostU16(udpHeaderData.mSourceSocket);
  writeHostU16(udpHeaderData.mTargetSocket);
  // length place holder
  writeHostU16(0);
  // crc place holder
  writeHostU16(0);
}

// **************************************************************
// this methods removes all previous data in message
// we use this option for writing error data (and removing previous data).
// returns the number of bytes removed
// **************************************************************
int OutUDPMsg::resetMsg()
{
  if (isBad()) {
    return -1;
  }

  uint16_t msgLen = getCurPtr() - mUDPHeaderBase - UDP_HEADER_LEN;
  removeData(msgLen);
  return msgLen;
}

// **************************************************************
// The method for sending this tcp message.
// will return number of bytes sent on success,
// negative number on failure.
// **************************************************************
int OutUDPMsg::sendMsg()
{
  if (isBad()) {
    return -1;
  }

  updateChecksum();
  return OutIPMsg::sendMsg();
}

// **************************************************************
// Invokes the IP layer 'appendData' method
// **************************************************************
int OutUDPMsg::appendData(const Buffer& src, unsigned int blockSize)
{
  return OutIPMsg::appendData(src, blockSize);
}

// **************************************************************
// Updates the checksum for this TCP message after appending all data.
// **************************************************************
void OutUDPMsg::updateChecksum()
{
  if (isBad()) {
    return;
  }

  if (mUDPHeaderBase == NULL) {
    setBad();
    return;
  }

  uint16_t msgLen = getCurPtr() - mUDPHeaderBase;
  *(uint16_t*)(mUDPHeaderBase+4) = htons(msgLen);

  // note that we turn message offset to network order in the virtual message
  // so we do not need to apply 'htons' on 'msgLen'.
  uint32_t virtualChecksum = mVirtualChecksum + msgLen;
  uint16_t checkSum = calcChecksum(mUDPHeaderBase, msgLen, virtualChecksum, 0);
  if (checkSum == 0) {
    // turn '+0' to '-0' according to UDP protocol.
    checkSum = 0xFFFF;
  }
  *(uint16_t*)(mUDPHeaderBase+6) = checkSum;
}
