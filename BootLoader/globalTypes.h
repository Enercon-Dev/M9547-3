// **********************************************************************
// **********************************************************************
// Copyright 2011 by Avi Owshanko. All rights reserved.
// **********************************************************************
// **********************************************************************

#ifndef GLOBAL_TYPES_H
#define GLOBAL_TYPES_H

#include <stdint.h>

typedef enum {DISABLE = 0, ENABLE = !DISABLE} FunctionalState;
typedef enum {RESET = 0, SET = !RESET} FlagStatus;

#ifdef __cplusplus
#define     __I     volatile
#else
#define     __I     volatile const
#endif
#define     __O     volatile
#define     __IO    volatile

#define htons(val) (((val&0xFF) << 8) | ((val & 0xFF00) >> 8))
#define ntohs(val) (((val&0xFF) << 8) | ((val & 0xFF00) >> 8))

#define htonl(val)                                              \
  (((val & 0x000000FF) << 24) | ((val & 0x0000FF00) <<  8) |    \
   ((val & 0x00FF0000) >>  8) | ((val & 0xFF000000) >> 24))

#define ntohl(val)                                              \
  (((val & 0x000000FF) << 24) | ((val & 0x0000FF00) <<  8) |    \
   ((val & 0x00FF0000) >>  8) | ((val & 0xFF000000) >> 24))

// read from host memory (little ENDIAN)
#define hGetU16(base, offset)                                   \
  (*(const uint16_t*)(((const char*)base) + offset))

#define hGetU32(base, offset)                                   \
  (*(const uint32_t*)(((const char*)base) + offset))

#define SHIFT_32_BIT_LEFT(loc) (((uint32_t)(0x01)) << loc)
#define SHIFT_32_BIT_RIGHT(loc) (((uint32_t)(0x80000000)) >> loc)

#define RETURN_ON_NEGATIVE0(type, func)             \
  {                                                 \
    type retVal = func();                           \
    if (retVal < 0) {                               \
      return retVal;                                \
    }                                               \
  }

#define RETURN_ON_NEGATIVE1(type, func, param1)     \
  {                                                 \
    type retVal = func((param1));                   \
    if (retVal < 0) {                               \
      return retVal;                                \
    }                                               \
  }

// **************************************************************
// Port types
// **************************************************************
typedef enum {
  SERIAL_PORT_1,
  SERIAL_PORT_2,
  SERIAL_PORT_3,
  SERIAL_PORT_4,
  SERIAL_PORT_INVALID
} SerialPortNum;

// *******************************************************************
// *******************************************************************
typedef enum {
  TRANSPORT_TYPE_TCP,
  TRANSPORT_TYPE_UDP,
  TRANSPORT_TYPE_DISABLED
} TransportType;

// *******************************************************************
// *******************************************************************
typedef enum {
  TRANSCEIVER_TYPE_RS232,
  TRANSCEIVER_TYPE_RS422,
  TRANSCEIVER_TYPE_DISABLED
} TransceiverType;

// *******************************************************************
// *******************************************************************
typedef enum {
  TRANSCEIVER_SPEED_1200,
  TRANSCEIVER_SPEED_2400,
  TRANSCEIVER_SPEED_4800,
  TRANSCEIVER_SPEED_9600,
  TRANSCEIVER_SPEED_19200,
  TRANSCEIVER_SPEED_38400,
  TRANSCEIVER_SPEED_57600,
  TRANSCEIVER_SPEED_115200,
  TRANSCEIVER_SPEED_230400
} TransceiverSpeed;

// *******************************************************************
// *******************************************************************
typedef enum {
  TRANSCEIVER_DATA_7,
  TRANSCEIVER_DATA_8
} TransceiverDatabits;

// *******************************************************************
// *******************************************************************
typedef enum {
  TRANSCEIVER_STOPBITS_1,
  TRANSCEIVER_STOPBITS_2
} TransceiverStopbits;

// *******************************************************************
// *******************************************************************
typedef enum {
  TRANSCEIVER_PARITY_NONE,
  TRANSCEIVER_PARITY_ODD,
  TRANSCEIVER_PARITY_EVEN
} TransceiverParity;

// *******************************************************************
// *******************************************************************
typedef enum {
  TRANSCEIVER_FLUSH_NONE,
  TRANSCEIVER_FLUSH_HEADER,
  TRANSCEIVER_FLUSH_TRAILER,
  TRANSCEIVER_FLUSH_TIMEOUT
} TransceiverFlushCriteria;

// *******************************************************************
// *******************************************************************
typedef enum {
  PORT_STATUS_ESTABLISHED,
  PORT_STATUS_LISTENING,
  PORT_STATUS_CLOSING,
  PORT_STATUS_CLOSED
  
} PortStatus;

// *******************************************************************
// List of all physical connection types
// *******************************************************************
typedef enum
{
  SWITCH_CONNECTION_AUTO_NEG,
  SWITCH_CONNECTION_100_FULL,
  SWITCH_CONNECTION_100_HALF,
  SWITCH_CONNECTION_10_FULL,
  SWITCH_CONNECTION_10_HALF,
  SWITCH_CONNECTION_DISCONNECTED,
  SWITCH_CONNECTION_UNKNOWN
} SwitchPortConnectionType;


// *******************************************************************
// List of all switch ports
// *******************************************************************
#define SWITCH_PORT_COUNT (4)
typedef enum
{
  SWITCH_PORT_0,
  SWITCH_PORT_1,
  SWITCH_PORT_2,
  SWITCH_PORT_3
} SwitchPortTypes;

#endif // GLOBAL_TYPES_H