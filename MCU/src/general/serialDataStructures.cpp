// **********************************************************************
// **********************************************************************
// Copyright 2011 by Avi Owshanko. All rights reserved.
// **********************************************************************
// **********************************************************************

#include "serialDataStructures.h"
#include "communicationCenter/ethernet/ethernetCommon.h"
#include "systemManagement.h"
#include "communicationCenter/communicationCenter.h"
#include <string.h>

// **********************************************************************
// **********************************************************************
// ***************   Serial Structures Initialization     ***************
// **********************************************************************
// **********************************************************************

// **************************************************************
// constructor
// **************************************************************
/*SerialPhysicalStreamSettings::SerialPhysicalStreamSettings() :
  mType(TRANSCEIVER_TYPE_RS232),
  mSpeed(TRANSCEIVER_SPEED_19200),
  mDatabits(TRANSCEIVER_DATA_8),
  mStopbits(TRANSCEIVER_STOPBITS_1),
  mParity(TRANSCEIVER_PARITY_NONE)
{
}

// **************************************************************
// translates baud rate to integer
// **************************************************************
uint32_t SerialPhysicalStreamSettings::getBaudRate() const
{
  switch (mSpeed) {
  case TRANSCEIVER_SPEED_1200:
    return 1200;
  case TRANSCEIVER_SPEED_2400:
    return 2400;
  case TRANSCEIVER_SPEED_4800:
    return 4800;
  case TRANSCEIVER_SPEED_9600:
    return 9600;
  case TRANSCEIVER_SPEED_19200:
    return 19200;
  case TRANSCEIVER_SPEED_38400:
    return 38400;
  case TRANSCEIVER_SPEED_57600:
    return 57600;
  case TRANSCEIVER_SPEED_115200:
    return 115200;
  case TRANSCEIVER_SPEED_230400:
    return 230400;
  default:
    break;
  }
  return 0;
}

// **************************************************************
// Comparator
// **************************************************************
bool operator==(const SerialPhysicalStreamSettings& first, const SerialPhysicalStreamSettings& second)
{
  return ((first.mType == second.mType) &&
          (first.mSpeed == second.mSpeed) &&
          (first.mDatabits == second.mDatabits) &&
          (first.mStopbits == second.mStopbits) &&
          (first.mParity == second.mParity));
}
*/  
// **************************************************************
// constructor
// **************************************************************
SerialBufferStreamSettings::SerialBufferStreamSettings() :
  mFlushCriteria(TRANSCEIVER_FLUSH_TIMEOUT),
  mTimeout(SERIAL_DEFUALT_TIMEOUT),
  mPattern(0x01000000)
{
}

// **************************************************************
// Comparator
// **************************************************************
bool operator==(const SerialBufferStreamSettings& first, const SerialBufferStreamSettings& second)
{
  return ((first.mFlushCriteria == second.mFlushCriteria) &&
          (first.mTimeout == second.mTimeout) &&
          (first.mPattern.toU32() == second.mPattern.toU32()));
}

// **************************************************************
// constructor
// **************************************************************
EthernetStreamSettings::EthernetStreamSettings(SerialPortNum portNum) :
  mTransportType(TRANSPORT_TYPE_TCP),
  mLocalSocket(getDefaultSocketAddr(portNum)),
  mTargetSocket(getDefaultSocketAddr(portNum)),
  mTargetIP(0)
{
}

// **************************************************************
// returns the default socket number for 'portNum'
// **************************************************************
uint16_t EthernetStreamSettings::getDefaultSocketAddr(SerialPortNum portNum)
{
  switch (portNum) {
  case SERIAL_PORT_1:
    return SOCKET_5000;
  case SERIAL_PORT_2:
    return SOCKET_5001;
  case SERIAL_PORT_3:
    return SOCKET_5002;
  case SERIAL_PORT_4:
    return SOCKET_5003;
  default:
    break;
  }

  return 0;
}
/*
// **********************************************************************
// **********************************************************************
// *********************   Single Port Settings    **********************
// **********************************************************************
// **********************************************************************

// **************************************************************
// constructor
// **************************************************************
PortSystemSettings::PortSystemSettings(SerialPortNum portNum) :
  //mPhysicalSettings(),
  mBufferSettings(),
  mEthernetSettings(portNum)
{
}

// **************************************************************
// captures persistent system settings
// **************************************************************
void PortSystemSettings::copyPersistentSettings(SerialPortNum portNum)
{
  //mPhysicalSettings = SystemManagement::getSerialPhysicalStreamSettings(portNum);
  mBufferSettings = SystemManagement::getSerialBufferStreamSettings(portNum);
  mEthernetSettings = SystemManagement::getEthernetStreamSettings(portNum);
}

// **************************************************************
// captures current system settings
// **************************************************************
void PortSystemSettings::copyCurrentSettings(CommunicationCenter& communicationCenter,
                                             SerialPortNum portNum)
{
  //mPhysicalSettings = communicationCenter.getPhysicalState(portNum);
  mBufferSettings = communicationCenter.getBufferState(portNum);
  mEthernetSettings = communicationCenter.getEhternetState(portNum);
}

// **********************************************************************
// **********************************************************************
// ***********************   System Settings    *************************
// **********************************************************************
// **********************************************************************

// **************************************************************
// constructor
// **************************************************************
SystemSettings::SystemSettings() :
  mPort1(SERIAL_PORT_1),
  mPort2(SERIAL_PORT_2),
  mPort3(SERIAL_PORT_3),
  mPort4(SERIAL_PORT_4)
{
}

// **************************************************************
// captures persistent system settings
// **************************************************************
void SystemSettings::copyPersistentSettings()
{
  mPort1.copyPersistentSettings(SERIAL_PORT_1);
  mPort2.copyPersistentSettings(SERIAL_PORT_2);
  mPort3.copyPersistentSettings(SERIAL_PORT_3);
  mPort4.copyPersistentSettings(SERIAL_PORT_4);
}

// **************************************************************
// captures current system settings
// **************************************************************
void SystemSettings::copyCurrentSettings(CommunicationCenter& communicationCenter)
{
  mPort1.copyCurrentSettings(communicationCenter, SERIAL_PORT_1);
  mPort2.copyCurrentSettings(communicationCenter, SERIAL_PORT_2);
  mPort3.copyCurrentSettings(communicationCenter, SERIAL_PORT_3);
  mPort4.copyCurrentSettings(communicationCenter, SERIAL_PORT_4);
}

// **************************************************************
// returns the relevant port settings
// **************************************************************
const PortSystemSettings& SystemSettings::getPortSystemSettings(SerialPortNum portNum) const
{
  switch (portNum) {
  case SERIAL_PORT_1:
    return mPort1;
  case SERIAL_PORT_2:
    return mPort2;
  case SERIAL_PORT_3:
    return mPort3;
  case SERIAL_PORT_4:
    return mPort4;
  default:
    break;
  }

  return mPort1;
}
*/
// **********************************************************************
// **********************************************************************
// *********************   Serial Stream Pattern    *********************
// **********************************************************************
// **********************************************************************

// **************************************************************
// raw data constructor
// **************************************************************
SerialStreamPattern::SerialStreamPattern(uint32_t data)
{
  *(uint32_t*)mData = data;
}

// **************************************************************
// constructor for hex ascii string
// **************************************************************
SerialStreamPattern::SerialStreamPattern(const char* string, char terminator)
{
  *(uint32_t*)mData = 0;

  int len = 0;
  if (string != NULL) {
    while (string[len] != terminator) {
      if (string[len] == 0) {
        // reached end of stream before the terminator - this is an error
        // note that we will not reach this code if 'terminator' == 0
        len = 0;
        break;
      }

      len++;
    }
  }

  // handle the case of odd length
  if ((len % 2) == 1) {
    len = 0;
  }
  len = len / 2;
  if (len > 3) {
    len = 0;
  }

  // at this point, if 'len' > 0, then 'len' is valid in the range {1, 2, 3}
  int i;
  for (i=0; i<len; i++) {
    int val = readHexOctat(string + (2 * i));
    if (val < 0) {
      len = 0;
      break;
    }
    mData[i] = val & 0xFF;
  }

  if (len == 0) {
    *(uint32_t*)mData = 0;
  }
  else {
    mData[3] = len;
  }
}

// **************************************************************
// returns the pattern length
// **************************************************************
int SerialStreamPattern::getPatternLen() const
{
  return mData[3];
}

// **************************************************************
// returns pointer to the pattern.
// NULL if data is invalid.
// **************************************************************
const unsigned char* SerialStreamPattern::getPattern() const
{
  if (mData[3] == 0) {
    return NULL;
  }
  return mData;
}

// **************************************************************
// constructor for hex ascii string
// **************************************************************
uint32_t SerialStreamPattern::toU32() const
{
  return *(uint32_t*)mData;
}

// **************************************************************
// Returns the raw data
// **************************************************************
SerialStreamPattern::SerialStreamPattern(const SerialStreamPattern& other)
{
  *(uint32_t*)mData = *(const uint32_t*)(other.mData);
}

// **************************************************************
// Reads a single hex octat (two chars)
// returns negative on error
// **************************************************************
int SerialStreamPattern::readHexOctat(const char* data)
{
  int retVal;
  int i;
  for (i=0; i<2; i++) {
    retVal = (retVal << 4);
    int ch = data[i];
    if ((ch >= '0') && (ch <= '9')) {
      retVal += (ch - '0');
    }
    else if ((ch >= 'A') && (ch <= 'F')) {
      retVal += (ch - 'A' + 10);
    }
    else if ((ch >= 'a') && (ch <= 'f')) {
      retVal += (ch - 'a' + 10);
    }
    else {
      return -1;
    }
  }

  return retVal;
}

// **************************************************************
// returns mask for pattern matching
// returns '0' if pattern is invalid
// **************************************************************
uint32_t SerialStreamPattern::getPatternMask() const
{
  int patternLen = getPatternLen();

  if (patternLen == 1) {
    return (uint32_t)0x000000FF;
  }
  if (patternLen == 2) {
    return (uint32_t)0x0000FFFF;
  }
  if (patternLen == 3) {
    return (uint32_t)0x00FFFFFF;
  }

  // default is irrelevant
  return (uint32_t)0x00000000;
}

// **************************************************************
// returns a value that will never match pattern
// We test against all data, even the irrelevant parts
// **************************************************************
uint32_t SerialStreamPattern::getUnmatchPattern() const
{
  int i;

  unsigned char unMatch = 0xFF;
  while (unMatch > 0) {
    bool bMatch = false;
    for (i=0; i<4; i++) {
      if (unMatch == mData[i]) {
        bMatch = true;
      }
    }

    // this character is not part of stream
    if (!bMatch) {
      break;
    }
    unMatch--;
  }

  uint32_t retVal = 0;
  for (i=0; i<4; i++) {
    retVal = retVal << 8;
    retVal += unMatch;
  }

  return retVal;
}

// **************************************************************
// returns the target pattern as represented in stream.
// the first byte is the msB
// **************************************************************
uint32_t SerialStreamPattern::getTargetPattern() const
{
  int patternLen = getPatternLen();
  uint32_t retVal = 0;
  int i;

  for (i=0; i < patternLen; i++) {
    retVal = retVal << 8;
    retVal += mData[i];
  }

  return retVal;
}