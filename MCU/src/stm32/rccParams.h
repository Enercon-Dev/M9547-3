#ifndef RCC_PARAMS_H
#define RCC_PARAMS_H

#define RCC_CR_HSE_BYPASS_MASK  ((uint32_t)0x00040000)
#define RCC_CR_HSE_ON_MASK      ((uint32_t)0x00010000)
#define RCC_CR_HSE_RDY_MASK     ((uint32_t)1 << 0x11)
#define RCC_CR_PLL_RDY_MASK     ((uint32_t)1 << 0x19)
#define RCC_CR_PLL2_RDY_MASK    ((uint32_t)1 << 0x1B)
#define RCC_CR_PLL3_RDY_MASK    ((uint32_t)1 << 0x1D)

#define RCC_AHBPeriph_ETH_MAC           ((uint32_t)0x00004000)
#define RCC_AHBPeriph_ETH_MAC_Tx        ((uint32_t)0x00008000)
#define RCC_AHBPeriph_ETH_MAC_Rx        ((uint32_t)0x00010000)

#define CFGR_SW_SET_MASK        ((uint32_t)0x00000003)
#define CFGR_SW_SOURCE_MASK     ((uint32_t)0x0000000C)
#define CFGR_HPRE_SET_MASK      ((uint32_t)0x000000F0)
#define CFGR_PPRE1_SET_MASK     ((uint32_t)0x00000700)
#define CFGR_PPRE2_SET_MASK     ((uint32_t)0x00003800)
#define CFGR_ADCPRE_SET_MASK    ((uint32_t)0x0000C000)
#define CFGR_PLL_SET_MASK       ((uint32_t)0x003D0000)

#define CFGR2_PREDIV1_SET_MASK  ((uint32_t)0x0001000F)
#define CFGR2_PREDIV2_SET_MASK  ((uint32_t)0x000000F0)
#define CFGR2_PLL2MUL_SET_MASK  ((uint32_t)0x00000F00)
#define CFGR2_PLL3MUL_SET_MASK  ((uint32_t)0x0000F000)

// CFGR register
#define RCC_SYSCLK_Div1                  ((uint32_t)0x00000000)
#define RCC_SYSCLKSource_PLLCLK          ((uint32_t)0x00000002)
#define RCC_HCLK_Div2                    ((uint32_t)0x00000400)
#define RCC_PCLK2_Div6                   ((uint32_t)0x00008000)
#define RCC_PLLSource_Div1_Mul9          ((uint32_t)0x001D0000)
#define RCC_MCO_HSE                      ((uint32_t)0x06000000)

// CFGR2 register
#define RCC_PREDIV2_Div5                 ((uint32_t)0x00000040)
#define RCC_PLL2Mul_8                    ((uint32_t)0x00000600)
#define RCC_PLL3Mul_10                   ((uint32_t)0x00008000)
#define RCC_PREDIV1_PLL2_Div5            ((uint32_t)0x00010004)

// RCC peripherial clocks
#define RCC_APB1Periph_USART2            ((uint32_t)0x00020000)
#define RCC_APB1Periph_UART4             ((uint32_t)0x00080000)
#define RCC_APB1Periph_UART5             ((uint32_t)0x00100000)

#define RCC_APB2Periph_AFIO              ((uint32_t)0x00000001)
#define RCC_APB2Periph_GPIOA             ((uint32_t)0x00000004)
#define RCC_APB2Periph_GPIOB             ((uint32_t)0x00000008)
#define RCC_APB2Periph_GPIOC             ((uint32_t)0x00000010)
#define RCC_APB2Periph_GPIOD             ((uint32_t)0x00000020)
#define RCC_APB2Periph_GPIOE             ((uint32_t)0x00000040)
#define RCC_APB2Periph_USART1            ((uint32_t)0x00004000)

// Alias word address of PLLON bit
#define PLLON_BitNumber           0x18
#define PLL2ON_BitNumber          0x1A
#define PLL3ON_BitNumber          0x1C

#define RCC_OFFSET                (RCC_BASE - PERIPH_BASE)
#define CR_OFFSET                 (RCC_OFFSET + 0x00)
#define PLL_ADDRESS(offset)       (PERIPH_BB_BASE + (CR_OFFSET * 32) + (offset * 4))
#define SET_PLL_CR_BB(offset, val)                                  \
  {                                                                 \
    *(__IO uint32_t *)(PLL_ADDRESS(offset)) = val;                  \
  }

#define CFGR_BYTE4_ADDRESS        ((uint32_t)0x40021007)
#define RCC_MCO_PLL3CLK_Div2      ((uint8_t)0x09)

























#endif // RCC_PARAMS_H