// **********************************************************************
// **********************************************************************
// Copyright 2011 by Avi Owshanko. All rights reserved.
// **********************************************************************
// **********************************************************************

#include "globalInterface.h"
//#include "ethernetCommon.h"


void systemReset()
{
  systemResetNoStats();
}

// **************************************************************
// This is our error function.
// Enter live loop so we can catch on 
// **************************************************************
void enterLiveLoop()
{
#ifdef DEBUG
  while(1);
#else
  systemReset();
#endif  // DEBUG
}
/*
// **************************************************************
// translates 'SerialPortNum' to 'TCPPortTypes'
// **************************************************************
TCPPortTypes portNum2portType(SerialPortNum portNum)
{
  switch (portNum) {
  case SERIAL_PORT_1:
    return TCP_PORT_1;
  case SERIAL_PORT_2:
    return TCP_PORT_2;
  case SERIAL_PORT_3:
    return TCP_PORT_3;
  case SERIAL_PORT_4:
    return TCP_PORT_4;
  default:
    break;
  }

  return TCP_PORT_1;
}
*/