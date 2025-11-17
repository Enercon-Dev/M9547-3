// **********************************************************************
// **********************************************************************
// Copyright 2011 by Avi Owshanko. All rights reserved.
// **********************************************************************
// **********************************************************************

#ifndef ETHERNET_COMMON_H
#define ETHERNET_COMMON_H

#include "globalTypes.h"

// *******************************************************************
// List of all connection valid ports
// *******************************************************************
typedef enum
{
  UDP_PORT_DHCP,
  UDP_PORT_FTP,
  UDP_PORT_FTP_FW,
  UDP_PORT_COMMANDS,
  UDP_PORT_NEIGHB
} TCPPortTypes;

// *******************************************************************
// List of all ethernet connection types
// *******************************************************************
typedef enum {
  IP_INVALID_TYPE,
  IP_TCP_TYPE,
  IP_UDP_TYPE,
  IP_ICMP_TYPE
} IPFrameType;

// *******************************************************************
// Map to the actual socket ID
// *******************************************************************
#define SOCKET_DHCP       (0x4400)
#define SOCKET_5000       (0x8813)
#define SOCKET_5001       (0x8913)
#define SOCKET_5002       (0x8A13)
#define SOCKET_5003       (0x8B13)

// *******************************************************************
// GENERAL
// *******************************************************************
#define PORT_TYPES_COUNT (6)

#define BAD_IP_ADDR (0x000000FF)

// *******************************************************************
// general functions - keep them in c style.
// *******************************************************************

#ifdef __cplusplus
 extern "C" {
#endif

TCPPortTypes portNum2portType(SerialPortNum);

#ifdef __cplusplus
 }
#endif 

#endif // ETHERNET_COMMON_H