// **********************************************************************
// **********************************************************************
// Copyright 2011 by Avi Owshanko. All rights reserved.
// **********************************************************************
// **********************************************************************

#ifndef M359_MESSAGE_QUEUE_H
#define M359_MESSAGE_QUEUE_H

#include "globalInterface.h"
#include "ethernetGlobal.h"

#ifdef __cplusplus
 extern "C" {
#endif

// STM constants
#define ETHERNET_RX_FT_MASK          ((uint32_t)0x00000020)
#define ETHERNET_RX_LS_MASK          ((uint32_t)0x00000100)
#define ETHERNET_RX_FS_MASK          ((uint32_t)0x00000200)
#define ETHERNET_RX_LE_MASK          ((uint32_t)0x00001000)
#define ETHERNET_RX_DE_MASK          ((uint32_t)0x00004000)
#define ETHERNET_RX_ES_MASK          ((uint32_t)0x00008000)
#define ETHERNET_RX_FL_MASK          ((uint32_t)0x3FFF0000)

#define ETHERNET_TX_RCH_MASK         ((uint32_t)0x00004000)
#define ETHERNET_TX_TCH_MASK         ((uint32_t)0x00100000)
#define ETHERNET_TX_OWN_MASK         ((uint32_t)0x80000000)

#define ETHERNET_CRC_LEN (4)

// program twik params.
#define IN_BUFFER_COUNT (10)
#define OUT_BUFFER_COUNT (3)
#define BUFFER_COUNT (IN_BUFFER_COUNT+OUT_BUFFER_COUNT)
// longest message body is 1500, so we have some spares here...
#define ETHERNET_MESSAGE_BUFFER_SIZE (0x600)
#define MESSAGE_POOL_SIZE (BUFFER_COUNT * ETHERNET_MESSAGE_BUFFER_SIZE)
// padd the ethernet physical header to 16 bytes.
#define ETHERNET_HEADER_PAD (2)
#define ETHERNET_CRC_LEN (4)

#define ETH_DMATxDesc_OWN   ((uint32_t)0x80000000)
#define ETH_DMATxDesc_LS    ((uint32_t)0x20000000)
#define ETH_DMATxDesc_FS    ((uint32_t)0x10000000)
#define ETH_DMATxDesc_TCH   ((uint32_t)0x00100000)

typedef struct  {
  volatile uint32_t   Status;                /* Status */
  uint32_t   ControlBufferSize;     /* Control and Buffer1, Buffer2 lengths */
  uint32_t   Buffer1Addr;           /* Buffer1 address pointer */
  uint32_t   NextDescAddr;          /* next descriptor address pointer */
} EthernetDMAPacket;

// interface

void initPackets();

// returns next incoming packet. NULL if no message pending.
const struct netbuf* readInPacket();
// marks current packet as 'used'
int dequeueInPacket(const struct netbuf*);

// guess what the following functions does
int initializeTransmitBuffers();
int initializeReceiveBuffers();

// private functions
void initBuffer(unsigned int bufIndex,
                void* base,
                unsigned int tableSize,
                unsigned int bufferOffset,
                unsigned long sizeMask,
                unsigned long status);
void initInBuffer(unsigned int bufIndex, unsigned long statusMask);
void initOutBuffer(unsigned int bufIndex, unsigned long statusMask);
unsigned int getNextRxPacketIndex();
void freeRxPacket(unsigned int rxIndex);
int handleIncomingPacket(unsigned int rxIndex);

#ifdef __cplusplus
 }
#endif

#endif /* M359_MESSAGE_QUEUE_H */