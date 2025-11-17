// **********************************************************************
// **********************************************************************
// Copyright 2011 by Avi Owshanko. All rights reserved.
// **********************************************************************
// **********************************************************************

#include "systemObjects.h"
//#include "ethernetCommon.h"
//#include "globalInterface.h"

SystemObjects SystemObjects::mSingleton;

// **************************************************************
// Constructor
// **************************************************************
SystemObjects::SystemObjects() :
  mCommunicationCenter(),
  mPersistentData()
{
}

// **************************************************************
// global interface function
// **************************************************************
uint16_t get16RandomNumber()
{
  SystemObjects& systemObjects = SystemObjects::getSingleton();
  return systemObjects.mPersistentData.getNext16RandomNumber();
}