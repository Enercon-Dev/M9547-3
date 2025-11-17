// **********************************************************************
// **********************************************************************
// Copyright 2011 by Avi Owshanko. All rights reserved.
// **********************************************************************
// **********************************************************************

#include "genericClasses.h"
#include "stm32f10x_flash.h"
#include <string.h>

// **************************************************************
// Unlock flash
// **************************************************************
FlashUnlock::FlashUnlock()
{
  FLASH_Unlock();

  FLASH_Status flashstatus = FLASH_GetStatus();
  if (flashstatus == FLASH_ERROR_PG) { 
    FLASH_ClearFlag(FLASH_FLAG_PGERR);
  }
  if (flashstatus == FLASH_ERROR_WRP) {
    FLASH_ClearFlag(FLASH_FLAG_WRPRTERR);
  }
}

// **************************************************************
// Lock flash
// **************************************************************
FlashUnlock::~FlashUnlock()
{
  FLASH_Lock();
}
