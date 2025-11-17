// **********************************************************************
// **********************************************************************
// Copyright 2011 by Avi Owshanko. All rights reserved.
// **********************************************************************
// **********************************************************************

#ifndef TIME_ACC_H
#define TIME_ACC_H

#include <stdint.h>
#include "globalInterface.h"
#include "stm32Interface.h"

class TimeAcc
{
public:
  TimeAcc() : mAccumulatedTime(0), mAccumulatedCalls(0) {}
  void start() {mBase = getTimerTicks(); mAccumulatedCalls++;}
  void stop() {mAccumulatedTime += (getTimerTicks()-mBase);}

private:
  uint32_t mAccumulatedTime;
  uint32_t mBase;
  uint32_t mAccumulatedCalls;
};

class TimeAccWrapper
{
public:
  TimeAccWrapper(TimeAcc& accCopy) : mAccCopy(accCopy) {mAccCopy.start();}
  ~TimeAccWrapper() {mAccCopy.stop();}
  
private:
  TimeAcc& mAccCopy;
};





#endif  // TIME_ACC_H