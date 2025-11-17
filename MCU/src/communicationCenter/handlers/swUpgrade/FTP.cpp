// **********************************************************************
// **********************************************************************
// Copyright 2011 by Avi Owshanko. All rights reserved.
// **********************************************************************
// **********************************************************************

#include "FTP.h"

#include "globalInterface.h"
#include "stm32Interface.h"
//#include "persistentDataCenter.h"
#include "persistentCommon.h"
#include "systemManagement.h"
//#include "handlersCenter.h"
#include "genericClasses.h"
#include "stm32f10x_flash.h"

void FTP_OnReceiveEndDefault(uint32_t len){return;}
int FTP_OnReceiveDefault(Buffer& buf, uint16_t block){return -1;}
// **********************************************************************
// **********************************************************************
// ************************   Telnet Server     *************************
// **********************************************************************
// **********************************************************************

// **************************************************************
// constructor
// **************************************************************
FTPClient::FTPClient( uint16_t mySocket, TCPPortTypes portType, Layer3& udpLayer,void (*ReceiveEndHandler)(uint32_t), int (*ReceiveHandler)(Buffer&,uint16_t)) :
  mBuffers(portType),
  mSocket(mySocket),
  mUDPConnection(mBuffers, udpLayer),
  mState(FTP_State_IDLE),
  mLastError(FTP_ERRCODE_NO_ERROR),
  mTimeout(0),
  OnReceiveEnd(ReceiveEndHandler),
  OnReceive(ReceiveHandler),
  mBytesReceived(0),
  mRetryCounter(0)
    
  { 
  }
  
  // **************************************************************
  // initialization call.
  // notifies that persistent settings are loaded.
  // **************************************************************
  void FTPClient::openSocket()
  {
    if (mServerIP == 0)
      return;
    mUDPConnection.openObject(mSocket, FTP_SERVER_PORT, mServerIP );
  }
  
  // **************************************************************
  // Send RRQ to Server on Port 69
  // LOL
  // **************************************************************
  int FTPClient::RequestFile( uint32_t ServerIP, const char* FileName){
    if (mState == FTP_State_END) return -2;
    mState = FTP_State_RECV;
    mServerIP = ServerIP;
    openSocket();
    strncpy(mFileName,FileName,MAX_FILENAME_LENGTH);
    mFileName[MAX_FILENAME_LENGTH] = '\0';
    mLastBlockReceived = 0;
    mTimeout = (getTimerTicks()+10000) % 11000;
    mBytesReceived = 0;
    mLastError = FTP_ERRCODE_NO_ERROR;
    mUDPConnection.changeTargetSocket(FTP_SERVER_PORT);
    
    while (mBuffers.getInBuffer().readChar() >= 0);
    int rc;
    uint16_t Op = htons((uint16_t)FTP_OPCODE_RRQ);
    rc = (mBuffers.getOutBuffer().writeBlock(&Op, 2) >= 0);
    rc += (mBuffers.getOutBuffer().writeBlock(mFileName, strlen(mFileName)+1) >= 0);
    rc += (mBuffers.getOutBuffer().writeBlock("octet", 6) >= 0);
    
    mBuffers.advanceReadTop();
    mRetryCounter = 0;
    return rc;
  }
  
  // **************************************************************
  // Retransmit when Timeout and other small errors
  // **************************************************************
  int FTPClient::Retransmit(){
    if (isClosed()) return -1;
    if (mState == FTP_State_END) return -2;
    
    mTimeout = (getTimerTicks()+10000) % 11000;
    
    int rc;
    uint16_t Op;
    if (mLastBlockReceived == 0)
    {
      Op = htons((uint16_t)FTP_OPCODE_RRQ);
      mUDPConnection.changeTargetSocket(FTP_SERVER_PORT);
      rc = (mBuffers.getOutBuffer().writeBlock(&Op, 2) >= 0);
      rc += (mBuffers.getOutBuffer().writeBlock(mFileName, strlen(mFileName)+1) >= 0);
      rc += (mBuffers.getOutBuffer().writeBlock("octet", 6) >= 0);
    }
    else
    {
      Op = htons((uint16_t)FTP_OPCODE_ACK);
      rc = (mBuffers.getOutBuffer().writeBlock(&Op, 2) >= 0);
      uint16_t h_octet = htons(mLastBlockReceived);
      rc += (mBuffers.getOutBuffer().writeBlock(&h_octet, 2) >= 0);
    }
    mBuffers.advanceReadTop();
    mRetryCounter++;
    return (rc > 0);
  }
  
  // **************************************************************
  // Send Error Message to Server
  // **************************************************************
  int FTPClient::SendError(FTP_ERRCODE ErrCode){
    if (isClosed()) return -1;
    if (mState == FTP_State_END) return -2;
    
    mTimeout = (getTimerTicks()+10000) % 11000;
    
    int rc;
    uint16_t Op = htons((uint16_t)FTP_OPCODE_ERROR);
    uint16_t ErrCodeByte = htons(ErrCode);
    rc = (mBuffers.getOutBuffer().writeBlock(&Op, 2) >= 0);
    rc += (mBuffers.getOutBuffer().writeBlock(&ErrCodeByte, 2) >= 0);
    mBuffers.getOutBuffer().writeChar('\0');
    
    mBuffers.advanceReadTop();
    return (rc > 0);
  }
  
  // **************************************************************
  // Save Incomming Block To Memory.
  // **************************************************************
  int FTPClient::SaveBlockToMem(uint16_t BlockNum){
    int rc = 0;
    //if (BlockNum > (MAX_VERSION_SIZE/FTP_BLOCK_SIZE)) return -4; //TODO: Add Size limitation
    if (BlockNum ==0 ||BlockNum != (uint16_t)(mLastBlockReceived +1)){
      Retransmit();
      return -1;
    }

    rc = OnReceive(mBuffers.getInBuffer(), BlockNum);
    if (rc < 0) return rc;
    while (mBuffers.getInBuffer().readChar() >= 0);
    mLastBlockReceived = BlockNum;
    uint16_t h_octet = htons(mLastBlockReceived);
    uint16_t Op = htons((uint16_t)FTP_OPCODE_ACK);
    mBytesReceived += rc;
    rc += (mBuffers.getOutBuffer().writeBlock(&Op, 2) >0);
    rc += (mBuffers.getOutBuffer().writeBlock(&h_octet, 2) >0 );
    mBuffers.advanceReadTop();
    mRetryCounter = 0;
    return rc;
    
  }
  
  // **************************************************************
  // Periodic maintenance of the telnet connection if open.
  // **************************************************************
  void FTPClient::periodicCall()
  {
    
    if (isClosed()) return;
    
    readBuffer();
    switch(mState){
    case FTP_State_IDLE:
      if (!isClosed() && mBuffers.getOutBuffer().getDataSize() == 0)
                closeSocket();
      break;
    case FTP_State_END:
      if (mBuffers.getOutBuffer().getDataSize() == 0) {
        OnReceiveEnd(mBytesReceived);
        mState = FTP_State_IDLE;
      }
      break;
    case FTP_State_RECV:
      if ((mTimeout < getTimerTicks() % 11000) && (mTimeout+1000 > getTimerTicks() % 11000)){
        Retransmit();
        mTimeout = (getTimerTicks()+10000) % 11000;
      }
      if (  mRetryCounter > NUM_OF_RETRIES )
        mState = FTP_State_IDLE;
      break;

    }
    
    // refresh outbound stream
    mBuffers.advanceReadTop();
  }
  
  
  
  // **************************************************************
  // Tries to read from the inbound TCP stream
  // **************************************************************
  void FTPClient::readBuffer()
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
    char octet[4];
    FTP_OPCODE Opcode;
    switch(mState){
    case FTP_State_RECV:
      mUDPConnection.changeTargetSocket(mUDPConnection.LastRecievedSocket);
      mBuffers.getInBuffer().readBlock(octet,4);
      mBuffers.getInBuffer().advanceRead(4);
      Opcode = (FTP_OPCODE)ntohs((*(uint16_t *)octet));
      switch(Opcode)
      {
        
      case FTP_OPCODE_DATA:
        {
          uint32_t len = mBuffers.getInBuffer().getDataSize();
          int save_res = SaveBlockToMem(ntohs(*(uint16_t *)(octet+2)));
          if (save_res > 0 && len < FTP_BLOCK_SIZE ) 
            mState = FTP_State_END;
          else if (save_res == -4){
            mLastError = FTP_ERRCODE_DISK_FULL;
            SendError(FTP_ERRCODE_DISK_FULL);
            mState = FTP_State_IDLE;
          }
          mTimeout = (getTimerTicks()+10000) % 11000;
        }
        break;
      case FTP_OPCODE_ERROR:
        mTimeout = (getTimerTicks()+10000) % 11000;
        mLastError = (FTP_ERRCODE)ntohs((*(uint16_t *)(octet+2)));
        mState = FTP_State_IDLE;
        break;
      case FTP_OPCODE_RRQ:
      case FTP_OPCODE_WRQ:
      case FTP_OPCODE_ACK:
        SendError(FTP_ERRCODE_ILIGAL_OP);
        mTimeout = (getTimerTicks()+10000) % 11000;
        break;
      }
      
    default:
      break;    
    }
    while (mBuffers.getInBuffer().readChar() >= 0);
    
  }
  
  
  // **************************************************************
  // marks the telnet server as closing.
  // **************************************************************
  void FTPClient::closeSocket()
  {
    mUDPConnection.closeObject();
  }
  
  // **************************************************************
  // returns an indication whether server is closed.
  // server is closed only when connection is closed as well.
  // **************************************************************
  bool FTPClient::isClosed() const
  {
    if (!mUDPConnection.isClosed()) {
      return false;
    }
    
    return true;
  }
  
  // **************************************************************
  // Sends the discovery request to the web.
  // **************************************************************
  