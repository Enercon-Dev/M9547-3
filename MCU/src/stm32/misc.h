/**
  ******************************************************************************
  * @file    misc.h
  * @author  MCD Application Team
  * @version V3.1.0
  * @date    06/19/2009
  * @brief   This file contains all the functions prototypes for the miscellaneous
  *          firmware library functions (add-on to CMSIS functions).
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2009 STMicroelectronics</center></h2>
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MISC_H
#define __MISC_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

typedef struct
{
  uint8_t NVIC_IRQChannel;
  uint8_t NVIC_IRQChannelPreemptionPriority;
  uint8_t NVIC_IRQChannelSubPriority;
  FunctionalState NVIC_IRQChannelCmd;
} NVIC_InitTypeDef;

void NVIC_Init(NVIC_InitTypeDef* NVIC_InitStruct);

#ifdef __cplusplus
}
#endif

#endif /* __MISC_H */

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/

#ifndef __MISC_A_H
#define __MISC_A_H

#define UART_RXNEIE_ENABLE_MASK ((uint16_t)(0x0020))
#define UART_TXEIE_ENABLE_MASK ((uint16_t)(0x0080))
#define UART_RXNEIE_DISABLE_MASK (~UART_RXNEIE_ENABLE_MASK)
#define UART_TXEIE_DISABLE_MASK (~UART_TXEIE_ENABLE_MASK)

#define EnableRxneInterrupt(channel) (channel->CR1 |= UART_RXNEIE_ENABLE_MASK)
#define EnableTxeInterrupt(channel) (channel->CR1 |= UART_TXEIE_ENABLE_MASK)
#define DisableRxneInterrupt(channel) (channel->CR1 &= UART_RXNEIE_DISABLE_MASK)
#define DisableTxeInterrupt(channel) (channel->CR1 &= UART_TXEIE_DISABLE_MASK)






#endif // __MISC_A_H
