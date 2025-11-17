// **********************************************************************
// **********************************************************************
// Copyright 2011 by Avi Owshanko. All rights reserved.
// **********************************************************************
// **********************************************************************

#include "neighbConnection.h"
#include "../ethernet/udpMessage.h"
#include "../ethernet/layer3.h"
#include "../communicationCenter.h"
#include "handlersCommon.h"

// **********************************************************************
// **********************************************************************
// ************************   UDP Connection    *************************
// **********************************************************************
// **********************************************************************

// **************************************************************
// constructor
// **************************************************************
neighbConnection::neighbConnection( Layer3& udpLayer) : mUDPLayer(udpLayer), Next(0)
{
  for (int i =0; i<64 ; i++)
    FilterMACs[i] = 0;
}

// **************************************************************
// Handles an incoming udp message
// Tests for message integrity, and passes on to buffer
// **************************************************************
int neighbConnection::handleIncomingMsg(uint32_t srcIP, uint32_t dstIP,
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

  if (udpHeader.mTargetSocket != CONTROL_KEPPALIVE_PORT) {
    // This is an internal error - do not send message on internal errors.
    return -4;
  }

  const uint8_t* Msg = (const uint8_t*)(msgBase + UDP_HEADER_LEN);
  if (Msg[0] != CONTROL_OPCODE_KEEPALIVE) 
	  return -7;
  insertIP(srcIP);
  return 0;
}

// **************************************************************
// Periodic call for testing connection.
// Checks whether we need to send some reply to other side if open.
// **************************************************************
void neighbConnection::periodicCall()
{
   return;
}

// **************************************************************
// Tests whether connection is closed
// **************************************************************
bool neighbConnection::isClosed() const
{
  return !mUDPLayer.hasConnection(CONTROL_KEPPALIVE_PORT);
}

// **************************************************************
// Marks object as closed
// **************************************************************
void neighbConnection::closeObject()
{

  if (!isClosed()) {
    mUDPLayer.removeListener(CONTROL_KEPPALIVE_PORT);
  }
}

// **************************************************************
// Marks object as opened
// **************************************************************
void neighbConnection::openObject()
{
 
  if (mUDPLayer.hasConnection(CONTROL_KEPPALIVE_PORT)) {
    return;
  }
  Next = 0;
  for (int i =0; i<64 ; i++)
    FilterMACs[i] = 0;
  mUDPLayer.addListener(CONTROL_KEPPALIVE_PORT, this);
}


//****************************************************************
// Check if MAC's filter is new and add if it is new
//****************************************************************
void neighbConnection::insertIP(uint32_t IP)
{
  uint64_t MAC = (*(const uint64_t *)(mUDPLayer.getCommunicationCenter().getMacAddr(IP).getAddr())) & 0x0000FFFFFFFFFFFF;
  if (MAC == 0) return;
  for ( int i = 0; i < 64; i++)
    if ( MAC == FilterMACs[i])
      return;
  FilterMACs[Next] = MAC;
  Next = (Next + 1) % 64;
  return;
}

