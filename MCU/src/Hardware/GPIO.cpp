#include "GPIO.h"
#include "stm32Interface.h"

GPIO* GPIO::mSingelton = NULL;

GPIO::GPIO()
{
  GPIO_InitTypeDef   GPIO_InitStructure;
  
  // GPIO - DCI, RAT, SHDN, WP
  GPIO_InitStructure.GPIO_Pin   = DCI_Pin | RAT_Pin | SHDN_Pin| WP_Pin; ;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
  GPIO_Init(IO_Port, &GPIO_InitStructure);
  
  // GPIO - FAULT 
  GPIO_InitStructure.GPIO_Pin   = FAULT_Pin;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
  GPIO_Init(IO_Port, &GPIO_InitStructure);
  
  gpio_Inputs.DCI          = 0;
  gpio_Inputs.RAT          = 0;
  gpio_Inputs.SHDN         = 0;
  gpio_Inputs.SHDN_H2L_Flag = 0;
  gpio_Inputs.DCI_L2H_Flag = 0;
  gpio_Inputs.RAT_L2H_Flag = 0;
  
  filt_DCI  = 1;
  filt_RAT  = 1;
  filt_SHDN = 1;
  filt_WP = 1;

  acc_DCI  = 255;
  acc_RAT  = 255; 
  acc_SHDN = 100;
  acc_WP = 255;
}

void GPIO::periodicCall()
{
  /* DCI Acc&Filt */
  if (GPIO_ReadInputDataBit(IO_Port, DCI_Pin) == 0) 
    acc_DCI = ( acc_DCI == 255 ) ? 255 : acc_DCI + 1;
  else
    acc_DCI = ( acc_DCI == 0 ) ? 0 : acc_DCI - 1;
    
    if (filt_DCI == 0) 
    {
      filt_DCI = ( acc_DCI > 170 ) ? 1 : 0;
      gpio_Inputs.DCI_L2H_Flag |= filt_DCI;
    }
    else 
    {
      filt_DCI = ( acc_DCI > 85 ) ? 1 : 0;
    }
    
    gpio_Inputs.DCI = filt_DCI;
    
    /* RAT Acc&Filt */
    if (GPIO_ReadInputDataBit(IO_Port, RAT_Pin) == 0) 
      acc_RAT = ( acc_RAT == 255 ) ? 255 : acc_RAT + 1;
    else
      acc_RAT = ( acc_RAT == 0 ) ? 0 : acc_RAT - 1;
      
      if (filt_RAT == 0){
        filt_RAT = ( acc_RAT > 170 ) ? 1 : 0;
        gpio_Inputs.RAT_L2H_Flag |= filt_RAT;
      }
      else
      {
        filt_RAT = ( acc_RAT > 85 ) ? 1 : 0;
        
      }
      
      gpio_Inputs.RAT = filt_RAT;
      

      /* WP Acc&Filt */
            if (GPIO_ReadInputDataBit(IO_Port, WP_Pin) == 0)
              acc_WP = ( acc_WP == 255 ) ? 255 : acc_WP + 1;
            else
            	acc_WP = ( acc_WP == 0 ) ? 0 : acc_WP - 1;

        if (acc_WP == 0)
          filt_WP = ( acc_WP > 170 ) ? 1 : 0;
        else
        {
          filt_WP = ( acc_WP > 85 ) ? 1 : 0;
        }

        gpio_Inputs.WP_MCU = filt_WP;


      /* SHDN Acc&Filt */
      if (GPIO_ReadInputDataBit(IO_Port, SHDN_Pin) == 0) 
        acc_SHDN = ( acc_SHDN == 255 ) ? 255 : acc_SHDN + 1;
      else
        acc_SHDN = ( acc_SHDN == 0 ) ? 0 : acc_SHDN - 1;
        
  if (filt_SHDN == 0)
    filt_SHDN = ( acc_SHDN > 170 ) ? 1 : 0;
  else
  {
    filt_SHDN = ( acc_SHDN > 85 ) ? 1 : 0;
    gpio_Inputs.SHDN_H2L_Flag |= !filt_SHDN;
  }
    
    gpio_Inputs.SHDN = filt_SHDN;
}

GPIO_Inputs GPIO::Get_GPIO_Inputs()
{
  GPIO_Inputs res = gpio_Inputs;
  gpio_Inputs.DCI_L2H_Flag = 0;
  gpio_Inputs.RAT_L2H_Flag = 0;
  gpio_Inputs.SHDN_H2L_Flag = 0;
   
  return res;
}

/* This function updates the fault output */
void GPIO::SetFault(FunctionalState enable)
{
  if (enable) {
    GPIO_SetBits(IO_Port,FAULT_Pin);
  } else {
    GPIO_ResetBits(IO_Port,FAULT_Pin);
  }
}


  
