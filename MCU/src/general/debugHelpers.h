// **********************************************************************
// **********************************************************************
// Copyright 2011 by Avi Owshanko. All rights reserved.
// **********************************************************************
// **********************************************************************

#ifndef DEBUG_HELPERS_H
#define DEBUG_HELPERS_H

#ifdef DEBUG

#include <stdint.h>

// following numbers determin the incoming/outbound package drop.
// set value to '0' to disable
#define RX_PACKAGE_DROP (0)
#define TX_PACKAGE_DROP (0)
#define _5003_RX_PACKAGE_DROP (0)

#endif // DEBUG
#endif // DEBUG_HELPERS_H