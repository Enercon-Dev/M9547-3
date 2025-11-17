// **********************************************************************
// **********************************************************************
// Copyright 2011 by Avi Owshanko. All rights reserved.
// **********************************************************************
// **********************************************************************

#ifndef BUFFER_FLUSH_H
#define BUFFER_FLUSH_H

#include "serialDataStructures.h"
#include "buffers.h"

// predeclaration
class BidirectionalBuffer;
class SerialBufferStreamSettings;

#define BUFFER_FLUSH_SIZE_LIMIT (512)

// *******************************************************************
// A class used for managing buffer flushing
// *******************************************************************
class BufferFlush
{
public:
  BufferFlush(BidirectionalBuffer&);

  void updateCriteria(const SerialBufferStreamSettings&);
  void testFlush();
  const SerialBufferStreamSettings& getCriteria() const {return mCriteria;}


private:
  void initPattern();
  void flushBuffer();
  void limitedFlush(unsigned int len);

  void testPattern();
  void testTimeout();
  void testSizeLimit();


  unsigned int mLastOffsetChecked;

  // timeout management
  uint32_t mLeaseTime;

  // pattern management
  uint32_t mCurrentPattern;
  uint32_t mTargetPattern;
  uint32_t mUnmatchPattern;
  uint32_t mPatternMask;

  SerialBufferStreamSettings mCriteria;
  BidirectionalBuffer& mBuffer;
};























#endif // BUFFER_FLUSH_H