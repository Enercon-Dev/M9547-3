// **********************************************************************
// **********************************************************************
// Copyright 2011 by Avi Owshanko. All rights reserved.
// **********************************************************************
// **********************************************************************

#include "bufferFlush.h"
#include "buffers.h"
#include "stm32Interface.h"

// **********************************************************************
// **********************************************************************
// *************************   Buffer Flush    **************************
// **********************************************************************
// **********************************************************************

// **************************************************************
// Constructor
// **************************************************************
BufferFlush::BufferFlush(BidirectionalBuffer& buffer) :
  //mCriteria(),
  mBuffer(buffer)
{
  initPattern();
}

// **************************************************************
// reloads settings
// **************************************************************
void BufferFlush::updateCriteria(const SerialBufferStreamSettings& newSettings)
{
  //mCriteria = newSettings;
  initPattern();
}

// **************************************************************
// tests buffer against flush criteria
// **************************************************************
void BufferFlush::testFlush()
{
  mBuffer.getInBuffer().advanceReadTop();

  testPattern();
  testTimeout();
  testSizeLimit();
}

// **************************************************************
// initializes the pattern matching data.
// **************************************************************
void BufferFlush::initPattern()
{
  switch (mCriteria.mFlushCriteria) {
  case TRANSCEIVER_FLUSH_HEADER:
  case TRANSCEIVER_FLUSH_TRAILER:
    // init pattern matching data
    mPatternMask = mCriteria.mPattern.getPatternMask();
    mUnmatchPattern = mCriteria.mPattern.getUnmatchPattern();
    mTargetPattern = mCriteria.mPattern.getTargetPattern();
    mCurrentPattern = mUnmatchPattern;
    break;

  default:
    // disable pattern matching
    mPatternMask = 0;
  }

  mLastOffsetChecked = 0;
}

// **************************************************************
// initializes the pattern matching data.
// **************************************************************
void BufferFlush::flushBuffer()
{
  mBuffer.getOutBuffer().advanceReadTop();
  mLastOffsetChecked = 0;
  mCurrentPattern = mUnmatchPattern;
}

// **************************************************************
// initializes the pattern matching data.
// **************************************************************
void BufferFlush::limitedFlush(unsigned int len)
{
  mBuffer.getOutBuffer().advanceReadTop(len);

  if (mLastOffsetChecked > len) {
    mLastOffsetChecked -= len;
  }
  else {
    mLastOffsetChecked = 0;
  }
}

// **************************************************************
// tests buffer against flush criteria
// **************************************************************
void BufferFlush::testPattern()
{
  unsigned char target[BUFFER_FLUSH_SIZE_LIMIT];
  int i;

  if (mPatternMask == 0) {
    return;
  }

  int dataSize = mBuffer.getOutBuffer().readTopBlock(target, mLastOffsetChecked, BUFFER_FLUSH_SIZE_LIMIT);
  if (dataSize <= 0) {
    return;
  }
  // do not forget to increase lease each time we read some data...
  mLeaseTime = getTimerLease(mCriteria.mTimeout);

  unsigned int toFlush = 0;
  for (i=0; i < dataSize; i++) {
    mLastOffsetChecked++;
    mCurrentPattern = (mCurrentPattern << 8) + target[i];

    if ((mCurrentPattern & mPatternMask) == mTargetPattern) {
      // found pattern - mark it (overrun previous pattern if found)
      toFlush = mLastOffsetChecked;
      // start looking for a new pattern.
      mCurrentPattern = mUnmatchPattern;
    }
  }
  if ((toFlush > 0) && (mCriteria.mFlushCriteria == TRANSCEIVER_FLUSH_HEADER)) {
    // if flush criteria is the header of next message, remove header from flushing.
    toFlush -= mCriteria.mPattern.getPatternLen();
  }
  if (toFlush > 0) {
    limitedFlush(toFlush);
  }
}

// **************************************************************
// tests buffer against timeout limit
// **************************************************************
void BufferFlush::testTimeout()
{
  unsigned int unreadSize = mBuffer.getOutBuffer().getUnreadSize();
  if (unreadSize == 0) {
    return;
  }

  if (mCriteria.mFlushCriteria == TRANSCEIVER_FLUSH_NONE) {
    flushBuffer();
    return;
  }

  if (unreadSize > mLastOffsetChecked) {
    mLeaseTime = getTimerLease(mCriteria.mTimeout);
  }
  mLastOffsetChecked = unreadSize;

  if (getLeaseTime(mLeaseTime) < 0) {
    flushBuffer();
    return;
  }
}

// **************************************************************
// tests buffer against size limit
// **************************************************************
void BufferFlush::testSizeLimit()
{
  if (mLastOffsetChecked >= BUFFER_FLUSH_SIZE_LIMIT) {
    limitedFlush(BUFFER_FLUSH_SIZE_LIMIT);
    mCurrentPattern = mUnmatchPattern;
  }
}
