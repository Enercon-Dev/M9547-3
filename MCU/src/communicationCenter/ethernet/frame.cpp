// **********************************************************************
// **********************************************************************
// Copyright 2011 by Avi Owshanko. All rights reserved.
// **********************************************************************
// **********************************************************************

#include "frame.h"
#include "arp.h"
#include "ethernetGlobal.h"
#include "ipLayer.h"
#include "timeAcc.h"
#include "../buffers/buffers.h"
#include "../../persistentData/persistentDataCenter.h"
#include "systemManagement.h"
#include "../communicationCenter.h"
#include "debugHelpers.h"
#include <string.h>

static TimeAcc ethMsgAcc;

// **************************************************************
// A short waiting loop - about 3 micro seconds
// **************************************************************
static void shortDelay()
{
  __IO uint32_t i = 0;
  for(i = 0xFF; i != 0; i--)
  {
  }
}

// **************************************************************
// This is the main ethernet handling function.
// It first updates entry in the ARP table, and then passes the message
// to the relevant entity.
// **************************************************************
int processEthernetMsg(const char* messageBase,
                       unsigned int frameLen)
{
  TimeAccWrapper ackAcc(ethMsgAcc);

  InFrame inFrame(messageBase, frameLen);

  switch(inFrame.getType()) {
  case FRAME_IP_TYPE:
  case FRAME_ARP_TYPE:
    return SystemManagement::handleLeyer2Frame(inFrame);
  }

  return -1;
}

// **********************************************************************
// **********************************************************************
// **************************   IN FRAME      ***************************
// **********************************************************************
// **********************************************************************

// **************************************************************
// Constructor
// **************************************************************
InFrame::InFrame(const char* messageBase, unsigned int frameLen) :
  mBase(messageBase),
  mLen(frameLen),
  mOffset(14),
  mSrcMAC(messageBase+6),
  mFrameType(FRAME_INVALID_TYPE)
{
  if ((messageBase != NULL) && (frameLen >= 14)) {
    uint16_t typeField = *(uint16_t*)(messageBase+12);
    if (typeField == 0x0608) {
      mFrameType = FRAME_ARP_TYPE;
    }
    else if (typeField == 0x0008) {
      mFrameType = FRAME_IP_TYPE;
    }
  }
}

// **************************************************************
// Checks whether the target MAC is a broadcast - all 0xFF
// **************************************************************
bool InFrame::isBroadcast() const
{
  if ((*(uint32_t *)(mBase) == 0xFFFFFFFF) &&
      (*(uint16_t *)(mBase) == 0xFFFF)) {
    return true;
  }
  return false;
}

// **************************************************************
// Buffer method
// **************************************************************
uint16_t InFrame::readNextNetworkU16()
{
  uint16_t value = readNextHostU16();
  return ntohs(value);
}

// **************************************************************
// Buffer method
// **************************************************************
uint32_t InFrame::readNextNetworkU32()
{
  uint16_t value = readNextHostU16();
  return ntohl(((uint32_t)value));
}
  
// **************************************************************
// Reader
// **************************************************************
uint16_t InFrame::readNextHostU16()
{
  if (mOffset + 2 > mLen) {
    mOffset = mLen;
    return 0xFFFF;
  }

  uint16_t retVal = *(uint16_t *)(mBase+mOffset);
  mOffset += 2;
  return retVal;
}

// **************************************************************
// Reader
// **************************************************************  
uint32_t InFrame::readNextHostU32()
{
  if (mOffset + 4 > mLen) {
    mOffset = mLen;
    return 0xFFFFFFFF;
  }

  uint32_t retVal = *(uint32_t *)(mBase + mOffset);
  mOffset += 4;
  return retVal;
}

// **************************************************************
// Reader
// **************************************************************  
const char* InFrame::readNextMAC()
{
  if (mOffset + 6 > mLen) {
    mOffset = mLen;
    return NULL;
  }

  const char* retVal = mBase + mOffset;
  mOffset += 6;
  return retVal;
}

// **********************************************************************
// **********************************************************************
// **************************  OUT FRAME      ***************************
// **********************************************************************
// **********************************************************************

uint32_t OutFrame::mOutFrameCounter = 1;

// **************************************************************
// static accessor
// **************************************************************
uint32_t OutFrame::getAndIncFrameCtr()
{
  uint32_t retVal = mOutFrameCounter+1;
  if (retVal == 0xFFFFFFFF) {
    retVal = 1;
  }
  mOutFrameCounter = retVal;
  return retVal;
}

// **************************************************************
// Constructor
// **************************************************************
OutFrame::OutFrame(FrameType mFrameType, const MACAddr& target) :
  mBase(NULL),
  mOffset(0),
  mThisFrame(0)
{
  mBase = getFreeOutPacket();
  if (mBase == NULL) {
    shortDelay();
    mBase = getFreeOutPacket();
    if (mBase == NULL) {
      // need to protect against a busy system.
      return;
    }
  }

  // determine frame type
  uint16_t typeField;
  switch (mFrameType) {
  case FRAME_ARP_TYPE:
    typeField = 0x0608;
    break;
  case FRAME_IP_TYPE:
    typeField = 0x0008;
    break;
  default:
    return;
  }

  // register outbound message
  mThisFrame = getAndIncFrameCtr();

  // enter physical data
  writeMAC(target);
  writeMAC(SystemManagement::getPersistentDataCenter().getMacAddr());
  writeHostU16(typeField);
}

// **************************************************************
// Constructor
// **************************************************************
OutFrame::OutFrame(uint32_t targetIP, CommunicationCenter& communicationCenter) :
  mBase(NULL),
  mOffset(0),
  mThisFrame(0)
{
  // Verify target IP. Will be '0' if target no in subnet, and no default gateway.
  if (targetIP == 0) {
    return;
  }

  const MACAddr& targetMAC = communicationCenter.getMacAddr(targetIP);
  if (!targetMAC.isValid()) {
    return;
  }

  mBase = getFreeOutPacket();
  if (mBase == NULL) {
    shortDelay();
    mBase = getFreeOutPacket();
    if (mBase == NULL) {
      // need to protect against a busy system.
      return;
    }
  }

  // register outbound message
  mThisFrame = getAndIncFrameCtr();
  
  writeMAC(targetMAC);
  writeMAC(SystemManagement::getPersistentDataCenter().getMacAddr());
  writeHostU16(0x0008);
}

// **************************************************************
// Buffer method
// **************************************************************
int OutFrame::writeNetworkU16(uint16_t val)
{
  return writeHostU16(htons(val));
}

// **************************************************************
// Buffer method
// **************************************************************
int OutFrame::writeNetworkU32(uint32_t val)
{
  return writeHostU32(htonl(val));
}
  
// **************************************************************
// Writer
// **************************************************************
int OutFrame::writeHostU16(uint16_t val)
{
  // verify valid frame
  if (isBad()) {
    return -2;
  }

  // verify enough space left
  if (getFreeSpace() < 2) {
    mThisFrame = 0;              // invalidate frame
    return -1;
  }

  *(uint16_t*)(mBase + mOffset) = val;
  mOffset += 2;

  return 0;
}

// **************************************************************
// Writer
// **************************************************************
int OutFrame::writeHostU32(uint32_t val)
{
  // verify valid frame
  if (isBad()) {
    return -2;
  }

  // verify enough space left
  if (getFreeSpace() < 4) {
    mThisFrame = 0;              // invalidate frame
    return -1;
  }

  *(uint32_t*)(mBase + mOffset) = val;
  mOffset += 4;

  return 0;
}

// **************************************************************
// Writer
// **************************************************************
int OutFrame::writeMAC(const MACAddr& mac)
{
  // verify valid frame
  if (isBad()) {
    return -2;
  }

  // verify enough space left
  if (getFreeSpace() < 6) {
    mThisFrame = 0;              // invalidate frame
    return -1;
  }

  if (mac.isValid()) {
    memcpy (mBase + mOffset, mac.getAddr(), 6);
  }
  else {
    // invalid MAC turns into a broadcast braodcast
    memset(mBase + mOffset, 0xFF, 6);
  }
  mOffset += 6;

  return 0;
}

// **************************************************************
// Writer
// **************************************************************
int OutFrame::appendData(const char* blockBase, unsigned int blockSize)
{
  // verify valid frame
  if (isBad()) {
    return -2;
  }

  // verify enough space left
  if (getFreeSpace() < blockSize) {
    mThisFrame = 0;              // invalidate frame
    return -1;
  }

  memcpy (mBase +  mOffset, blockBase, blockSize);
  mOffset += blockSize;

  return blockSize;
}

// **************************************************************
// Writer
// Returns the number of bytes written. '0' on error.
// **************************************************************
int OutFrame::appendData(const Buffer& src, unsigned int blockSize)
{
  return appendData(src, 0, blockSize);
}

// **************************************************************
// Writer
// Returns the number of bytes written. '0' on error.
// **************************************************************
int OutFrame::appendData(const Buffer& src,
                         unsigned int bufferOffset,
                         unsigned int blockSize)
{
  // verify valid frame
  if (isBad()) {
    return 0;
  }

  // verify enough space left
  if (getFreeSpace() < blockSize) {
    setBad();
    return 0;
  }

  int readLen = src.readBlock(mBase +  mOffset, bufferOffset, blockSize);
  if ((readLen < 0) || (readLen > blockSize)) {
    setBad();
    return 0;
  }

  mOffset += readLen;
  return readLen;
}

// **************************************************************
// removes data from frame
// **************************************************************
void OutFrame::removeData(unsigned int dataSize)
{
  // verify valid frame
  if (isBad()) {
    return;
  }

  if (mOffset < dataSize) {
    setBad();
    return;
  }
  mOffset -= dataSize;
}

// **************************************************************
// Writer
// **************************************************************
int OutFrame::sendMsg()
{
  // verify valid frame
  if (isBad()) {
    return -2;
  }

  if (mThisFrame != mOutFrameCounter) {
    // unexpected error, but none the less.
    mThisFrame = 0;
    return -3;
  }

#ifdef DEBUG
  // error injection. will loose 1 package of every 'INCOMING_PACKAGE_DROP'
  uint16_t randomNum = get16RandomNumber();
  if ((TX_PACKAGE_DROP != 0) && ((randomNum % TX_PACKAGE_DROP) == 0)) {
    setBad();
    return mOffset;
  }
#endif // DEBUG

  bool bSendFailed = (sendCurOutPacket(mBase, mOffset) != 0);
  // do not allow a resend.
  setBad();

  if (bSendFailed) {
    return -4;
  }

  return mOffset;
}
