#include "SPI.h"
#include "stm32Interface.h"

#define Timed(x) Timeout = 0xFFF; while (x) { if (Timeout-- == 0) goto errReturn;}
#define Delay(x) { uint32_t Timeout = x*75; while (Timeout--);}

SPI_DRV* SPI_DRV::spi_m_pInstance = NULL;

ErrorStatus SPI_DRV::GPIO_INIT(void)
{
        GPIO_InitTypeDef GPIO_InitStructure;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
        
        //Configurate PSI_SCK_Pinpin
        GPIO_InitStructure.GPIO_Pin = SCK_Pin;
        GPIO_Init(SPI_Port, &GPIO_InitStructure);
      
        //Configurate SPI_MOSI_Pin
        GPIO_InitStructure.GPIO_Pin = MOSI_Pin;
        GPIO_Init(SPI_Port, &GPIO_InitStructure); 
        
        //Configurate SPI_MISO_Pin
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
        GPIO_InitStructure.GPIO_Pin = MISO_Pin;
        GPIO_Init(SPI_Port, &GPIO_InitStructure);
        
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_InitStructure.GPIO_Pin = MSTRB_Pin;
        GPIO_SetBits(SPI_CS_Port,MSTRB_Pin);
        GPIO_Init(SPI_CS_Port, &GPIO_InitStructure);

        return ErrorStatus::SUCCESS;
        
}

ErrorStatus SPI_DRV:: Init(void)
{
    ErrorStatus status = SUCCESS;
    
    GPIO_INIT();
    
    SPI_InitTypeDef SPI_InitStruct;
    SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStruct.SPI_Mode = SPI_Mode_Master;  
    SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
    SPI_InitStruct.SPI_FirstBit =SPI_FirstBit_LSB;
    SPI_InitStruct.SPI_CRCPolynomial =0;



    /* Enable the SPI1 clock*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
    SPI_Init(SPI1,&SPI_InitStruct);
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE))
    	uint16_t tmp = SPI_I2S_ReceiveData(SPI1);
    SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Rx | SPI_I2S_DMAReq_Tx, ENABLE);
    GPIO_WriteBit(SPI_CS_Port,MSTRB_Pin, Bit_SET);
  
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    /* Enable the DMA_Rx*/
    DMA_InitTypeDef DMA_Rx_InitStruct;
    DMA_Rx_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&(SPI1->DR);
    DMA_Rx_InitStruct.DMA_MemoryBaseAddr = 0;
    DMA_Rx_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_Rx_InitStruct.DMA_BufferSize = 0;
    DMA_Rx_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_Rx_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_Rx_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_Rx_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_Rx_InitStruct.DMA_Mode = DMA_Mode_Normal;
    DMA_Rx_InitStruct.DMA_Priority = DMA_Priority_Low;
    DMA_Rx_InitStruct.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel2,&DMA_Rx_InitStruct);    

    /* Enable the DMA_Tx*/ 
    DMA_InitTypeDef DMA_Tx_InitStruct;
    DMA_Tx_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&(SPI1->DR);
    DMA_Tx_InitStruct.DMA_MemoryBaseAddr = 0;
    DMA_Tx_InitStruct.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_Tx_InitStruct.DMA_BufferSize = 0;
    DMA_Tx_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_Tx_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_Tx_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_Tx_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_Tx_InitStruct.DMA_Mode = DMA_Mode_Normal;
    DMA_Tx_InitStruct.DMA_Priority = DMA_Priority_Low;
    DMA_Tx_InitStruct.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel3,&DMA_Tx_InitStruct);
    
    IT_INIT();

    return((ErrorStatus)status);
}


ErrorStatus SPI_DRV::IT_INIT(void)
{
  
  DMA_ITConfig(DMA1_Channel2, DMA_IT_TC, FunctionalState::ENABLE);
  DMA_ITConfig(DMA1_Channel3, DMA_IT_TC, FunctionalState::ENABLE);
  NVIC_InitTypeDef  NVIC_InitStructure;
  // Configure and enable I2C event interrupt
  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //DONE: what this for?
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; //DONE: what this for?
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel3_IRQn;
  NVIC_Init(&NVIC_InitStructure);

  return(SUCCESS);
}

ErrorStatus SPI_DRV:: SPI_TransmitReceiveMessage(void* dataRcv, uint8_t RcvLen, void* dataTx, uint8_t TxLen)
{
  ErrorStatus status = SUCCESS;
  
  if (SPI_Busy() == SET)
    return ERROR;
  
  DMA_Cmd(DMA1_Channel2, FunctionalState::DISABLE);
  DMA_Cmd(DMA1_Channel3, FunctionalState::DISABLE);
  
  DMA1_Channel2->CMAR = (uint32_t)dataRcv;
  DMA1_Channel2->CNDTR = RcvLen;
  DMA1_Channel3->CMAR = (uint32_t)dataTx;
  DMA1_Channel3->CNDTR = TxLen;
  GPIO_ResetBits(SPI_CS_Port,MSTRB_Pin);
  SPI1->CR1 |= ((uint16_t)0x0040);
  DMA1_Channel2->CCR |= DMA_CCR1_EN;
  DMA1_Channel3->CCR |= DMA_CCR1_EN;
  
  return((ErrorStatus)status);
}


void DMA1_Channel2_IRQHandler(void)
{
  DMA_ClearITPendingBit(DMA1_IT_TC2);
  if (DMA1_Channel3->CNDTR)
    return;
  GPIO_SetBits(SPI_CS_Port,MSTRB_Pin);
  SPI1->CR1 &= ((uint16_t)0xFFBF);
  DMA_ClearITPendingBit(DMA1_IT_TC2);
}

void DMA1_Channel3_IRQHandler(void)
{
  DMA_ClearITPendingBit(DMA1_IT_TC3);
  if (DMA1_Channel2->CNDTR)
    return;
  GPIO_SetBits(SPI_CS_Port,MSTRB_Pin);
  SPI1->CR1 &= ((uint16_t)0xFFBF);
  DMA_ClearITPendingBit(DMA1_IT_TC3);
}
