#include "ADC.h"

ADC_DRV* ADC_DRV::m_pInstance = NULL;

ADC_DRV::ADC_DRV() : ADC_Inputs{0} {
    Init();
}

/* ADC Init shall config the ADC and the DMA, so the DMA will be able to constantly measure the ADC inputs (Temp, Vin_Sense, TVs_Vsense) */
ErrorStatus ADC_DRV::Init(void)
{
  ADC_InitTypeDef ADC_InitStruct;
  DMA_InitTypeDef DMA_InitStruct;
  
  ErrorStatus status = GPIO_INIT();
  ADC_DeInit(ADC1);                                    /* Reset the I2C1           */
  DMA_DeInit(DMA1_Channel4);
  //RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); /* Enable the I2C1 clock    */	
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);

  /* Configure ADC */
  ADC_StructInit(&ADC_InitStruct);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); 
  
  /* Update ADC default configuration values from init */
  ADC_InitStruct.ADC_Mode               = ADC_Mode_Independent;
  ADC_InitStruct.ADC_ScanConvMode       = ENABLE;
  ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStruct.ADC_ExternalTrigConv   = ADC_ExternalTrigConv_None;
  ADC_InitStruct.ADC_DataAlign          = ADC_DataAlign_Right;
  ADC_InitStruct.ADC_NbrOfChannel       = 3;
  ADC_Init(ADC1,&ADC_InitStruct);
  
  ADC_RegularChannelConfig(ADC1, ADC_Channel_9 , 1, ADC_SampleTime_239Cycles5); /* Temperature, Pin No.36 */
  ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 2, ADC_SampleTime_239Cycles5); /* Vin Sense, Pin No. 15  */
  ADC_RegularChannelConfig(ADC1, ADC_Channel_4 , 3, ADC_SampleTime_239Cycles5); /* TVs Vsense, Pin No. 29 */
  
  ADC_Cmd(ADC1, FunctionalState::ENABLE);
  ADC_DMACmd(ADC1,FunctionalState::ENABLE);
  /* Configure DMA */
  DMA_StructInit(&DMA_InitStruct);
  /* Update DMA default configuration values from init */
  DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)(&(ADC1->DR));
  DMA_InitStruct.DMA_MemoryBaseAddr     = (uint32_t)(&(ADC_Inputs[0]));
  DMA_InitStruct.DMA_DIR                = DMA_DIR_PeripheralSRC;
  DMA_InitStruct.DMA_BufferSize         = 3; 
  DMA_InitStruct.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
  DMA_InitStruct.DMA_MemoryInc          = DMA_MemoryInc_Enable;            
  DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStruct.DMA_MemoryDataSize     = DMA_MemoryDataSize_HalfWord; 
  DMA_InitStruct.DMA_Mode               = DMA_Mode_Circular;                 
  DMA_InitStruct.DMA_Priority           = DMA_Priority_Low;           
  DMA_InitStruct.DMA_M2M                = DMA_M2M_Disable;              
    
  DMA_Init(DMA1_Channel1,&DMA_InitStruct);
  
  //Enable DMA1 Channel transfer
  DMA_Cmd(DMA1_Channel1,FunctionalState::ENABLE);
  //Start ADC1 Software Conversion
  ADC_SoftwareStartConvCmd(ADC1,FunctionalState::ENABLE);
  
  for(int i=0;i<5;i++); //wait a little
  ADC_StartCalibration(ADC1);  // Extremely important
  uint32_t timeout =0;
  while(ADC_GetCalibrationStatus(ADC1)==SET){
          timeout++;
          if (timeout==0x0000FFFF) return ERROR;
  }
  
  
  return status;
}

ErrorStatus ADC_DRV::GPIO_INIT(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	//Configurate I2C pin
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;

	GPIO_InitStructure.GPIO_Pin = ADC_VIN_Pin;
	GPIO_Init(ADC_VIN_Port, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = ADC_TEMP_Pin;
	GPIO_Init(ADC_TEMP_Port, &GPIO_InitStructure);
        GPIO_InitStructure.GPIO_Pin = ADC_TVS_Pin;
        GPIO_Init(ADC_TVS_Port, &GPIO_InitStructure);

	return(SUCCESS);

}

int16_t ADC_DRV::Get_ADC_Specific_Input_Data(ADC_Inputs_Enum Input_Type) {
  int16_t inputAfterConstant;
  switch(Input_Type) {
    case ADC_In_Vin_Sense:
    case ADC_In_Tvs_Vsense:
      inputAfterConstant = (uint16_t)(ADC_Inputs[Input_Type] * ADC_Inputs_Constants[Input_Type]);
      break;
    case ADC_In_Temp:
      inputAfterConstant = Thermistor::Format(ADC_Inputs[Input_Type]);
      break;
    default:
      inputAfterConstant = 0; 
  }
  
  return inputAfterConstant;
}
