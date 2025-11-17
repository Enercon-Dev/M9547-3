// **********************************************************************
// **********************************************************************
// Copyright 2011 by Avi Owshanko. All rights reserved.
// **********************************************************************
// **********************************************************************

#ifndef DHCP_H
#define DHCP_H

#include <stdint.h>
#include "../ethernet/udpConnection.h"
#include "../ethernet/mac.h"
#include "../buffers/buffers.h"
#include <string.h>

#define DHCP_TRY_INTERVAL (5000)
#define DHCP_CLIENT_PORT (0x4400)
#define DHCP_SERVER_PORT (0x4300)


typedef enum {DHCP_State_INIT, DHCP_State_SELECTING, DHCP_State_REQUESTING, DHCP_State_BOND, DHCP_State_RENEWING, DHCP_State_REBINDING} DHCP_State;
typedef enum {DHCPDISCOVER = 1, DHCPOFFER = 2, DHCPREQUEST = 3, DHCPACK = 5, DHCPNAK = 6 ,DHCPERROR = 13} DHCP_Message_Type;

// *******************************************************************
// A class for managing DHCP connection through port 67,68
// *******************************************************************
struct DHCPClient
{
public: 
  DHCPClient(IPData& IP, Layer3&);

  // upper layers requests
  void periodicCall();
  void closeSocket();
  void openSocket();
  bool isClosed() const;
  
private:

  void readBuffer();
  void sendMessage(DHCP_Message_Type messageType, uint32_t requestIP = 0, uint32_t serverIP = 0);
  DHCP_Message_Type recvMessage(IPData& offerIP, uint32_t *serverIP, uint32_t *Lease = NULL );

// internal data
  uint32_t mRenew_Time;
  uint32_t mRebind_Time;
  uint32_t mLease_Time;
  uint8_t  mTimeFlag;
  uint32_t mLastServer;
  IPData&  mIPData;
  BidirectionalBuffer mBuffers;
  UDPConnection mUDPConnection;
  
  DHCP_State mState;
  uint32_t mXID;
};



#endif // DHCP_H