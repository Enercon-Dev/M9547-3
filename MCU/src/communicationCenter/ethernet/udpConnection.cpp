// **********************************************************************
// **********************************************************************
// Copyright 2011 by Avi Owshanko. All rights reserved.
// **********************************************************************
// **********************************************************************

#include "udpConnection.h"
#include "udpMessage.h"
#include "../buffers/buffers.h"
#include "layer3.h"
#include "systemManagement.h"

// **********************************************************************
// **********************************************************************
// ************************   UDP Connection    *************************
// **********************************************************************
// **********************************************************************

// **************************************************************
// constructor
// **************************************************************
UDPConnection::UDPConnection(BidirectionalBuffer& buffer,
                             Layer3& udpLayer) :
  mMySocket(0),
  mUDPLayer(udpLayer),
  mBuffers(buffer),
  mObjectOpen(false)
{
}

// **************************************************************
// Handles an incoming udp message
// Tests for message integrity, and passes on to buffer
// **************************************************************
int UDPConnection::handleIncomingMsg(uint32_t srcIP, uint32_t dstIP,
                                     const unsigned char* msgBase,
                                     unsigned int msgLen)
{
  if (isClosed()) {
    return -3;
  }

  if (srcIP == 0) {
    return -1;
  }

  if (msgLen < 8) {
    return -6;
  }

  UDPHeaderData udpHeader(msgBase, srcIP, dstIP, msgLen);
  if (udpHeader.mTargetSocket == 0) {
    // illegal header
    return -2;
  }

  if (udpHeader.mTargetSocket != mMySocket) {
    // This is an internal error - do not send message on internal errors.
    return -4;
  }

  // verify that we have enough space in buffer
  unsigned int freeSpace = mBuffers.getInBuffer().getFreeSize();
  if (udpHeader.mMsgLen > freeSpace) {
    return -5;
  }

  LastRecievedSocket = udpHeader.mSourceSocket;
  LastRecievedIP = srcIP;
  const char* dataBase = (const char*)(msgBase + UDP_HEADER_LEN);
  return mBuffers.getInBuffer().writeBlock(dataBase,
                                           udpHeader.mMsgLen-UDP_HEADER_LEN);
}

// **************************************************************
// Periodic call for testing connection.
// Checks whether we need to send some reply to other side if open.
// **************************************************************
void UDPConnection::periodicCall()
{
  if (isClosed()) {
    return;
  }

  unsigned int dataLen = mBuffers.getOutBuffer().getDataSize();
  if (dataLen > 0) {
    sendUDPMessage();
  }
}

// **************************************************************
// Tests whether connection is closed
// **************************************************************
bool UDPConnection::isClosed() const
{
  return !mObjectOpen;
}

// **************************************************************
// Marks object as closed
// **************************************************************
void UDPConnection::closeObject()
{
  mObjectOpen = false;

  if (mMySocket != 0) {
    mUDPLayer.removeListener(mMySocket);
    mMySocket = 0;
  }
}

// **************************************************************
// Marks object as opened
// **************************************************************
void UDPConnection::openObject(uint16_t mySocket,
                               uint16_t targetSocket,
                               uint32_t targetIP)
{
  if ((mObjectOpen) || (mySocket == 0)) {
    return;
  }

// allow invalid targets - that way we handle only incoming messages.
  if (targetSocket == 0) {
    targetIP = 0;
  }

  if (mUDPLayer.hasConnection(mySocket)) {
    return;
  }

  mMySocket = mySocket;
  mTargetSocket = targetSocket;
  mTargetIP = targetIP;
  mUDPLayer.addListener(mySocket, this);
  mObjectOpen = true;
}


// **************************************************************
// Sends buffered data to target computer
// **************************************************************
void UDPConnection::sendUDPMessage()
{
  unsigned int dataLen = mBuffers.getOutBuffer().getDataSize();
  if (dataLen > MAX_LAYER3_MESSAGE) {
    dataLen = MAX_LAYER3_MESSAGE;
  }

  if (dataLen == 0) {
    // just in case
    return;
  }

  // if no valid target, just flush buffer
  if (mTargetIP == 0) {
    mBuffers.getOutBuffer().advanceRead(dataLen);
  }

  // prepares message
  UDPHeaderData headerData(mMySocket,                                  // source socket
                           mTargetSocket);                             // target socket
  OutUDPMsg answer(mTargetIP, headerData, mUDPLayer.getCommunicationCenter());
  answer.appendData(mBuffers.getOutBuffer(), dataLen);

  // sends message and updates the output buffer that message was sent
  if (answer.sendMsg() > 0) {
    mBuffers.getOutBuffer().advanceRead(dataLen);
  }
  else if (!SystemManagement::oneshotsPending()) {
    // leave system some time to warm up before starting to discard data.
    mBuffers.getOutBuffer().advanceRead(dataLen);
  }
}
