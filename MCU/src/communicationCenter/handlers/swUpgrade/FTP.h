// **********************************************************************
// **********************************************************************
// Copyright 2011 by Avi Owshanko. All rights reserved.
// **********************************************************************
// **********************************************************************

#ifndef FTP_H
#define FTP_H

#include <stdint.h>
#include "../../ethernet/udpConnection.h"
#include "../../ethernet/mac.h"
#include "../../buffers/buffers.h"
#include <string.h>

#define FTP_TIMEOUT_INTERVAL (5000)
#define FTP_CLIENT_PORT (0x4510)
#define FTP_HW_CLIENT_PORT (0x4610)
#define FTP_SERVER_PORT (0x4500)
#define FTP_BLOCK_SIZE (512)

#define MAX_FILENAME_LENGTH (32)
#define NUM_OF_RETRIES (10)

typedef enum {FTP_State_IDLE, FTP_State_RECV, FTP_State_TIMEOUT, FTP_State_END} FTP_State;
typedef enum {FTP_OPCODE_RRQ = 1, FTP_OPCODE_WRQ = 2, FTP_OPCODE_DATA = 3, FTP_OPCODE_ACK = 4, FTP_OPCODE_ERROR = 5 } FTP_OPCODE;
typedef enum {FTP_ERRCODE_UNKNOWN = 1, FTP_ERRCODE_FILE_NOT_FOUND = 2, FTP_ERRCODE_DISK_FULL = 3, FTP_ERRCODE_ILIGAL_OP = 4, 
              FTP_ERRCODE_UNKNOWN_ID = 5, FTP_ERRCODE_FILE_EXIST = 6, FTP_ERRCODE_UNKNOW_USER = 7 , FTP_ERRCODE_NO_ERROR = 0xFFFF, FTP_ERRCODE_TIMEOUT = 0xF0F0 } FTP_ERRCODE;

void FTP_OnReceiveEndDefault(uint32_t len);
int FTP_OnReceiveDefault(Buffer& buf, uint16_t block);
// *******************************************************************
// A class for managing DHCP connection through port 67,68
// *******************************************************************
class FTPClient
{
public: 
  FTPClient(uint16_t, TCPPortTypes, Layer3&, void (*)(uint32_t) = FTP_OnReceiveEndDefault, int (*)(Buffer&, uint16_t) = FTP_OnReceiveDefault );
  
  int RequestFile( uint32_t ServerIP, const char* FileName);

  // upper layers requests
  void periodicCall();
  void closeSocket();
  void openSocket();
  bool isClosed() const;
  
  FTP_ERRCODE mLastError;
  FTP_State mState;
  uint32_t mBytesReceived;
  uint32_t mServerIP;
  char mFileName[MAX_FILENAME_LENGTH+1];
  void (*OnReceiveEnd)(uint32_t);
  int   (*OnReceive)(Buffer&, uint16_t);

private:

  void readBuffer();
  int Retransmit();
  int SendError(FTP_ERRCODE ErrCode);
  int SaveBlockToMem(uint16_t BlockNum);
  

// internal data
  BidirectionalBuffer mBuffers;
  const uint16_t mSocket;
  UDPConnection mUDPConnection;
  

  uint16_t mLastBlockReceived;
  uint16_t mTimeout;
  uint8_t mRetryCounter;
  
};



#endif // FTP_H