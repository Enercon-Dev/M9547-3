// **********************************************************************
// **********************************************************************
// Copyright 2011 by Avi Owshanko. All rights reserved.
// **********************************************************************
// **********************************************************************

#include "mac.h"
#include "globalInterface.h"
#include "ethernetGlobal.h"
#include "stm32f10x.h"
#include "systemManagement.h"
#include "../../persistentData/persistentDataCenter.h"

#include <string.h>

// **********************************************************************
// **********************************************************************
// *************************   MAC Address     **************************
// **********************************************************************
// **********************************************************************

// **************************************************************
// Constructor
// **************************************************************
MACAddr::MACAddr() : mData(0)
{
}

// **************************************************************
// Constructor
// **************************************************************
MACAddr::MACAddr(const MACAddr& val) : mData(val.mData)
{
}

// **************************************************************
// Constructor
// **************************************************************
MACAddr::MACAddr(const char *addr) : mData(0)
{
  if (addr != NULL) {
    memcpy (&mData, addr, 6);
    ((char*)&mData)[7] = 1;
  }
}

// **************************************************************
// tests is address is all '0'
// **************************************************************
bool MACAddr::isEmpty() const
{
  const char* base = (const char*)&mData;
  int i;
  for (i=0; i<6; i++) {
    if (base[i] != 0) {
      return false;
    }
  }

  return true;
}

// **************************************************************
// Compare operator
// **************************************************************
bool operator==(const MACAddr& first, const MACAddr& second)
{
  return first.equal(second);
}

// **************************************************************
// Compare operator
// **************************************************************
bool operator!=(const MACAddr& first, const MACAddr& second)
{
  return !(first.equal(second));
}

// **************************************************************
// Global function for all the 'C' sections
// **************************************************************
const char* getMacAddr()
{
  const MACAddr& myAddr = SystemManagement::getPersistentDataCenter().getMacAddr();
  return myAddr.getAddr();
}

// **********************************************************************
// **********************************************************************
// **************************   IP Address     **************************
// **********************************************************************
// **********************************************************************

// **************************************************************
// Constructor
// **************************************************************
IPData::IPData() :
  mAddr(IP_DEFAULT_ADDR),
  mMask(IP_DEFAULT_MASK),
  mGateway(IP_DEFAULT_GATEWAY)
{
}

// **************************************************************
// Set object data
// **************************************************************
void IPData::setData(uint32_t addr, uint32_t mask, uint32_t gateway)
{
  mAddr = addr;
  mMask = mask;
  mGateway = gateway;
}

// **************************************************************
// A method for deciding whether 'ipAddr' is in my subnet
// XXX - currently disabled
// **************************************************************
bool IPData::isSameSubnet(uint32_t ipAddr) const
{
  return true;
 //return ((ipAddr & mMask) == (mAddr & mMask));
}

// **************************************************************
// A method for deciding whether 'dest' is in my subnet, or that 
// we need to go through a gateway.
// XXX - currently disabled
// **************************************************************
uint32_t IPData::getFirsthop(uint32_t dest) const
{
  if ((dest == 0) || (dest == 0xFFFFFFFF)){
    return dest;
  }

  return dest;
}

// **************************************************************
// A method for deciding whether 'ipAddr' is in my gateway
// **************************************************************
bool IPData::isGateway(uint32_t ipAddr) const
{
  if (mGateway == 0) {
    return false;
  }
  return (ipAddr == mGateway);
}

// **************************************************************
// Compare operator
// **************************************************************
bool IPData::equal(const IPData& other) const
{
  if (mAddr != other.mAddr) {
    return false;
  }
  if (mMask != other.mMask) {
    return false;
  }
  if (mGateway != other.mGateway) {
    return false;
  }

  return true;  
}

// **************************************************************
// Compare operator
// **************************************************************
bool operator==(const IPData& first, const IPData& second)
{
  return first.equal(second);
}