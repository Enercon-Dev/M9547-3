// **********************************************************************
// **********************************************************************
// Copyright 2011 by Avi Owshanko. All rights reserved.
// **********************************************************************
// **********************************************************************

#ifndef CONTROL_H
#define CONTROL_H

#include <stdint.h>
#include "communicationCenter/ethernet/udpConnection.h"
#include "communicationCenter/ethernet/mac.h"
#include "communicationCenter/buffers/buffers.h"
#include "handlersCommon.h"

#include <string.h>

#define KEEPALIVE_INTERVAL      (1000)
#define IP_MAC_INTERVAL         (1000)

//pre declarations
class CommunicationCenter;

// *******************************************************************
// A class for Control of FCU
// *******************************************************************
class Controller
{
public: 
  Controller(Layer3&, CommunicationCenter&);

  // upper layers requests
  void periodicCall();
  void closeSocket();
  void openSocket();
  bool isClosed() const;
        
  void SendStatus(CONTROL_OPCODE op, uint8_t broadcast);

private:

  uint8_t readBuffer();
  void sendAckNack(CONTROL_OPCODE OpCode, COMMAND_SUCCESS result);
  COMMAND_SUCCESS handleSetDebug();
  COMMAND_SUCCESS handleSetPor();
  COMMAND_SUCCESS handleSetDci();
  void handleGetPDUInfo( Buffer& DataOut );
  COMMAND_SUCCESS handleCalibrate();
  COMMAND_SUCCESS handleErase();
  void SendIpMac();
  COMMAND_SUCCESS handleSetIP();
 
// internal data
  BidirectionalBuffer mBuffers;
  UDPConnection mUDPConnection;
  CommunicationCenter& mCommunicationCenter;
  uint8_t isReset;
  
};



#endif // CONTROL_H