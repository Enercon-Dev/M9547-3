// **********************************************************************
// **********************************************************************
// Copyright 2011 by Avi Owshanko. All rights reserved.
// **********************************************************************
// **********************************************************************

#ifndef SYSTEM_OBJECTS_H
#define SYSTEM_OBJECTS_H

#include "../persistentData/persistentData.h"
#include "../communicationCenter/communicationCenter.h"
//#include "switchManager.h"

// *******************************************************************
// A class holding system objects (instead of a singleton)
// *******************************************************************
class SystemObjects
{
public:
  SystemObjects();

  CommunicationCenter mCommunicationCenter;
  PersistentData mPersistentData;
  //SwitchManager mSwitchManager;

  static SystemObjects& getSingleton()
      {return mSingleton;}

private:
  static SystemObjects mSingleton;
};

#endif // SYSTEM_OBJECTS_H