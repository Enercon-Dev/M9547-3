// **********************************************************************
// **********************************************************************
// Copyright 2011 by Avi Owshanko. All rights reserved.
// **********************************************************************
// **********************************************************************

#include "persistentSystemStats.h"
#include "persistentData.h"
#include "stm32Interface.h"
#include "core_cm3.h"
#include <string.h>

// **********************************************************************
// **********************************************************************
// ******************   Persistent System Settings     ******************
// **********************************************************************
// **********************************************************************

// **************************************************************
// constructor
// **************************************************************
PersistentSystemStats::PersistentSystemStats() :
  mRandomNumber(0)
{
}

// **************************************************************
// Generates the next 16 quasy random sequence number
// **************************************************************
uint16_t PersistentSystemStats::getNext16RandomNumber()
{
  if (mRandomNumber == 0) {
    initRandomNumber();
  }

  mRandomNumber = mRandomNumber * RANDOM_SEQ_MUL;
  mRandomNumber += SysTick->VAL + RANDOM_SEQ_ADD;
  mRandomNumber = mRandomNumber % RANDOM_SEQ_MOD;

  return mRandomNumber;
}

// **************************************************************
// initiates the random number according to reset counter and chip ID.
// **************************************************************
void PersistentSystemStats::initRandomNumber()
{
  mRandomNumber = SysTick->VAL * RANDOM_RST_MUL;

  unsigned char* chipIdAddr = (unsigned char*)(PERSISTENT_BLOCK_BASE(PERSISTENT_DATA_STATIC_BLOCK_A) + CHIP_ID_OFFSET);

  int i;
  for (i=0; i<12; i++) {
    unsigned char ch = chipIdAddr[i];
    mRandomNumber = (mRandomNumber + ch) * RANDOM_CHIP_MUL;
    mRandomNumber = mRandomNumber % RANDOM_CHIP_MOD;
  }
}
