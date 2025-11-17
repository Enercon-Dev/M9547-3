
#ifndef __ADC_H
#define __ADC_H
#include "stm32f10x_adc.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_dma.h"
#include "constants.h"
#include "stm32f10x.h"
#include "misc.h"
#include "I2C.h"
#include "Thermistor.h"
#ifndef NULL
  #define NULL 0
#endif

#define A2D_SLAVE_ADD (0x30)

/* ADC Inputs enum */
typedef enum {
  ADC_In_Temp          = 0,
  ADC_In_Vin_Sense        ,
  ADC_In_Tvs_Vsense       ,
  ADC_Number_Of_Inputs    ,
} ADC_Inputs_Enum;

/*
Explanation to constans array:
  Data (Resistors, Connectivities, etc.) is taken from the 9811 electronic circuit. Based on the fact that Vin_sense = 2.5V is (2^12)-1, we get 4 equaions to get the const for each input:
  Vin_sense = (Vbus * R5)/(R2 + R5) => Vin_sense = (Vbus * 69.8)/(69.8 + 2740) => (1) Vin_sense = (Vbus * 69.8) / 2809.8 => Vbus = 100.637
  (2) Vbus_dig = Vbus * 16 => Vbus_dig = 1610
  (3) Vin_dig = (Vin_sense /2.5) * (2^12 -1) => Vin_dig = 4095
  (4) Vbus_dig = CONSTANTS * Vin_dig => CONSTANTS = 1610 / 4095 = 0.393
  So, Vin_sense constant value is : 0.393
  Fixed Point - 4 LSB bits (0 - 3) are reserved for the floating part, and the rest of the bits (4 - 15) for the integer (This is why we multiply by 16)

Rest of the calculations are the same.

Note: Positions according to ADC_Inputs enum, for example, ADC_Inputs_Constants[ADC_In_Temp] holds the factor for Temp, and so on 
*/
constexpr double ADC_Inputs_Constants[ADC_Number_Of_Inputs] = {1, 0.393, 0.393}; 

/* Ports */
#define ADC_TEMP_Port GPIOB
#define ADC_VIN_Port  GPIOC
#define ADC_TVS_Port  GPIOA

/* Pins */
#define ADC_TVS_Pin  4
#define ADC_TEMP_Pin 1
#define ADC_VIN_Pin  0

static uint8_t RequestVoltage[] = {0};

class ADC_DRV {
public:
	static ADC_DRV&	Get_Instance(){
		if (!m_pInstance)   // Only allow one instance of class to be generated
			m_pInstance = new ADC_DRV;
		return *m_pInstance;;
	}
        
        int16_t Get_ADC_Specific_Input_Data(ADC_Inputs_Enum Input_Type);
        
private:

        ADC_DRV();						// Private so that it can  not be called
	ADC_DRV(ADC_DRV const&){};             	// copy constructor is private
	ADC_DRV& operator=(ADC_DRV const&){return *this;}; 	// assignment operator is private
	static ADC_DRV* m_pInstance;
        
	ErrorStatus Init(void);
	ErrorStatus GPIO_INIT(void);
        ErrorStatus ConvChannel(uint8_t Channel);
        
        uint16_t ADC_Inputs[ADC_Number_Of_Inputs];
};


#endif
