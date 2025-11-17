// **********************************************************************
// **********************************************************************
// Copyright 2011 by Avi Owshanko. All rights reserved.
// **********************************************************************
// **********************************************************************

#ifndef GENERIC_CLASSES_STRUCTURES_H
#define GENERIC_CLASSES_STRUCTURES_H

// relevant only in C++ env.
#ifdef __cplusplus

// *******************************************************************
// A class for locking / unlocking flash
// *******************************************************************
class FlashUnlock
{
public:
  FlashUnlock();
  ~FlashUnlock();
};


#endif // __cplusplus
#endif // GENERIC_CLASSES_STRUCTURES_H