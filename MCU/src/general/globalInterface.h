// **********************************************************************
// **********************************************************************
// Copyright 2011 by Avi Owshanko. All rights reserved.
// **********************************************************************
// **********************************************************************

#ifndef GLOBAL_INTERFACE_H
#define GLOBAL_INTERFACE_H

#include <stdint.h>

#ifdef __cplusplus
 extern "C" {
#endif

void enterLiveLoop();
void systemResetNoStats();
void systemReset();
uint16_t calcCRC(const unsigned char* buffer, uint16_t buf_length);
const char* getMacAddr();

// ****************************************************************************
// ethernet interface
// ****************************************************************************
int ethernetMACConfig();
void sendArpGratuatiousMsg();

// checks incoming queue
int handleIncomingPackets();
uint16_t calcChecksum(const char* base, unsigned int length,
                      uint32_t toAdd, uint32_t toSub);

// ****************************************************************************
// miscl
// ****************************************************************************
uint16_t get16RandomNumber();

// ****************************************************************************
// serial UART interrupt handlers
// ****************************************************************************
void USART1_IRQHandler();
void USART2_IRQHandler();
void UART4_IRQHandler();
void UART5_IRQHandler();

#ifdef __cplusplus
 }
#endif 

#endif /* GLOBAL_INTERFACE_H */