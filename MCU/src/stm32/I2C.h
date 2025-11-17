
#ifndef __I2C_H
#define __I2C_H
#include "stm32f10x_i2c.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
//#include "stm32f10x_dma.h"
//#include "constants.h"
#include "stm32f10x.h"
#include "misc.h"
//#include "Timing.h"
#ifndef NULL
  #define NULL 0
#endif

#define I2C_INTERRUPT_WD_MAX 255

#define I2C_SCL_Port GPIOB
#define I2C_SDA_Port GPIOB
#define I2C_SCL_Pin GPIO_Pin_6
#define I2C_SDA_Pin GPIO_Pin_7

#define READ_SCL (GPIO_ReadInputDataBit(I2C_SCL_Port, I2C_SCL_Pin) != Bit_RESET)
#define READ_SDA (GPIO_ReadInputDataBit(I2C_SDA_Port, I2C_SDA_Pin) != Bit_RESET)
#define SET_SCL(stats) GPIO_WriteBit(I2C_SCL_Port, I2C_SCL_Pin, (stats) ? Bit_SET : Bit_RESET);
#define SET_SDA(stats) GPIO_WriteBit(I2C_SDA_Port, I2C_SDA_Pin, (stats) ? Bit_SET : Bit_RESET);

typedef struct
{

	uint8_t Address; /*!< Slave address */

	uint8_t* pData; /*!< Pointer for recieved data */

	uint8_t Length; /*!< Length to be recieved */
        
        uint32_t* timeStamp = NULL; //when the message is recieved

} RxI2CMsg;

typedef struct
{

	uint8_t Address; /*!< Slave address */

	const uint8_t* pData; /*!< Pointer for recieved data */

	uint8_t Length; /*!< Length to be recieved */

} TxI2CMsg;

class I2C_DRV {
public:
	enum MsgStatus_Type{ Idle =0, Recieving = 1, Transmiting =2, MsgReady =3, Transcieving = 4};
	enum I2C_Error_Type{I2C_NoError, I2C_Timeout, I2C_Nack, I2C_ArbitrationLost, I2C_BusError, I2C_InterruptWatchdog, I2C_WrongParameters};
	enum I2C_State_Type : uint8_t{I2C_Idle = 0, I2C_DeviceAddress = 1, I2C_InternalAddress = 2, I2C_DataWrite = 3,
        I2C_ReStart = 4, I2C_DeviceAddress2 = 5, I2C_PrepareDataRead = 6, I2C_DataRead = 7, I2C_LastDataRead = 8, I2C_Stop = 9};

	MsgStatus_Type MsgStatus = Idle;
	RxI2CMsg RxMessage;
	TxI2CMsg TxMessage;
	I2C_Error_Type I2C_Last_Error;

	static I2C_DRV*	Get_Instance(){
		if (!m_pInstance)   // Only allow one instance of class to be generated
			m_pInstance = new I2C_DRV;
		return m_pInstance;
	}
	ErrorStatus Init(void);
	ErrorStatus SendMessage(const TxI2CMsg* Message);
	ErrorStatus RequestMessage(RxI2CMsg* Message);
	ErrorStatus SendRequestMessage(const TxI2CMsg* pTxMessage, RxI2CMsg* pRxMessage);
	uint8_t ScanBUS(uint8_t Address);
	BusyStatus PollAddress(uint8_t Address);
	inline void I2C_Handler(void);
	inline void I2C_ERROR_Handler(void);
	inline BusyStatus IsBusy(){
			static uint16_t timeout = 0;
			if ((MsgStatus != Idle && MsgStatus != MsgReady) || I2C_State != I2C_Idle)
	                  return BusyStatus::SET;
			else if (I2C_CheckEvent(I2C1,I2C_FLAG_BUSY)){
				timeout++;
				if (timeout == 0xFFF){
					Reset();
					timeout =0;
				}
				return BusyStatus::SET;
			}
			else {
				timeout =0;
				return BusyStatus::RESET;
			}
		}
        ErrorStatus WaitBusy();
	void Reset();
        I2C_State_Type I2C_State = I2C_Idle;
        uint8_t aaa = 0;

private:

	I2C_DRV(){
		Reset();
	};  							// Private so that it can  not be called
	I2C_DRV(I2C_DRV const&){};             	// copy constructor is private
	I2C_DRV& operator=(I2C_DRV const&){return *this;}; 	// assignment operator is private
	static I2C_DRV* m_pInstance;

	ErrorStatus GPIO_INIT(void);
	ErrorStatus IT_INIT(void);
	ErrorStatus StartCom();
	void BusReset();
	void FullReset();
	int InterruptWatchdogCounter = 0;
	//I2C_State_Type I2C_State = I2C_Idle;

};

inline int bIsFlagsSet(uint32_t lastevent, uint32_t flags)
{
  flags &= ((uint32_t)0x00FFFFFF);
  return ((lastevent & flags) == flags);
}


extern "C" void I2C1_EV_IRQHandler(void);
extern "C" void I2C1_ER_IRQHandler(void);


#endif
