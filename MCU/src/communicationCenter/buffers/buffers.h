// **********************************************************************
// **********************************************************************
// Copyright 2011 by Avi Owshanko. All rights reserved.
// **********************************************************************
// **********************************************************************

#ifndef BUFFERS_H
#define BUFFERS_H

#include "../ethernet/ethernetCommon.h"

// We have two kinds of buffers - the small ones (2KB), and large ones
// that are twice as large as the small ones (4KB).
#define SMALL_BUFFER_SIZE ((unsigned int)0x800)
#define LARGE_BUFFER_SIZE (2*SMALL_BUFFER_SIZE)
#define SMALL_BUFFER_MASK (SMALL_BUFFER_SIZE - 1)
#define LARGE_BUFFER_MASK (LARGE_BUFFER_SIZE - 1)
#define SMALL_BUFFER_COUNT (5)
#define LARGE_BUFFER_COUNT (0)
#define SMALL_BUFFER_POOL_SIZE (SMALL_BUFFER_COUNT * 2 * SMALL_BUFFER_SIZE)
#define LARGE_BUFFER_POOL_SIZE (LARGE_BUFFER_COUNT * 3 * SMALL_BUFFER_SIZE)
#define BUFFER_POOL_SIZE (SMALL_BUFFER_POOL_SIZE + LARGE_BUFFER_POOL_SIZE)

// small buffer offsets
#define GetSmallBufferBase(bufferInternalAddr)                \
  ((char*)(((unsigned int)bufferInternalAddr) & ~SMALL_BUFFER_MASK))
#define GetSmallBufferOffset(addr, inc)                       \
  (((unsigned int)(addr + inc)) & SMALL_BUFFER_MASK)
#define GetSmallBufferNewAddr(oldAddr, inc)                   \
  (GetSmallBufferBase(oldAddr) + GetSmallBufferOffset(oldAddr, inc))

// large buffer offsets
#define GetLargeBufferBase(bufferInternalAddr)                \
  ((char*)(((unsigned int)bufferInternalAddr) & ~LARGE_BUFFER_MASK))
#define GetLargeBufferOffset(addr, inc)                       \
  (((unsigned int)(addr + inc)) & LARGE_BUFFER_MASK)
#define GetLargeBufferNewAddr(oldAddr, inc)                   \
  (GetLargeBufferBase(oldAddr) + GetLargeBufferOffset(oldAddr, inc))

#define GetBufferBase(bufferInternalAddr)                     \
  (mLargeBuffer ? GetLargeBufferBase(bufferInternalAddr) : GetSmallBufferBase(bufferInternalAddr))
#define GetBufferSize()                                       \
  (mLargeBuffer ? LARGE_BUFFER_SIZE : SMALL_BUFFER_SIZE)
#define GetBufferMask()                                       \
  (mLargeBuffer ? LARGE_BUFFER_MASK : SMALL_BUFFER_MASK)
#define GetBufferNewAddr(oldAddr, inc)                        \
  (mLargeBuffer ? GetLargeBufferNewAddr(oldAddr, inc) : GetSmallBufferNewAddr(oldAddr, inc))

// predeclaration
class SerialStreamPattern;

// *******************************************************************
// A class implementing a sinle direction buffer
// This is a very delicate class, with 'mBufferRead', 'mBufferWrite'.
// We need this is as it is a performance critical class.
// *******************************************************************
class Buffer
{
public:
  Buffer(char* base, bool isLargeBuffer);

  void reset();                         // not multithreaded safe!

  // use these for fast serial access
  void writeChar(char ch);
  void writeShort(uint16_t sh);
  int writeNChar(char ch, int len);
  void writeLargeChar(char ch);         // large buffer - use with care
  int readChar();
  int readShort();
  int readSmallChar();                  // small buffer - use with care

  // use these for ethernet access
  int writeBlock(const void* source, unsigned int len);
  int readBlock(char* target, unsigned int len) const;
  int readBlock(char* target, unsigned int bufferOffset, unsigned int len) const;
  void advanceRead(unsigned int len);

  // used for marking input data as relevant
  void advanceReadTop();
  void advanceReadTop(unsigned int len);
  int readTopBlock(unsigned char* target, unsigned int topOffset, unsigned int maxLen) const;

  unsigned int getFreeSize() const;
  unsigned int getUnreadSize() const;
  unsigned int getDataSize() const;

private:
  // The variable is volatile, as opposed to the reference itself.
  char* volatile mBufferReadBase;               // current read location
  char* volatile mBufferReadTop;                // highest allowed read location
  char* volatile mBufferWrite;                  // write location
  const bool mLargeBuffer;
};

// *******************************************************************
// A full duplex buffer - both in and out stream
// *******************************************************************
class BidirectionalBuffer
{
public:
  BidirectionalBuffer(TCPPortTypes portType);
  //BidirectionalBuffer(SerialPortNum portNum);

  void reset();                         // not multithreaded safe!

  void advanceReadTop();

  Buffer& getInBuffer() {return mBufferIn;}
  Buffer& getOutBuffer() {return mBufferOut;}
  
private:
  Buffer mBufferIn;
  Buffer mBufferOut;
};


// Following is used for initializing the irq handlers
//void initIRQSerialBuffer(SerialPortNum, BidirectionalBuffer*);

#endif // BUFFERS_H