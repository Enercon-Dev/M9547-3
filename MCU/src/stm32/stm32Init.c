#include "stm32Interface.h"
#include "globalInterface.h"
#include "stm32f10x_flash.h"
#include "core_cm3.h"
#include "stm32f10x_gpio.h"
//#include "misc.h"
//#include "stm32f10x_usart.h"

#include "rccParams.h"

// **************************************************************
// Resets all system clocks to default.
// **************************************************************
static void RCC_reset()
{
  RCC->CR |= (uint32_t)0x00000001;
  RCC->CFGR &= (uint32_t)0xF0FF0000;
  RCC->CR &= (uint32_t)0xFEF6FFFF;
  RCC->CR &= (uint32_t)0xFFFBFFFF;
  RCC->CFGR &= (uint32_t)0xFF80FFFF;
  RCC->CR &= (uint32_t)0xEBFFFFFF;
  RCC->CIR = 0x00FF0000;
  RCC->CFGR2 = 0x00000000;
}

// **************************************************************
// Waits until flag's status in CR register is no longer 'RESET'
// **************************************************************
static void RCC_waitForCrFlagSet(uint32_t flagMask)
{
  while (1) {
    uint32_t statusReg = RCC->CR;

    statusReg &= flagMask;
    if (statusReg) {
      break;
    }
  }
}

// **************************************************************
// Configure the HSE clock
// Set source to external oscilator.
// **************************************************************
static void RCC_HSEConfig()
{
  RCC->CR &= (~RCC_CR_HSE_ON_MASK);
  RCC->CR &= (~RCC_CR_HSE_BYPASS_MASK);
  RCC->CR |= RCC_CR_HSE_ON_MASK;
}

// **************************************************************
// Configure a register according to clear mask 'clearMask'
// And a new value 'setMask'.
// **************************************************************
static void RCC_configCFGRReg(uint32_t clearMask, uint32_t setMask)
{
  if ((setMask & clearMask) != setMask) {
    //enterLiveLoop();
  }

  uint32_t curVal = RCC->CFGR;
  curVal &= (~clearMask);
  curVal |= setMask;
  RCC->CFGR = curVal;
}

// **************************************************************
// Configure a register according to clear mask 'clearMask'
// And a new value 'setMask'.
// **************************************************************
static void RCC_configCFGR2Reg(uint32_t clearMask, uint32_t setMask)
{
  if ((setMask & clearMask) != setMask) {
    //enterLiveLoop();
  }

  uint32_t curVal = RCC->CFGR2;
  curVal &= (~clearMask);
  curVal |= setMask;
  RCC->CFGR2 = curVal;
}


// **************************************************************
// Set PLL status according to newVal
// **************************************************************
static void RCC_setPLL_CR_BB(uint32_t offset, uint32_t newVal)
{
  SET_PLL_CR_BB(offset, newVal);
}

// **************************************************************
// Wait until sysclock is good.
// **************************************************************
static void RCC_waitForSysclock()
{
  while (1) {
    uint32_t statusReg = RCC->CFGR;

    statusReg &= CFGR_SW_SOURCE_MASK;
    if (statusReg == 0x08) {
      break;
    }
  }
}

// **************************************************************
// Activate perfipherial clocks for AHB
// **************************************************************
static void RCC_AHBPeriphClockActivate(uint32_t clocksMask)
{
  RCC->AHBENR |= clocksMask;
}

// **************************************************************
// Activate perfipherial clocks for APB2
// **************************************************************
static void RCC_APB2PeriphClockActivate(uint32_t clocksMask)
{
  RCC->APB2ENR |= clocksMask;
}

// **************************************************************
// System clocks configuration
// **************************************************************
static void RCC_Configuration()
{
  RCC_reset();                  // just to be on the safe side

  /* Enable HSE */
  RCC_HSEConfig();
  RCC_waitForCrFlagSet(RCC_CR_HSE_RDY_MASK);

  // Initialize FLASH
  FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
  FLASH_SetLatency(FLASH_Latency_2);

  // Configuration is as follows:
  // HSE     - 25 MHz
  // HCLK    - 72 MHz
  // PCLK2   - 72 MHz
  // PCLK1   - 36 MHz
  // ADC     - 6  MHz
  // PLL3    - 50 MHz
  // PLL2    - 40 MHz
  // PLL     - 72 MHz

  RCC_configCFGRReg(CFGR_HPRE_SET_MASK, RCC_SYSCLK_Div1);
  RCC_configCFGRReg(CFGR_PPRE1_SET_MASK, RCC_HCLK_Div2);
  RCC_configCFGRReg(CFGR_PPRE2_SET_MASK, 0);
  RCC_configCFGRReg(CFGR_ADCPRE_SET_MASK, RCC_PCLK2_Div6);
  RCC_configCFGR2Reg(CFGR2_PREDIV2_SET_MASK, RCC_PREDIV2_Div5);

  // PLL2
  RCC_configCFGR2Reg(CFGR2_PLL2MUL_SET_MASK, RCC_PLL2Mul_8);
  RCC_setPLL_CR_BB(PLL2ON_BitNumber, ENABLE);
  RCC_waitForCrFlagSet(RCC_CR_PLL2_RDY_MASK);

  // PLL
  RCC_configCFGR2Reg(CFGR2_PREDIV1_SET_MASK, RCC_PREDIV1_PLL2_Div5);
  RCC_configCFGRReg(CFGR_PLL_SET_MASK, RCC_PLLSource_Div1_Mul9);
  RCC_setPLL_CR_BB(PLLON_BitNumber, ENABLE);
  RCC_waitForCrFlagSet(RCC_CR_PLL_RDY_MASK);

  // set system clock to PLL
  RCC_configCFGRReg(CFGR_SW_SET_MASK, RCC_SYSCLKSource_PLLCLK);
  RCC_waitForSysclock();

  // Enable GPIOs clocks
  RCC_APB2PeriphClockActivate(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC |
                              RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE| RCC_APB2Periph_AFIO);
  // Enable ethernet clocks
  RCC_AHBPeriphClockActivate(RCC_AHBPeriph_ETH_MAC | RCC_AHBPeriph_ETH_MAC_Tx |
                             RCC_AHBPeriph_ETH_MAC_Rx);
  RCC_configCFGRReg(0,RCC_MCO_HSE);
                    
  
}

// **************************************************************
// Initializes the interrupt vector to point to the flash memory.
// **************************************************************
static void NVIC_Configuration()
{
  //SCB->VTOR = STM32_FLASH_BASE;
}

static void GPIO_setData(GPIO_TypeDef* gPIOx,
                         uint16_t pinID,
                         GPIOSpeed_TypeDef speed,
                         GPIOMode_TypeDef mode)
{
  GPIO_InitTypeDef GPIO_InitStructure = {pinID, speed, mode};
  GPIO_Init(gPIOx, &GPIO_InitStructure);
}

// **************************************************************
// Initializes the physical MII ethernet layer.
// Initialize SPI interface to the switch.
// Physical pin configuration:
// **************************************************************
static void ethernetPhysicalInit()
{
  uint16_t pinID;
  
  //         MDIO
  GPIO_PinRemapConfig(GPIO_Remap_ETH, ENABLE);
  GPIO_setData(GPIOA, GPIO_Pin_2, GPIO_Speed_50MHz, GPIO_Mode_AF_PP);
  // CRS, RX_CLK, COL
  pinID = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_3;
  GPIO_setData(GPIOA, pinID, GPIO_Speed_50MHz, GPIO_Mode_IN_FLOATING);
  // TXD[0,1,3], TX_EN, PIN5? 
  pinID = GPIO_Pin_5 | GPIO_Pin_8 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13;
  GPIO_setData(GPIOB, pinID, GPIO_Speed_50MHz, GPIO_Mode_AF_PP);
  GPIO_setData(GPIOB, GPIO_Pin_10, GPIO_Speed_50MHz, GPIO_Mode_IPD); //ERROR PIN MUST NOT BE FLOATING
  // TXD[2], TX_CLK
  pinID = GPIO_Pin_1 | GPIO_Pin_2;
  GPIO_setData(GPIOC, pinID, GPIO_Speed_50MHz, GPIO_Mode_AF_PP);
  GPIO_setData(GPIOC, GPIO_Pin_3, GPIO_Speed_50MHz, GPIO_Mode_IN_FLOATING);
  //RX_DV, RXD[0:3]
  pinID = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
  GPIO_setData(GPIOD, pinID, GPIO_Speed_50MHz, GPIO_Mode_IN_FLOATING);
  
}

// **************************************************************
// starts up the ethernet physical layer.
// **************************************************************
void ethernetMacInit()
{
  // clears the MII bit so that we can be sure we are running MII (and not RMII).
  GPIO_ETH_MediaInterfaceConfig(GPIO_ETH_MediaInterface_MII);

  // reset the AHB MAC peripherial
  RCC->AHBRSTR |= RCC_AHBPeriph_ETH_MAC;
  RCC->AHBRSTR &= ~RCC_AHBPeriph_ETH_MAC;

  // MAC software reset
  ETH->DMABMR |= ETH_DMABMR_SR;
  // All we have to do now is to wait for the software reset to finish.
  // Note that we must have an external clock working at this point - 
  // so we actually wait for the switch to wakeup.
  
  
  GPIO_setData(GPIOA, GPIO_Pin_8, GPIO_Speed_50MHz, GPIO_Mode_AF_PP);
  
  while ((ETH->DMABMR & ETH_DMABMR_SR) != 0);

}



// **************************************************************
// **************************************************************
int stm32SystemInit()
{

  ETH->DMAIER = 0; //Eth IT not valid
  ETH->DMASR = 0x1E7FF; //Reset pending IT

  /* System Clocks Configuration */
  RCC_Configuration();
  
  // NVIC configuration
  NVIC_Configuration();
  
  // Init the clock timer interrupt
  systemClockInit();



  // Initialize ethernet physical layer.
  // This initializes SPI as well. 
  while(getTimerTicks() < 100);
  ethernetPhysicalInit();
  //ethernetMacInit();
  GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); // switch to SW debug interface

  return 0;
}

// **************************************************************
// **************************************************************
void systemResetNoStats()
{
  NVIC_SystemReset();
}
