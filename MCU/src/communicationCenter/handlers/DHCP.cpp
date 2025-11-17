// **********************************************************************
// **********************************************************************
// Copyright 2011 by Avi Owshanko. All rights reserved.
// **********************************************************************
// **********************************************************************

#include "DHCP.h"

//#include "layer3Connection.h"
#include "globalInterface.h"
#include "stm32Interface.h"
#include "persistentDataCenter.h"
#include "systemManagement.h"
#include "../Hardware/HardwareCenter.h"
//#include "handlersCenter.h"
#include "genericClasses.h"

// **********************************************************************
// **********************************************************************
// ************************   Telnet Server     *************************
// **********************************************************************
// **********************************************************************

// **************************************************************
// constructor
// **************************************************************
DHCPClient::DHCPClient( IPData& IP, Layer3& udpLayer) :
  mBuffers(UDP_PORT_DHCP),
  mUDPConnection(mBuffers, udpLayer),
  mLease_Time(0),
  mTimeFlag(0),
  mIPData(IP),
  mState(DHCP_State_INIT)
    
  { 
  }
  
  // **************************************************************
  // initialization call.
  // notifies that persistent settings are loaded.
  // **************************************************************
  void DHCPClient::openSocket()
  {
    mUDPConnection.openObject(DHCP_CLIENT_PORT, DHCP_SERVER_PORT, 0xFFFFFFFF );
  }
  
  // **************************************************************
  // Periodic maintenance of the telnet connection if open.
  // **************************************************************
    
  void DHCPClient::periodicCall()
  {
    if (isClosed()) return;
    
    if (getTimerTicks() < 0x0000FFFF)
      mTimeFlag = 0;
    readBuffer();
    
    switch (mState){
    case DHCP_State_INIT:
      if (getTimerTicks() % DHCP_TRY_INTERVAL < (1000)){
        mXID = SystemManagement::getPersistentDataCenter().getNext32RandomNumber();
        sendMessage(DHCPDISCOVER);
        mState = DHCP_State_SELECTING;
      }
      break;
    case DHCP_State_SELECTING:
    case DHCP_State_REQUESTING:
      if (getTimerTicks() % DHCP_TRY_INTERVAL > (DHCP_TRY_INTERVAL-1000)){
        mState = DHCP_State_INIT;
        mIPData.mAddr = 0; 
      }
      break;
    case DHCP_State_BOND:
      if ((getTimerTicks()  > mRenew_Time) && mTimeFlag == 0){
        mState = DHCP_State_RENEWING;
        if ( getTimerTicks() > mRebind_Time)
          mTimeFlag = 1;
        sendMessage(DHCPREQUEST, mIPData.mAddr, mLastServer); 
      }
      break;
    case DHCP_State_RENEWING:
      if ((getTimerTicks()  > mRebind_Time) && mTimeFlag == 0){
        mState = DHCP_State_REBINDING;
        if ( getTimerTicks() > mLease_Time)
          mTimeFlag = 1;
        sendMessage(DHCPREQUEST, mIPData.mAddr); 
      }
      break;
    case DHCP_State_REBINDING:
      if ((getTimerTicks()  > mLease_Time) && mTimeFlag == 0){
        mIPData.mAddr = 0; 
        mState = DHCP_State_INIT; 
      }
      break;
    default:
      break;
    }
    
    
    // refresh outbound stream
    mBuffers.advanceReadTop();
  }
  
  // **************************************************************
  // Tries to read from the inbound TCP stream
  // **************************************************************
  void DHCPClient::readBuffer()
  {
    // refresh incoming stream
    mBuffers.advanceReadTop();
    
    unsigned int inSize = mBuffers.getInBuffer().getDataSize();
    if (inSize == 0) {
      return;
    }
    
    // We do not check if out buffer has enough space left - 
    
    // Read data until we reached the end of stream,
    //Check end with 0xFF
    IPData NewIP;
    uint32_t lease = 1;
    uint32_t serverAddress = 0;
    switch (mState){
    case DHCP_State_INIT:
      return;
    case DHCP_State_SELECTING:
      switch (recvMessage(NewIP, &serverAddress, NULL )){
      case DHCPOFFER: //Send DHCP Request message
        mIPData.mMask = NewIP.mMask;
        sendMessage(DHCPREQUEST, NewIP.mAddr, serverAddress);
        mState = DHCP_State_REQUESTING;
        break;
      default:
        break;
      }
      break;
    case DHCP_State_REQUESTING:
    case DHCP_State_REBINDING:
    case DHCP_State_RENEWING:
      switch (recvMessage(NewIP, &serverAddress, &lease )){
      case DHCPNAK:
        mIPData.mAddr = 0;
        mState = DHCP_State_INIT;      
        break;
      case DHCPACK:
        mIPData = NewIP;
        mLastServer = serverAddress;
        mRenew_Time = lease *500 + getTimerTicks();
        mRebind_Time = lease * 875 + getTimerTicks();
        mLease_Time = lease * 1000 + getTimerTicks();
        if ( mRenew_Time < getTimerTicks())
          mTimeFlag = 1;
        else
          mTimeFlag =0;
        mState = DHCP_State_BOND;
        break;
      default:
        break;
      }
      break;
    default:
      break;
    }
    while (mBuffers.getInBuffer().readChar() >= 0);
    
  }
  
  
  // **************************************************************
  // marks the telnet server as closing.
  // **************************************************************
  void DHCPClient::closeSocket()
  {
    mUDPConnection.closeObject();
  }
  
  // **************************************************************
  // returns an indication whether server is closed.
  // server is closed only when connection is closed as well.
  // **************************************************************
  bool DHCPClient::isClosed() const
  {
    if (!mUDPConnection.isClosed()) {
      return false;
    }
    
    return true;
  }
  
  // **************************************************************
  // Sends the discovery request to the web.
  // **************************************************************
  /*void DHCPClient::dhcpDiscover(){
  mXID = SystemManagement::getPersistentDataCenter().getNext32RandomNumber();
  sendMessage(DHCPDISCOVER);
  mLastMessage = DHCP_DISCOVER;
}
  
  void DHCPClient::dhcpRequest()
  {
  IPData NewIP;
  uint32_t serverAddress;
  //Check if Message is correct
  switch (recvMessage(NewIP, &serverAddress, NULL )){
  case DHCPOFFER:
  break;
  default:
  return;
  }
  
  //Send DHCP Request message
  sendMessage(DHCPREQUEST, NewIP.mAddr, serverAddress);
  mLastMessage = DHCP_REQUEST;
}
  void DHCPClient::ReadAckData()
  {
  uint32_t serverAddress;
  uint32_t lease = 1;
  IPData NewIP;
  //Check if Message is correct
  switch (recvMessage(NewIP, &serverAddress, &lease )){
  case DHCPERROR:
  return;
  case DHCPNAK:
  mLastMessage = DHCP_UNREGISTER;
  return;
  case DHCPACK:
  mIPData = NewIP;
  mLease_Time = lease *1000 + getTimerTicks();
  if ( mLease_Time < getTimerTicks())
  mTimeFlag = 1;
   else
  mTimeFlag =0;
  mLastMessage = DHCP_ACK;
  return;
  }
  
  }
  */ 
  void DHCPClient::sendMessage(DHCP_Message_Type messageType, uint32_t requestIP, uint32_t serverIP){
    mBuffers.getOutBuffer().writeBlock("\x01\x01\x06\x00",4);
    mBuffers.getOutBuffer().writeBlock(&mXID,4);
    mBuffers.getOutBuffer().writeBlock("\x00\x00\x80\x00",4); //SECS, FLAGS
    //mBuffers.getOutBuffer().writeBlock(&requestIP,4); //CIADDR
    mBuffers.getOutBuffer().writeNChar(0, 16);  //CIADDR, YIADDR , SIADDR , GIADDR
    MACAddr macaddr = SystemManagement::getPersistentDataCenter().getMacAddr();
    mBuffers.getOutBuffer().writeBlock(macaddr.getAddr(),6); //CHADDR
    mBuffers.getOutBuffer().writeNChar(0, 202);  //all the rest that is not used
    mBuffers.getOutBuffer().writeBlock("\x63\x82\x53\x63",4); //Magic cookie
    mBuffers.getOutBuffer().writeBlock("\x35\x01",2); //Discover Option
    mBuffers.getOutBuffer().writeChar(messageType);
    if (requestIP != 0){
      mBuffers.getOutBuffer().writeBlock("\x32\x04",2); // My IP that I request
      mBuffers.getOutBuffer().writeBlock(&requestIP, 4);
    }
    if (serverIP != 0){
      mBuffers.getOutBuffer().writeBlock("\x36\x04",2); // Server IP
      mBuffers.getOutBuffer().writeBlock(&serverIP , 4);
    }
    mBuffers.getOutBuffer().writeBlock("\xFF",1);
  }
  
  DHCP_Message_Type  DHCPClient::recvMessage(IPData& offerIP, uint32_t *serverIP, uint32_t *Lease ){
    uint32_t octet;
    uint32_t MyIP;
    uint32_t serverAddress;
    uint32_t getway = 0x0000A8C0;
    uint32_t subnetMask = 0x00FFFFFF;
    DHCP_Message_Type rc;
    //Check if Message is correct
    mBuffers.getInBuffer().readBlock((char *)&octet, 4); //options
    mBuffers.getInBuffer().advanceRead(4);
    if (octet != 0x00060102) return DHCPERROR;
    mBuffers.getInBuffer().readBlock((char *)&octet, 4); // XID
    mBuffers.getInBuffer().advanceRead(4);
    if (octet != mXID) return DHCPERROR; 
    mBuffers.getInBuffer().advanceRead(8);
    mBuffers.getInBuffer().readBlock((char *)&MyIP, 4);  //YIADDR
    mBuffers.getInBuffer().advanceRead(12);
    {
      char macComp[8];
      mBuffers.getInBuffer().readBlock(macComp, 8);
      macComp[7] = 0x01;
      if (memcmp(macComp,SystemManagement::getPersistentDataCenter().getMacAddr().getAddr(),8) != 0) return DHCPERROR;
      mBuffers.getInBuffer().advanceRead(208);
    }
    mBuffers.getInBuffer().readBlock((char *)&octet, 4);
    mBuffers.getInBuffer().advanceRead(4);
    if (octet != 0x63538263) return DHCPERROR;
    uint8_t c;
    while ( (c = mBuffers.getInBuffer().readChar()) != 0xFF && mBuffers.getInBuffer().getDataSize() > 0){
      octet = 0;
      switch (c){
      case 0x35:
        c = mBuffers.getInBuffer().readChar();
        if (c != 1) return DHCPERROR;
        rc = (DHCP_Message_Type)mBuffers.getInBuffer().readChar();
        break;
      case 0x36:
        c = mBuffers.getInBuffer().readChar();
        if (c == 4)
          mBuffers.getInBuffer().readBlock((char *)&serverAddress, 4);
        mBuffers.getInBuffer().advanceRead(c);
        break;
      case 0x33:
        c = mBuffers.getInBuffer().readChar();
        if (c == 4 && Lease != NULL){
          *Lease = mBuffers.getInBuffer().readChar() << 24;
          *Lease += mBuffers.getInBuffer().readChar() << 16;
          *Lease += mBuffers.getInBuffer().readChar() << 8;
          *Lease += mBuffers.getInBuffer().readChar();
        }
        else
          mBuffers.getInBuffer().advanceRead(c);
        break;
      case 0x01:
        c = mBuffers.getInBuffer().readChar();
        if (c == 4)
          mBuffers.getInBuffer().readBlock((char *)&subnetMask, 4);
        mBuffers.getInBuffer().advanceRead(c);
        break;  
      case 0x03:
        c = mBuffers.getInBuffer().readChar();
        if (c == 4)
          mBuffers.getInBuffer().readBlock((char *)&getway, 4);
        mBuffers.getInBuffer().advanceRead(c);
        break;
      default:
        c = mBuffers.getInBuffer().readChar();
        mBuffers.getInBuffer().advanceRead(c);
        break;
      }
    }
    if (serverIP != NULL)
      *serverIP = serverAddress;
    offerIP.setData(MyIP, subnetMask, getway);

    return rc;
  }