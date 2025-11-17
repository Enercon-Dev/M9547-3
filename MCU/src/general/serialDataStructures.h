// **********************************************************************
// **********************************************************************
// Copyright 2011 by Avi Owshanko. All rights reserved.
// **********************************************************************
// **********************************************************************

#ifndef SERIAL_DATA_STRUCTURES_H
#define SERIAL_DATA_STRUCTURES_H

#include "globalTypes.h"

#define SERIAL_DEFUALT_TIMEOUT (10)
#define SERIAL_MIN_TIMEOUT (0x0001)
#define SERIAL_MAX_TIMEOUT (0xFFFF)

// predeclaration
class CommunicationCenter;

// *******************************************************************
// A class for holding a pattern.
// Pattern can be from 1 char to 3 chars.
// An invalid pattern holds no chars ('getPattern' will return NULL).
// *******************************************************************
class SerialStreamPattern
{
public:
  SerialStreamPattern(uint32_t data);                           // constructor for actual data.
  SerialStreamPattern(const char* string, char terminator);     // constructor for hex ascii string
  SerialStreamPattern(const SerialStreamPattern&);

  int getPatternLen() const;
  const unsigned char* getPattern() const;
  uint32_t toU32() const;

  // helpers for pattern matching
  uint32_t getPatternMask() const;
  uint32_t getUnmatchPattern() const;
  uint32_t getTargetPattern() const;

private:
  SerialStreamPattern() {}
  int readHexOctat(const char* data);

  unsigned char mData[4];
};

// *******************************************************************
// Serial stream settings
// the structure's constructor declared in 'persistentSerialSettings'
// *******************************************************************
/*class SerialPhysicalStreamSettings
{
public:
  SerialPhysicalStreamSettings();
  uint32_t getBaudRate() const;

  TransceiverType mType;
  TransceiverSpeed mSpeed;
  TransceiverDatabits mDatabits;
  TransceiverStopbits mStopbits;
  TransceiverParity mParity;
};

bool operator==(const SerialPhysicalStreamSettings& first, const SerialPhysicalStreamSettings& second);
*/
// *******************************************************************
// Serial stream settings
// the structure's constructor declared in 'persistentSerialSettings'
// *******************************************************************
class SerialBufferStreamSettings
{
public:
  SerialBufferStreamSettings();

  TransceiverFlushCriteria mFlushCriteria;
  uint16_t mTimeout;
  SerialStreamPattern mPattern;
};

bool operator==(const SerialBufferStreamSettings& first, const SerialBufferStreamSettings& second);

// *******************************************************************
// Ethernet port settings
// the structure's constructor declared in 'persistentSerialSettings'
// *******************************************************************
class EthernetStreamSettings
{
public:
  EthernetStreamSettings(SerialPortNum);
  static uint16_t getDefaultSocketAddr(SerialPortNum portNum);

  TransportType mTransportType;
  uint16_t mLocalSocket;                      // socket to listen to
  uint16_t mTargetSocket;                     // target socket for UDP messages.
  uint32_t mTargetIP;
};
/*
// *******************************************************************
// a single port full settings
// *******************************************************************
class PortSystemSettings
{
public:
  PortSystemSettings(SerialPortNum);
  void copyPersistentSettings(SerialPortNum);
  void copyCurrentSettings(CommunicationCenter&, SerialPortNum);

  //SerialPhysicalStreamSettings mPhysicalSettings;
  SerialBufferStreamSettings mBufferSettings;
  EthernetStreamSettings mEthernetSettings;
};

// *******************************************************************
// the whole system settings
// *******************************************************************
class SystemSettings
{
public:
  SystemSettings();
  void copyPersistentSettings();
  void copyCurrentSettings(CommunicationCenter&);

  const PortSystemSettings& getPortSystemSettings(SerialPortNum) const;

private:
  PortSystemSettings mPort1;
  PortSystemSettings mPort2;
  PortSystemSettings mPort3;
  PortSystemSettings mPort4;
};
*/
#endif // SERIAL_DATA_STRUCTURES_H