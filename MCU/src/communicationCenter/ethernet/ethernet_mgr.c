// **********************************************************************
// **********************************************************************
// Copyright 2011 by Avi Owshanko. All rights reserved.
// **********************************************************************
// **********************************************************************

#include "stm32f10x.h"
#include "messageQueue.h"
#include "globalInterface.h"
#include "globalTypes.h"

#define MACMIIAR_CR_Mask    ((uint32_t)0xFFFFFFE3)
#define MACCR_CLEAR_Mask    ((uint32_t)0xFF20810F)
#define MACFCR_CLEAR_Mask   ((uint32_t)0x0000FF41)
#define DMAOMR_CLEAR_Mask   ((uint32_t)0xF8DE3F23)

#define ETH_Watchdog_Disable      ((uint32_t)0x00800000)
#define ETH_Jabber_Disable   ((uint32_t)0x00400000)
#define ETH_JumboFrame_Disable    ((uint32_t)0x00000000)
#define ETH_InterFrameGap_64Bit   ((uint32_t)0x00080000)
#define ETH_CarrierSense_Enable   ((uint32_t)0x00000000)
#define ETH_Speed_100M       ((uint32_t)0x00004000)
#define ETH_ReceiveOwn_Disable    ((uint32_t)0x00002000)
#define ETH_LoopbackMode_Disable       ((uint32_t)0x00000000)
#define ETH_Mode_FullDuplex       ((uint32_t)0x00000800)
#define ETH_ChecksumOffload_Disable    ((uint32_t)0x00000000)
#define ETH_RetryTransmission_Disable  ((uint32_t)0x00000200)
#define ETH_AutomaticPadCRCStrip_Disable    ((uint32_t)0x00000000)
#define ETH_BackOffLimit_10  ((uint32_t)0x00000000)
#define ETH_DeferralCheck_Disable      ((uint32_t)0x00000000)

#define ETH_ReceiveAll_Disable    ((uint32_t)0x00000000)
#define ETH_SourceAddrFilter_Disable             ((uint32_t)0x00000000)
#define ETH_PassControlFrames_ForwardPassedAddrFilter ((uint32_t)0x000000C0)
#define ETH_BroadcastFramesReception_Enable      ((uint32_t)0x00000000)
#define ETH_DestinationAddrFilter_Normal    ((uint32_t)0x00000000)
#define ETH_PromiscuousMode_Disable    ((uint32_t)0x00000000)
#define ETH_MulticastFramesFilter_Perfect             ((uint32_t)0x00000000)
#define ETH_UnicastFramesFilter_Perfect          ((uint32_t)0x00000000)

#define ETH_ZeroQuantaPause_Disable    ((uint32_t)0x00000080)
#define ETH_PauseLowThreshold_Minus4        ((uint32_t)0x00000000)
#define ETH_UnicastPauseFrameDetect_Disable ((uint32_t)0x00000000)
#define ETH_ReceiveFlowControl_Disable      ((uint32_t)0x00000000)
#define ETH_TransmitFlowControl_Disable     ((uint32_t)0x00000000)

#define ETH_VLANTagComparison_16Bit    ((uint32_t)0x00000000)

#define ETH_DropTCPIPChecksumErrorFrame_Disable  ((uint32_t)0x04000000)
#define ETH_ReceiveStoreForward_Enable      ((uint32_t)0x02000000)
#define ETH_FlushReceivedFrame_Enable       ((uint32_t)0x00000000)
#define ETH_TransmitStoreForward_Enable     ((uint32_t)0x00200000)
#define ETH_TransmitThresholdControl_64Bytes     ((uint32_t)0x00000000)
#define ETH_ForwardErrorFrames_Disable      ((uint32_t)0x00000000)
#define ETH_ForwardUndersizedGoodFrames_Disable  ((uint32_t)0x00000000)
#define ETH_ReceiveThresholdControl_64Bytes      ((uint32_t)0x00000000)
#define ETH_SecondFrameOperate_Enable       ((uint32_t)0x00000004)

#define ETH_AddressAlignedBeats_Enable      ((uint32_t)0x02000000)
#define ETH_FixedBurst_Enable     ((uint32_t)0x00010000)
#define ETH_RxDMABurstLength_32Beat    ((uint32_t)0x00400000)
#define ETH_TxDMABurstLength_32Beat    ((uint32_t)0x00002000)
#define ETH_DMAArbitration_RoundRobin_RxTx_1_1   ((uint32_t)0x00000000)

#define ETH_MAC_AddrHighBase   (ETH_MAC_BASE + 0x40)
#define ETH_MAC_AddrLowBase    (ETH_MAC_BASE + 0x44)

#define EXTI_Line19      ((uint32_t)0x80000)

// **************************************************************
// **************************************************************
void ethernetConfigMIIAR()
{
  uint32_t tmpReg;

  tmpReg = ETH->MACMIIAR & MACMIIAR_CR_Mask;
  tmpReg |= ETH_MACMIIAR_CR_Div42;
  ETH->MACMIIAR = tmpReg;
}

// **************************************************************
// **************************************************************
void ethernetConfigCR()
{
  uint32_t tmpReg = ETH->MACCR & MACCR_CLEAR_Mask;

  tmpReg |= ETH_Watchdog_Disable;
  tmpReg |= ETH_Jabber_Disable;
  tmpReg |= ETH_JumboFrame_Disable;
  tmpReg |= ETH_InterFrameGap_64Bit;
  tmpReg |= ETH_CarrierSense_Enable;
  tmpReg |= ETH_Speed_100M;
  tmpReg |= ETH_ReceiveOwn_Disable;
  tmpReg |= ETH_LoopbackMode_Disable;
  tmpReg |= ETH_Mode_FullDuplex;
  tmpReg |= ETH_ChecksumOffload_Disable;
  tmpReg |= ETH_RetryTransmission_Disable;
  tmpReg |= ETH_AutomaticPadCRCStrip_Disable;
  tmpReg |= ETH_BackOffLimit_10;
  tmpReg |= ETH_DeferralCheck_Disable;

  ETH->MACCR = (uint32_t)tmpReg;
}

// **************************************************************
// **************************************************************
void ethernetConfigFFR()
{
  uint32_t tmpReg = ETH_ReceiveAll_Disable |
                    ETH_SourceAddrFilter_Disable |
                    ETH_PassControlFrames_ForwardPassedAddrFilter |
                    ETH_BroadcastFramesReception_Enable |
                    ETH_DestinationAddrFilter_Normal |
                    ETH_PromiscuousMode_Disable |
                    ETH_MulticastFramesFilter_Perfect |
                    ETH_UnicastFramesFilter_Perfect;

  ETH->MACFFR = tmpReg;
}

// **************************************************************
// **************************************************************
void ethernetConfigHTR()
{
  ETH->MACHTHR = 0;
  ETH->MACHTLR = 0;
}

// **************************************************************
// **************************************************************
void ethernetConfigFCR()
{
  uint32_t tmpReg = ETH->MACFCR & MACFCR_CLEAR_Mask;

  tmpReg |= 0;          // This one for pause time
  tmpReg |= ETH_ZeroQuantaPause_Disable;
  tmpReg |= ETH_PauseLowThreshold_Minus4;
  tmpReg |= ETH_UnicastPauseFrameDetect_Disable;
  tmpReg |= ETH_ReceiveFlowControl_Disable;
  tmpReg |= ETH_TransmitFlowControl_Disable;
  
  ETH->MACFCR = tmpReg;
}

// **************************************************************
// **************************************************************
static void ethernetConfigVLANTR()
{
  uint32_t tmpReg = ETH_VLANTagComparison_16Bit;
  tmpReg |= 0;      // VLAN tag identifier

  ETH->MACVLANTR = tmpReg;
}

// **************************************************************
// **************************************************************
static void ethernetConfigDMA0MR()
{
  uint32_t tmpReg = ETH->DMAOMR & DMAOMR_CLEAR_Mask;

  tmpReg |= ETH_DropTCPIPChecksumErrorFrame_Disable;
  tmpReg |= ETH_ReceiveStoreForward_Enable;
  tmpReg |= ETH_FlushReceivedFrame_Enable;
  tmpReg |= ETH_TransmitStoreForward_Enable;
  tmpReg |= ETH_TransmitThresholdControl_64Bytes;
  tmpReg |= ETH_ForwardErrorFrames_Disable;
  tmpReg |= ETH_ForwardUndersizedGoodFrames_Disable;
  tmpReg |= ETH_ReceiveThresholdControl_64Bytes;
  tmpReg |= ETH_SecondFrameOperate_Enable;          // XXX - need to verify this one.

  ETH->DMAOMR = tmpReg;
}

// **************************************************************
// **************************************************************
static void ethernetConfigDMABMR()
{
  uint32_t tmpReg = ETH_AddressAlignedBeats_Enable |
                    ETH_FixedBurst_Enable |             // XXX - why override?
                    ETH_RxDMABurstLength_32Beat |
                    ETH_TxDMABurstLength_32Beat |
                    0 |                                 // description skip length
                    ETH_DMAArbitration_RoundRobin_RxTx_1_1 |
                    ETH_DMABMR_USP;
                      
  ETH->DMABMR = tmpReg;
}

// **************************************************************
// **************************************************************
static void ethernetConfig()
{
  ethernetConfigMIIAR();
  ethernetConfigCR();
  ethernetConfigFFR();
  ethernetConfigHTR();
  ethernetConfigFCR();
  ethernetConfigVLANTR();
  ethernetConfigDMA0MR();
  ethernetConfigDMABMR();
}

// **************************************************************
// **************************************************************
static void ETH_Start()
{
  ETH->MACCR |= ETH_MACCR_TE;
  ETH->DMAOMR |= ETH_DMAOMR_FTF;
  ETH->MACCR |= ETH_MACCR_RE;
  ETH->DMAOMR |= ETH_DMAOMR_ST;
  ETH->DMAOMR |= ETH_DMAOMR_SR;
}

// **************************************************************
// Configures the ethernet layer.
// **************************************************************
int ethernetMACConfig()
{
  ethernetConfigMAC();
  ethernetConfig();
  
  /* Initialize Tx Descriptors list: Chain Mode */
  initializeTransmitBuffers();
  initializeReceiveBuffers();

  /* Enable MAC and DMA transmission and reception */
  ETH_Start();
  ETH->DMASR = ETH_DMASR_TBUS;
  

  return 0; 
}

// **************************************************************
// **************************************************************
void ETH_WKUP_IRQHandler(void)
{
  EXTI->PR = EXTI_Line19;
}

/*******************************************************************************
* Function Name  : ETH_DMAReceptionCmd
* Desciption     : Enables or disables the DMA reception.
* Input          : - NewState: new state of the DMA reception.
*                    This parameter can be: ENABLE or DISABLE.
* Output         : None
* Return         : None
*******************************************************************************/
void ETH_DMAReceptionCmd(FunctionalState NewState)
{
  if (NewState != DISABLE)
  {
    /* Enable the DMA reception */
    ETH->DMAOMR |= ETH_DMAOMR_SR;
  }
  else
  {
    /* Disable the DMA reception */
    ETH->DMAOMR &= ~ETH_DMAOMR_SR;
  }
}

// **************************************************************
// Configures the ethernet layer.
// **************************************************************
void ethernetConfigMAC()
{
  const char* macAddr = getMacAddr();
  uint32_t tmpReg;

  /* Calculate the selectecd MAC address high register */	
  tmpReg = (((uint32_t)(unsigned char)macAddr[5]) << 8) |
           ((uint32_t)(unsigned char)macAddr[4]);

  /* Load the selectecd MAC address high register */	
  *(__IO uint32_t *)ETH_MAC_AddrHighBase = tmpReg;

  /* Calculate the selectecd MAC address low register */	
  tmpReg = (((uint32_t)(unsigned char)macAddr[3]) << 24) |
           (((uint32_t)(unsigned char)macAddr[2]) << 16) |
           (((uint32_t)(unsigned char)macAddr[1]) << 8) |
           ((uint32_t)(unsigned char)macAddr[0]);

  /* Load the selectecd MAC address low register */
  *(__IO uint32_t *) ETH_MAC_AddrLowBase = tmpReg;
}

// **************************************************************
// Calculate IP/TCP/UDP checksum
// **************************************************************
uint16_t calcChecksum(const char* base, unsigned int length,
                      uint32_t toAdd, uint32_t toSub)
{
  const uint16_t* base16 = (const uint16_t*)base;

  uint32_t result = toAdd;

  if (length & 0x01) {
    uint16_t last = (unsigned char)(base[length-1]);
    result += ntohs(last);
  }
  length = length/2;

  int i;
  for (i=0; i<(length); i++) {
    uint32_t thisVal = ntohs(base16[i]);
    result += thisVal;
  }
  result -= toSub;

  // add up the carriers
  while (result>>16) {
    result = (result & 0xFFFF)+(result >> 16);
  }

  uint16_t retVal = htons(result);
  // verify that checksum is never '0'
  if (retVal == 0x0000) {
    // This is '+0' in 1s complement. Never returns '-0' according to RFC1624.
    return 0x0000;
  }
  return ~retVal;
}
