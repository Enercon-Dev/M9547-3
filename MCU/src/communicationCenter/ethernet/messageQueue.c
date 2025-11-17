// **********************************************************************
// **********************************************************************
// Copyright 2011 by Avi Owshanko. All rights reserved.
// **********************************************************************
// **********************************************************************

#include "messageQueue.h"
#include "stm32f10x.h"
#include "ethernetGlobal.h"
#include "debugHelpers.h"
#include <string.h>


// leave some extra 0x100 bytes for padding (just in case)
int x;
#ifdef __ICCARM__
__no_init static char messageBuffer[MESSAGE_POOL_SIZE + 0x100];
__no_init static EthernetDMAPacket inPacketBuffers[IN_BUFFER_COUNT];
__no_init static EthernetDMAPacket outPacketBuffers[OUT_BUFFER_COUNT];
#elif defined ( __GNUC__ )
 static char messageBuffer[MESSAGE_POOL_SIZE + 0x100];
 static EthernetDMAPacket inPacketBuffers[IN_BUFFER_COUNT];
 static EthernetDMAPacket outPacketBuffers[OUT_BUFFER_COUNT];
#endif

static unsigned int curInIndex = 0;
static unsigned int curOutIndex = 0;

// **************************************************************
// initiates a buffer info.
// **************************************************************
void incAndSetRxIndex(unsigned int rxIndex)
{
  initInBuffer(rxIndex, 0);
  rxIndex++;
  if (rxIndex >= IN_BUFFER_COUNT) {
    rxIndex= 0;
  }
  curInIndex = rxIndex;
}

// **************************************************************
// initiates a buffer info.
// **************************************************************
void initBuffer(unsigned int bufIndex,
                void* base,
                unsigned int tableSize,
                unsigned int bufferOffset,
                unsigned long sizeMask,
                unsigned long status)
{
  EthernetDMAPacket *thisBuffer = ((EthernetDMAPacket*)base)+bufIndex;
  unsigned int nextIndex = bufIndex+1;
  if (nextIndex >= tableSize) {
    nextIndex = 0;
  }
  EthernetDMAPacket *nextBuffer = ((EthernetDMAPacket*)base)+nextIndex;


  thisBuffer->ControlBufferSize = (ETHERNET_MESSAGE_BUFFER_SIZE-ETHERNET_HEADER_PAD) | sizeMask;
  thisBuffer->Buffer1Addr = (uint32_t)messageBuffer + (ETHERNET_MESSAGE_BUFFER_SIZE*(bufIndex+bufferOffset));
  thisBuffer->Buffer1Addr += ETHERNET_HEADER_PAD;   // use offset for alligning IP data...
  thisBuffer->NextDescAddr = (uint32_t)nextBuffer;

  // status must come last - as we might set the OWN bit.
  thisBuffer->Status = status;
}

// **************************************************************
// initiates an incoming buffer info.
// **************************************************************
void initInBuffer(unsigned int bufIndex, unsigned long statusMask)
{
  statusMask |= ETHERNET_TX_OWN_MASK;      // always return buffers to DMA.
  initBuffer(bufIndex,
             inPacketBuffers,
             IN_BUFFER_COUNT,   // entries count.
             0,                 // offset in 'buffer'.
             ETHERNET_TX_RCH_MASK, // size mask (chained list)
             statusMask);
}

// **************************************************************
// initiates a transmitting buffer info.
// **************************************************************
void initOutBuffer(unsigned int bufIndex, unsigned long statusMask)
{
  statusMask |= ETHERNET_TX_TCH_MASK;    // (chaned list)
  initBuffer(bufIndex,
             outPacketBuffers,
             OUT_BUFFER_COUNT,  // entries count.
             IN_BUFFER_COUNT,   // offset in 'buffer'.
             0,                 // size mask
             statusMask);
}


// **************************************************************
// looks for the first packet not owned by the DMA.
// starts at 'curInIndex'
// returns 'IN_BUFFER_COUNT' no message received.
// **************************************************************
unsigned int getNextRxPacketIndex()
{
  EthernetDMAPacket* curInBuffer = inPacketBuffers + curInIndex;
  uint32_t curBufStatus = curInBuffer->Status;
  if (!(curBufStatus & ETHERNET_TX_OWN_MASK)) {
    return curInIndex;
  }

  unsigned int index = curInIndex;
  unsigned int packetCounter = IN_BUFFER_COUNT;
  for (packetCounter = 0; packetCounter<IN_BUFFER_COUNT; packetCounter++)
  {
    index++;
    if (index >= IN_BUFFER_COUNT) {
      index = 0;
    }
    EthernetDMAPacket* thisInBuffer = inPacketBuffers + index;
    uint32_t thisBufStatus = thisInBuffer->Status;
    if (!(curBufStatus & ETHERNET_TX_OWN_MASK)) {
      break;
    }
  }

  if (packetCounter >= IN_BUFFER_COUNT) {
    // all packets are owned by the DMA
    return IN_BUFFER_COUNT;
  }

  // at this point we have some packet in list.
  // let the current packet have priority.
  curBufStatus = curInBuffer->Status;
  if (!(curBufStatus & ETHERNET_TX_OWN_MASK)) {
    // message received while we were looping around.
    return curInIndex;
  }

  // this is actually an error state...
  return index;
}

// **************************************************************
// goes over all the list.
// if found an inbound message, handle it.
// will handle only a single message.
// returns '0' if no message pending.
// **************************************************************
int handleIncomingPackets()
{
  unsigned int rxIndex = getNextRxPacketIndex();
  if (rxIndex >= IN_BUFFER_COUNT) {
    return 0;
  }

  int retVal = handleIncomingPacket(rxIndex);
  incAndSetRxIndex(rxIndex);

  return retVal;
}

// **************************************************************
// process message in 'rxIndex'.
// returns '1' if a single message was processed.
// returns negative on error.
// **************************************************************
int handleIncomingPacket(unsigned int rxIndex)
{
#ifdef DEBUG
  // error injection. will loose 1 package of every 'INCOMING_PACKAGE_DROP'
  uint16_t randomNum = get16RandomNumber();
  if ((RX_PACKAGE_DROP != 0) && ((randomNum % RX_PACKAGE_DROP) == 0)) {
    return 0;
  }
#endif // DEBUG

  EthernetDMAPacket* curInBuffer = inPacketBuffers + rxIndex;
  uint32_t curBufStatus = curInBuffer->Status;

  if (!(curBufStatus & ETHERNET_RX_FS_MASK) ||
      !(curBufStatus & ETHERNET_RX_LS_MASK)) {
    // we handle only single packets.
    return -21;
  }
  if ((curBufStatus & ETHERNET_RX_ES_MASK) ||
      (curBufStatus & ETHERNET_RX_DE_MASK)){
    // packets contains some error - ignore it...
    return -22;
  }

  unsigned int frameLen = (curBufStatus & ETHERNET_RX_FL_MASK) >> 16;
  if ((frameLen < ETHERNET_MIN_MESSAGE_SIZE) || (frameLen > ETHERNET_MAX_MESSAGE_SIZE)) {
    // packet too small/large.
    return -23;
  }

    if ((curBufStatus & ETHERNET_RX_LE_MASK) &&
        !(curBufStatus & ETHERNET_RX_FT_MASK)) {
    //"Length error"
    return -25;
  }

  const char* messageBase = messageBuffer +
                            (ETHERNET_MESSAGE_BUFFER_SIZE*rxIndex) +
                            ETHERNET_HEADER_PAD;
  // remove CRC trailer length.
  frameLen -= ETHERNET_CRC_LEN;
  return processEthernetMsg(messageBase, frameLen);
}


// **************************************************************
// initializes list of transmition buffers
// **************************************************************
int initializeTransmitBuffers()
{
  unsigned int i;
  for (i=0; i<OUT_BUFFER_COUNT; i++) {
    initOutBuffer(i, ETHERNET_TX_TCH_MASK);
  }
  //the actual data buffers address initialized to "0"
  curOutIndex = 0;
  ETH->DMATDLAR = (uint32_t) outPacketBuffers;

  return 0;
}

// **************************************************************
// initializes list of receiving buffers
// **************************************************************
int initializeReceiveBuffers()
{
  unsigned int i;
  for (i=0; i<IN_BUFFER_COUNT; i++) {
    initInBuffer(i, 0);
  }
  //the actual data buffers address initialized to "0"
  curInIndex = 0;
  ETH->DMARDLAR = (uint32_t) inPacketBuffers;

  return 0;
}

// **************************************************************
// a function for getting next outbound free buffer.
// If no buffer is free, wait at least 10 msec. If buffer is still not
// free, return NULL.
// **************************************************************
char* getFreeOutPacket()
{
  EthernetDMAPacket* curOutBuffer = outPacketBuffers + curOutIndex;
  uint32_t curBufStatus = curOutBuffer->Status;

  if (curBufStatus & ETH_DMATxDesc_OWN) {
    return NULL;
  }

  char* base = messageBuffer + (ETHERNET_MESSAGE_BUFFER_SIZE*(curOutIndex+IN_BUFFER_COUNT));
  base += ETHERNET_HEADER_PAD;   // use offset for alligning IP data...
  return base;
}

// **************************************************************
// Notifies the DMA that we have a new ethernet package to send.
// **************************************************************
static void ethernetSendPacket()
{

  // When Tx Buffer unavailable flag is set: clear it and resume transmission
  if ((ETH->DMASR & ETH_DMASR_TBUS) != (uint32_t)RESET)
  {
    // Clear TBUS ETHERNET DMA flag
    ETH->DMASR = ETH_DMASR_TBUS;
  }

  // Resume DMA transmission
  ETH->DMATPDR = 0;
}

// **************************************************************
// a function for sending out current buffer.
// **************************************************************
int sendCurOutPacket(char* base, uint32_t frameLen)
{
  EthernetDMAPacket* curOutBuffer = outPacketBuffers + curOutIndex;
  uint32_t curBufStatus = curOutBuffer->Status;

  unsigned int nextOutIndex = curOutIndex+1;
  if (nextOutIndex >= OUT_BUFFER_COUNT) {
    nextOutIndex = 0;
  }

  // prepare transmition buffer.
  curOutBuffer->NextDescAddr = (uint32_t)(outPacketBuffers + nextOutIndex);
  curOutBuffer->ControlBufferSize = frameLen;
  curOutBuffer->Buffer1Addr = (uint32_t)base;
  curOutBuffer->Status = (ETH_DMATxDesc_OWN |
                          ETH_DMATxDesc_LS |
                          ETH_DMATxDesc_FS |
                          ETH_DMATxDesc_TCH);

  // update the transmition buffer index
  curOutIndex = nextOutIndex;
  // give the transmitter a hint, just in case it fell asleep...
  ethernetSendPacket();

  return 0;
}
