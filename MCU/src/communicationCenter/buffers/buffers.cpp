// **********************************************************************
// **********************************************************************
// Copyright 2011 by Avi Owshanko. All rights reserved.
// **********************************************************************
// **********************************************************************

#include "buffers.h"
#include "globalInterface.h"
//#include "serialIRQHandlers.h"
#include "stm32Interface.h"
//#include "serialDataStructures.h"
#include <string.h>


#ifdef __GNUC__
 static char buffersPoolBase[BUFFER_POOL_SIZE + SMALL_BUFFER_SIZE];
#elif defined ( __ICCARM__ )
__no_init static char buffersPoolBase[BUFFER_POOL_SIZE + SMALL_BUFFER_SIZE];
#endif
#if (BUFFER_COUNT < BUFFER_INDEX_5003)
#error not enough memory alloced for buffers
#endif

#if (BUFFER_COUNT < BUFFER_INDEX_5003)
#error not enough memory alloced for buffers
#endif

// **********************************************************************
// **********************************************************************
// **********************   Helper functions    *************************
// **********************************************************************
// **********************************************************************

// **************************************************************
// Used for initialization of buffers
// **************************************************************
static char* getAllignedBase()
{
  char* base = (buffersPoolBase + SMALL_BUFFER_MASK);
  base = base - (((unsigned int)base) & SMALL_BUFFER_MASK);
  return base;
}

// **************************************************************
// Used for initialization of buffers
// **************************************************************
static char* getBufferBase(TCPPortTypes portType)
{
  switch (portType) {
  case UDP_PORT_DHCP:
    return (getAllignedBase());
  case UDP_PORT_FTP:
    return (getAllignedBase() + (2 * SMALL_BUFFER_SIZE));
  case UDP_PORT_FTP_FW:
    return (getAllignedBase() + (4 * SMALL_BUFFER_SIZE));
  case UDP_PORT_COMMANDS:
    return (getAllignedBase() + (6 * SMALL_BUFFER_SIZE));
  case UDP_PORT_NEIGHB:
    return (getAllignedBase() + (8 * SMALL_BUFFER_SIZE));

  default:
    break;
  }

  return NULL;;
}


// **********************************************************************
// **********************************************************************
// ************************   Duplex Buffer    **************************
// **********************************************************************
// **********************************************************************

// **************************************************************
// constructor
// tcp ports will have small output buffer
// **************************************************************
BidirectionalBuffer::BidirectionalBuffer(TCPPortTypes portType) :
  mBufferIn(getBufferBase(portType), false),
  mBufferOut((getBufferBase(portType) + SMALL_BUFFER_SIZE), false)
{
}


// **************************************************************
// resets both in and out buffer.
// THIS IS NOT MULTITHREADED SAFE. DO NOT USE WHEN INTERRUPTS ARE ACTIVE.
// **************************************************************
void BidirectionalBuffer::reset()
{
  mBufferIn.reset();
  mBufferOut.reset();
}

// **************************************************************
// **************************************************************
void BidirectionalBuffer::advanceReadTop()
{
  mBufferIn.advanceReadTop();
  mBufferOut.advanceReadTop();
}

// **********************************************************************
// **********************************************************************
// ***************************   Buffer     *****************************
// **********************************************************************
// **********************************************************************

// **************************************************************
// Constructor
// **************************************************************
Buffer::Buffer(char* base, bool isLargeBuffer) :
  mBufferReadBase(base),
  mBufferReadTop(base),
  mBufferWrite(base),
  mLargeBuffer(isLargeBuffer)
{
}

// **************************************************************
// Resets a buffer to empty.
// NOTE - THIS OPERATION IS NOT MULTI THREADED SAFE
// (it is protected against interrupts to some length)
// **************************************************************
void Buffer::reset()
{
  char* base = GetBufferBase(mBufferReadBase);

  while ((mBufferReadTop != base) ||
         (mBufferReadBase != base) ||
         (mBufferWrite != base))
  {
    // for maximal interrupt protection, order is imporatant.
    mBufferReadTop = base;
    // note that if buffer was empty before , and we received a read interrupt
    // then a 'readSmallChar' will not work correctly
    mBufferReadBase = base;
    mBufferWrite = base;
  }
}

// **************************************************************
// returns the amount of space left in buffer
// use the 'mBufferWrite' for the top address.
// **************************************************************
unsigned int Buffer::getFreeSize() const
{
  const char* nextRead = mBufferReadBase;
  const char* nextWrite = mBufferWrite;

  unsigned int dataSize = ((unsigned int)(nextWrite - nextRead)) & GetBufferMask();
  return (GetBufferMask() - dataSize);
}

// **************************************************************
// returns the amount of data that was written, but can not be read
// **************************************************************
unsigned int Buffer::getUnreadSize() const
{
  const char* readTop = mBufferReadTop;
  const char* nextWrite = mBufferWrite;

  unsigned int dataSize = ((unsigned int)(nextWrite - readTop)) & GetBufferMask();
  return dataSize;
}

// **************************************************************
// returns the amount of space that reader can read from buffer
// use the 'mBufferReadTop' for the top address.
// **************************************************************
unsigned int Buffer::getDataSize() const
{
  const char* readTop = mBufferReadTop;
  const char* readBase = mBufferReadBase;

  unsigned int dataSize = ((unsigned int)(readTop - readBase)) & GetBufferMask();
  return dataSize;
}

// **************************************************************
// Writes a character to buffer.
// Gives no indication on failure.
// THIS IS A CRITICAL PERFORMANCE FUNCTION
// **************************************************************
void Buffer::writeLargeChar(char ch)
{
  // use local for multithread race.
  char* nextRead = mBufferReadBase;
  char* nextWrite = mBufferWrite;

  unsigned int diff = nextWrite - nextRead;
  if ((diff & LARGE_BUFFER_MASK) == LARGE_BUFFER_MASK) {
    // buffer is full
    return;
  }

  *nextWrite = ch;
  mBufferWrite = GetLargeBufferNewAddr(nextWrite, 1);
}

// **************************************************************
// Writes a character to buffer.
// Gives no indication on failure.
// **************************************************************
void Buffer::writeChar(char ch)
{
  if (mLargeBuffer) {
    return writeLargeChar(ch);
  }

  // use local for multithread race.
  char* nextRead = mBufferReadBase;
  char* nextWrite = mBufferWrite;

  unsigned int diff = nextWrite - nextRead;
  if ((diff & SMALL_BUFFER_MASK) == SMALL_BUFFER_MASK) {
    // buffer is full
    return;
  }

  *nextWrite = ch;
  mBufferWrite = GetSmallBufferNewAddr(nextWrite, 1);
}

// **************************************************************
// Writes a character to buffer.
// Gives no indication on failure.
// **************************************************************
void Buffer::writeShort(uint16_t sh)
{
  writeChar((sh >> 8)&0xFF);
  writeChar(sh & 0xFF);
}

// **************************************************************
// Writes a character to buffer n Times.
// Gives no indication on failure.
// **************************************************************
int Buffer::writeNChar(char ch, int len)
{
  // always allow writing '0' characters.
  if (len == 0) {
    return 0;
  }
  // use local for multithread race.
  char* nextRead = mBufferReadBase;
  char* nextWrite = mBufferWrite;
  
  unsigned int dataSize = (nextWrite - nextRead) & GetBufferMask();
  if (dataSize + len > GetBufferMask()) {
    // buffer will be full
    return -1;
  }
  
  for (int i =0; i < len; i++)
    writeChar(ch);
  
  return len;
}

// **************************************************************
// Reads a single character from buffer.
// Returns negative on failure (buffer empty)
// THIS IS A CRITICAL PERFORMANCE FUNCTION
// **************************************************************
int Buffer::readSmallChar()
{
  // use local for multithread race.
  char* readBase = mBufferReadBase;
  char* readTop = mBufferReadTop;
  if (readBase == readTop) {
    return -1;
  }

  int retVal = *readBase;
  mBufferReadBase = GetSmallBufferNewAddr(readBase, 1);

  return retVal;
}

// **************************************************************
// Reads a single character from buffer.
// Returns negative on failure (buffer empty)
// **************************************************************
int Buffer::readChar()
{
  if (!mLargeBuffer) {
    return readSmallChar();
  }

  // use local for multithread race.
  char* readBase = mBufferReadBase;
  char* readTop = mBufferReadTop;
  if (readBase == readTop) {
    return -1;
  }

  int retVal = *readBase;
  mBufferReadBase = GetLargeBufferNewAddr(readBase, 1);

  return retVal;
}

int Buffer::readShort()
{
  int c1 = readChar();
  if (c1 < 0) return -1;
  int c2 = readChar();
  if (c2 < 0) return -1;
  return  ((c2 & 0xFF) + ((c1 & 0xFF) << 8));
}

// **************************************************************
// Writes a block to buffer.
// Returns negative on failure
// **************************************************************
int Buffer::writeBlock(const void* source, unsigned int len)
{
  // always allow writing '0' characters.
  if (len == 0) {
    return 0;
  }

  // use local for multithread race.
  char* nextRead = mBufferReadBase;
  char* nextWrite = mBufferWrite;

  unsigned int dataSize = (nextWrite - nextRead) & GetBufferMask();
  if (dataSize + len > GetBufferMask()) {
    // buffer will be full
    return -1;
  }

  // 'writeOffset' is relative to buffer base.
  unsigned int writeOffset = ((unsigned int)nextWrite) & GetBufferMask();

  // We do not test for equality so that we will not need to 
  // apply MOD after operation ends.
  if ((writeOffset + len) < GetBufferSize()) {    // single block copy
    // most times we will go through this clause.
    memcpy(nextWrite, source, len);
    mBufferWrite = nextWrite + len;
  }
  else {                                      // multiple block copy.
    // note that 'dataWrite' may equal 'len'
    unsigned int dataWrite = GetBufferSize() - writeOffset;
    memcpy(nextWrite, source, dataWrite);
    // move 'nextWrite' to buffer base.
    nextWrite = nextWrite - writeOffset;
    memcpy(nextWrite, ((const char *)source) + dataWrite, len - dataWrite);
    mBufferWrite = nextWrite + len - dataWrite;
  }

  return len;
}

// **************************************************************
// Reads a block from buffer to 'target'.
// Returns negative on failure
// **************************************************************
int Buffer::readBlock(char* target, unsigned int len) const
{
  return readBlock(target, 0, len);
}

// **************************************************************
// Reads a block from buffer to 'target'.
// Returns negative on failure
// **************************************************************
int Buffer::readBlock(char* target,
                      unsigned int bufferOffset,
                      unsigned int len) const
{
  // always allow reading '0' characters.
  if (len == 0) {
    return 0;
  }

  // use local for multithread race.
  const char* readTop = mBufferReadTop;
  const char* readBase = mBufferReadBase;

  unsigned int dataSize = (readTop - readBase) & GetBufferMask();
  if (dataSize < (len + bufferOffset)) {
    // not enough data in buffer
    return -1;
  }
  readBase = GetBufferNewAddr(readBase, bufferOffset);

  // 'readOffset' is relative to buffer base.
  unsigned int readOffset = ((unsigned int)readBase) & GetBufferMask();

  // We do not test for equality so that we will not need to 
  // apply MOD after operation ends.
  if ((readOffset + len) < GetBufferSize()) {     // single block operation
    // most times we will go through this clause.
    memcpy(target, readBase, len);
  }
  else {                                      // multiple block operation.
    // note that 'dataWrite' may equal 'len'
    unsigned int dataRead = GetBufferSize() - readOffset;
    memcpy(target, readBase, dataRead);
    memcpy(target + dataRead, readBase - readOffset, len - dataRead);
  }

  return len;
}

// **************************************************************
// Used for updating read location.
// Seperated from the actual read action due to TCP ACK mechanism.
// **************************************************************
void Buffer::advanceRead(unsigned int len)
{
  // use local for multithread race.
  const char* readTop = mBufferReadTop;
  const char* readBase = mBufferReadBase;

  unsigned int dataSize = (readTop - readBase) & GetBufferMask();
  if (dataSize < len) {
    // do nothing on error.
    return;
  }

  mBufferReadBase = GetBufferNewAddr(readBase, len);
}

// **************************************************************
// advances the read top marker to point to the end of buffer data
// **************************************************************
void Buffer::advanceReadTop()
{
  mBufferReadTop = mBufferWrite;
}

// **************************************************************
// advances the read top marker to point to the end of buffer data
// **************************************************************
void Buffer::advanceReadTop(unsigned int len)
{
  // use local for multithread race.
  const char* readTop = mBufferWrite;
  const char* readBase = mBufferReadTop;

  unsigned int dataSize = (readTop - readBase) & GetBufferMask();
  if (dataSize < len) {
    // advance as much as possible
    len = dataSize;
  }

  mBufferReadTop = GetBufferNewAddr(readBase, len);
}

// **************************************************************
// Reads a block from buffer to 'target'.
// Returns negative on failure
// **************************************************************
int Buffer::readTopBlock(unsigned char* target,
                         unsigned int topOffset,
                         unsigned int maxLen) const
{
  // always allow reading '0' characters.
  if (maxLen == 0) {
    return 0;
  }

  // use local for multithread race.
  const char* readTop = mBufferWrite;
  const char* readBase = mBufferReadTop;

  unsigned int len = (readTop - readBase) & GetBufferMask();
  if (len <= topOffset) {
    // not enough data in buffer
    int retVal = -(topOffset - len);
    return retVal;
  }

  readBase = GetBufferNewAddr(readBase, topOffset);
  len -= topOffset;
  if (len > maxLen) {
    len = maxLen;
  }

  // 'readOffset' is relative to buffer base.
  unsigned int readOffset = ((unsigned int)readBase) & GetBufferMask();

  // We do not test for equality so that we will not need to 
  // apply MOD after operation ends.
  if ((readOffset + len) < GetBufferSize()) {     // single block operation
    // most times we will go through this clause.
    memcpy(target, readBase, len);
  }
  else {                                      // multiple block operation.
    // note that 'dataWrite' may equal 'len'
    unsigned int dataRead = GetBufferSize() - readOffset;
    memcpy(target, readBase, dataRead);
    memcpy(target + dataRead, readBase - readOffset, len - dataRead);
  }

  return len;
}
