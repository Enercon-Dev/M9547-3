#ifndef __SPI_H
#define __SPI_H

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "misc.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_spi.h"
#include "stm32f10x_dma.h"

#ifndef NULL
  #define NULL 0
#endif


#define SPI_Port        GPIOA
#define SPI_CS_Port     GPIOB
#define MSTRB_Pin       GPIO_Pin_0 
#define SCK_Pin         GPIO_Pin_5
#define MISO_Pin        GPIO_Pin_6
#define MOSI_Pin        GPIO_Pin_7

class SPI_DRV {
public:
  
  
  
  static SPI_DRV&	Get_Instance()
  {
    if (!spi_m_pInstance)   // Only allow one instance of class to be generated
      spi_m_pInstance = new SPI_DRV;
    return *spi_m_pInstance;
  }
  
  ErrorStatus Init(void);
  ErrorStatus SPI_TransmitReceiveMessage(void* dataRcv, uint8_t RcvLen, void* dataTx, uint8_t TxLen);
  inline BusyStatus SPI_Busy(){
    
    return (GPIO_ReadInputDataBit(SPI_CS_Port, MSTRB_Pin) == 0) ? SET : RESET;
  }
private:
  SPI_DRV(){
  };  							// Private so that it can  not be called
  SPI_DRV(SPI_DRV const&){};             	                // copy constructor is private
  SPI_DRV& operator=(SPI_DRV const&){return *this;}; 	// assignment operator is private
  static SPI_DRV* spi_m_pInstance;
  
  ErrorStatus GPIO_INIT(void);
  ErrorStatus DMA_INIT(void);
  ErrorStatus IT_INIT(void);
                      
};

extern "C" void DMA1_Channel2_IRQHandler(void);
extern "C" void DMA1_Channel3_IRQHandler(void);

#endif
