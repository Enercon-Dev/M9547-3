// **********************************************************************
// **********************************************************************
// Copyright 2011 by Avi Owshanko. All rights reserved.
// **********************************************************************
// **********************************************************************

// Following file is used for reserving space for the persistent data.
// It can be done more cleanly if we edit the 'stm32f10x_flash.icf' file,
// but it's safer this way.

#include "persistentCommon.h"

//#pragma location=INT_VEC_TOP
//__no_init const char boolLoader[BOOT_LOADER_TOP - INT_VEC_TOP];
#ifdef __GNUC__
extern const char MagicNum[20] __attribute__((section(".magic_const"))) = PERSISTENT_MAGIC_NUM ;
#elif defined ( __ICCARM__ )
#pragma location=MAGIC_NUM_LOCATION
__root const char MagicNum[] = PERSISTENT_MAGIC_NUM;
#endif


