// **********************************************************************
// **********************************************************************
// Copyright 2011 by Avi Owshanko. All rights reserved.
// **********************************************************************
// **********************************************************************

#ifndef PERSISTENT_OBJECT_H
#define PERSISTENT_OBJECT_H

#include <stdint.h>

#define SINGLE_DATA_SINGLE_BYTE_MASK ((unsigned char)0x80)
#define SINGLE_DATA_SINGLE_WORD_MASK ((unsigned char)0x40)
#define SINGLE_DATA_MASK (SINGLE_DATA_SINGLE_BYTE_MASK | SINGLE_DATA_SINGLE_WORD_MASK)
#define SINGLE_DATA_NOP ((unsigned char)0xFF)

#define PERSISTENT_OBJECT_VERSION 0x0002
#define PERSISTENT_OBJECT_HEADER_SIZE (0x10)

#define PERSISTENT_VERSION_OFFSET (0x00)
#define PERSISTENT_RESET_COUNTER_OFFSET (0x04)

#define PERSISTENT_COPY_A (0x01)
#define PERSISTENT_COPY_B (0x02)
#define PERSISTENT_COPY_C (0x04)

// *******************************************************************
// A class for holding the next read answer / next write action.
// *******************************************************************
class PersistentSingleData
{
public:
  PersistentSingleData(unsigned char opCode, unsigned char length, const unsigned char* base);
  PersistentSingleData(unsigned char opCode, uint32_t data);

  unsigned char length() const;
  unsigned char opcode() const {return mOpCode;}
  uint32_t singleData() const {return mData;}
  const unsigned char* dataBase() const {return mBase;}
  unsigned char  dataLen() const {return mLength;}

  bool isNopOpcode() const {return (mOpCode == SINGLE_DATA_NOP);}
  bool isSingleData() const {return ((mOpCode & SINGLE_DATA_MASK) != 0);}
  bool isSingleByte() const {return ((mOpCode & SINGLE_DATA_SINGLE_BYTE_MASK) != 0);}
  bool isSingleWord() const {return ((mOpCode & SINGLE_DATA_SINGLE_WORD_MASK) != 0);}

private:
  const unsigned char* mBase;           // base points to the data address
  unsigned char mOpCode;                // an opcode of '-1' means invalid.
  unsigned char mLength;                // relevant only if opcode is under 0x40
  uint32_t mData;                       // relevant only if short opcode
};

// *******************************************************************
// The actual basic persistent object.
// *******************************************************************
class PersistentObject
{
public:
  void loadPersistentObject();
  void initPersistentObject();

protected:
  PersistentObject(int blockNum, uint16_t version);

  virtual void writeFullData() = 0;
  virtual void processNextEntry(const PersistentSingleData&) = 0;

  // Following three methods are used for initializing persistent data.
  void resetFirstCopy();
  void writeFirstCopyDataObject(const PersistentSingleData& data);
  void resetOtherCopies();

  void writeDataObject(const PersistentSingleData& data);
  PersistentSingleData readSingleData();

  bool mIsGood;

private:
  void testCopyIntegrity();
  void testDataIntegrity();
  void copyBlock(uint32_t target, uint32_t src);
  PersistentSingleData readSingleData(uint16_t offset);
  void writeFlash(uint32_t target, uint16_t val);
  void internalLoadPersistentObject();
  void writeAdminData(uint32_t base);
  uint16_t writeDataObject(const uint32_t base, const PersistentSingleData& data);

  uint16_t writeDataObject_2(uint32_t base, const PersistentSingleData& data);
  uint16_t writeDataObject_4(uint32_t base, const PersistentSingleData& data);
  // NOTE - 'dataSize' in 'writeDataObject_7F' is the number of short words in 'data'
  uint16_t writeDataObject_7F(uint32_t base, const uint16_t* data, unsigned char opcode, unsigned char dataSize);

  const uint32_t mBlockBase;
  uint16_t mOffset;

  const uint16_t mVersion;
  uint32_t mResetCounter;
};



#endif // PERSISTENT_OBJECT_H
