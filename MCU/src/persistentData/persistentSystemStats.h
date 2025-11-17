// **********************************************************************
// **********************************************************************
// Copyright 2011 by Avi Owshanko. All rights reserved.
// **********************************************************************
// **********************************************************************

#ifndef PERSISTENT_SYSTEM_STATS_H
#define PERSISTENT_SYSTEM_STATS_H

#include <stdint.h>

// opcodes
#define PERSISTENT_SYSTEM_RESET_REGISTER_OPCODE        (0x01 | SINGLE_DATA_SINGLE_BYTE_MASK)
#define PERSISTENT_SYSTEM_RESET_COUNT_OPCODE           (0x02 | SINGLE_DATA_SINGLE_WORD_MASK)

// random generator hash constants
#define RANDOM_SEQ_MUL (0x3053)
#define RANDOM_SEQ_MOD (0xF309)
#define RANDOM_SEQ_ADD (0x6123)
#define RANDOM_RST_MUL (0x5911)
#define RANDOM_CHIP_MUL (0x6351)
#define RANDOM_CHIP_MOD (0xF311)

// **************************************************************
// The Serial persistent object
// **************************************************************
class PersistentSystemStats
{
public:
  PersistentSystemStats();
  uint16_t getNext16RandomNumber();

private:
  void initRandomNumber();
  uint16_t mRandomNumber;
};








#endif // PERSISTENT_SYSTEM_STATS_H