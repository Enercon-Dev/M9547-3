// **********************************************************************
// **********************************************************************
// Copyright 2011 by Avi Owshanko. All rights reserved.
// **********************************************************************
// **********************************************************************

#ifndef MAC_H
#define MAC_H

#include <stdint.h>

#define IP_DEFAULT_ADDR    (0x00000000)
#define IP_DEFAULT_MASK    (0x00FFFFFF)
#define IP_DEFAULT_GATEWAY (0x00000000)

// *******************************************************************
// Class for holding the MAC address
// *******************************************************************
class MACAddr
{
public:
  MACAddr();
  MACAddr(const MACAddr& val);
  MACAddr(const char *addr);

  const char* getAddr() const {return ((const char*)&mData);}
  bool isValid() const {return (((const char*)&mData)[7] != 0);}
  bool equal(const MACAddr& other) const {return (mData == other.mData);}
  bool isEmpty() const;

private:
  uint64_t mData;
};

bool operator==(const MACAddr& first, const MACAddr& second);
bool operator!=(const MACAddr& first, const MACAddr& second);

// *******************************************************************
// Class for holding the IP definitions of this machine
// *******************************************************************
class IPData
{
public:
  IPData();
  bool isSameSubnet(uint32_t ipAddr) const;
  uint32_t getFirsthop(uint32_t dest) const;
  bool isGateway(uint32_t ipAddr) const;

  void setData(uint32_t addr, uint32_t mask, uint32_t gateway);
  bool equal(const IPData& other) const;
  
  uint32_t mAddr;
  uint32_t mMask;
  uint32_t mGateway;
};

bool operator==(const IPData& first, const IPData& second);

#endif // MAC_H