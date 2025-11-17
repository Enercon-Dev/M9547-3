#include "I2C.h"
#include "stm32Interface.h"

#define Timed(x) Timeout = 0xFFF; while (x) { if (Timeout-- == 0) goto errReturn;}
#define Delay(x) { uint32_t Timeout = x*75; while (Timeout--);}

I2C_DRV* I2C_DRV::m_pInstance = NULL;

ErrorStatus I2C_DRV::WaitBusy()
{
  uint16_t Timeout = 0xFFFF;
  while (IsBusy()) 
  { 
    if (Timeout-- == 0) 
      return ERROR;
  }
  return SUCCESS;
}

ErrorStatus I2C_DRV::SendMessage(const TxI2CMsg* Message){
	if (IsBusy())
		return ERROR;
	TxMessage.Address = Message->Address;
	TxMessage.Length = Message->Length;
	TxMessage.pData = Message->pData;
	MsgStatus = Transmiting;
	return StartCom();

}

ErrorStatus I2C_DRV::RequestMessage(RxI2CMsg* Message){
	if (IsBusy())
		return ERROR;
	RxMessage.Address = Message->Address;
	RxMessage.Length = Message->Length;
	RxMessage.pData = Message->pData;
        RxMessage.timeStamp = Message->timeStamp;
	MsgStatus = Recieving;
	return StartCom();
}


ErrorStatus I2C_DRV::SendRequestMessage(const TxI2CMsg* pTxMessage, RxI2CMsg* pRxMessage){
	if (IsBusy())
		return ERROR;
	RxMessage.Address = pRxMessage->Address;
	RxMessage.Length = pRxMessage->Length;
	RxMessage.pData = pRxMessage->pData;
        RxMessage.timeStamp = pRxMessage->timeStamp;
	TxMessage.Address = pTxMessage->Address;
	TxMessage.Length = pTxMessage->Length;
	TxMessage.pData = pTxMessage->pData;
	MsgStatus = Transcieving;
	return StartCom();
}

ErrorStatus I2C_DRV::StartCom(){
	if (I2C_State != I2C_Idle)
		return ERROR;

	//check parameters:
	if ((TxMessage.pData == NULL) || (TxMessage.Length == 0))
	{
		I2C_Last_Error = I2C_WrongParameters;
		return ERROR;
	}

	// Cleare last error
	I2C_Last_Error = I2C_NoError;
        I2C_ITConfig(I2C1, I2C_IT_EVT | I2C_IT_BUF | I2C_IT_ERR, DISABLE);
	I2C_ClearFlag(I2C1, I2C_FLAG_AF | I2C_FLAG_ARLO | I2C_FLAG_BERR);

	// Send START condition
	if (I2C_CheckEvent(I2C1,I2C_FLAG_BUSY)) //read flags in order to clear them, Check that is not still busy
		return ERROR;
       
	//TODO: enable clock stratching.
	I2C_State = I2C_DeviceAddress;
        I2C_GetLastEvent(I2C1);
        
        if (I2C_CheckEvent(I2C1,I2C_EVENT_SLAVE_STOP_DETECTED))
          Init();
	I2C_GenerateSTART(I2C1, ENABLE);

        
	//reset Interrupt watchdog
	InterruptWatchdogCounter = 0;
	// Enable event and buffer interrupts
	I2C_ITConfig(I2C1, I2C_IT_EVT | I2C_IT_BUF | I2C_IT_ERR, ENABLE);
        aaa = 0;
	return SUCCESS;
}

void I2C_DRV::I2C_Handler(void){

	static uint8_t ByteCounter = 0;
	uint8_t data;

	/* Relevant flags:
	  reg I2C_SR1:
	       SB       - Start condition generated (Master)
	       ADDR     - Address sent (Master)/ matched (Slave)
	       BTF      - Byte transfer finished
	       ADD10    - 10-bit header sent (Master)
	       STOPF    - Stop detection (Slave)
	       RxNE     - Data register not empty (receivers)
	       TxE      - Data register empty (transmitters) (not set during address phase)
	       BERR     - Bus error - Misplaced Start or Stop condition
	       ARLO     - Arbitration lost (Master)
	       AF       - Acknowledge failure
	       OVR      - Overrun/Underrun (only Slave and only when NOSTRETCH=1)
	       PECERR   - PEC Error in reception
	       TIMEOUT  - Timeout or Tlow error
	       SMBALERT - SMBus alert
	  reg I2C_SR2
	       MSL        - Master/slave
	       BUSY       - Bus busy
	       TRA        - Transmitter/receiver
	       GENCALL    - General call address (Slave) - irelevant
	       SMBDEFAULT - SMBus device default address (Slave) - irelevant
	       SMBHOST    - SMBus host header (Slave) - irelevant
	       DUALF      - Dual flag (Slave) - irelevant
	 */

	//Interrupt Watchdog
	InterruptWatchdogCounter++;
	if (InterruptWatchdogCounter >= I2C_INTERRUPT_WD_MAX)
	{
		I2C_GetLastEvent(I2C1);
		I2C_Last_Error = I2C_InterruptWatchdog;
		I2C_ITConfig(I2C1, I2C_IT_EVT | I2C_IT_BUF | I2C_IT_ERR, DISABLE);
		I2C_State = I2C_Idle;
		MsgStatus = Idle;
		return;
	}

	switch (I2C_State)
	{
	case I2C_Idle:
	        //BUG - ISR should never be trigered while in Idle state
	        if (!I2C_CheckEvent(I2C1,I2C_EVENT_SLAVE_STOP_DETECTED))
                  break;
                Init();
                break; // dealet with at the default case


	case I2C_DeviceAddress: //Transmit device Address (7bit + Rd/Wr bit)
          if (!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT)){ //EV5 - BUSY, MSL, SB
		aaa = 1;	
            break; //error
          }

		if (MsgStatus == Recieving)
		{
			I2C_Send7bitAddress(I2C1,RxMessage.Address << 1, I2C_Direction_Receiver);
			I2C_State = I2C_PrepareDataRead;
		}
		else if (MsgStatus == Transmiting || MsgStatus == Transcieving)
		{
			//transmit device address with Wr bit
			I2C_Send7bitAddress(I2C1, TxMessage.Address << 1, I2C_Direction_Transmitter);
			I2C_State = I2C_DataWrite;
		}
		ByteCounter = 0;
                aaa = 3;
		break;


	case I2C_DataWrite: //transmit Data
		if (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING) )   //EV6 - BUSY, MSL, TRA, TXE
			break; //error

		I2C_SendData(I2C1,TxMessage.pData[ByteCounter]);
		ByteCounter++;
		if (ByteCounter >= TxMessage.Length) //all data transmitted
		{
			if (MsgStatus == Transcieving)
				I2C_State = I2C_ReStart;
			else {
				// Disable buffer interrupt (since we are waiting for buffer to empty)
				I2C_ITConfig(I2C1, I2C_IT_BUF, DISABLE);
				I2C_State = I2C_Stop;
			}
		}
		break;

	case I2C_ReStart:
		if (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) //EV8_2 - TRA, BUSY, MSL, TXE and BTF flags
			break; //TODO: error
		I2C_ITConfig(I2C1, I2C_IT_BUF, DISABLE);
                I2C_AcknowledgeConfig(I2C1, ENABLE); //Enable acknowledgement
		I2C_GenerateSTART(I2C1, ENABLE);
		ByteCounter = 0;
		I2C_State = I2C_DeviceAddress2;
		break;


	case I2C_DeviceAddress2:
		if (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT) ) //EV5 - BUSY, MSL, SB
		{
			ByteCounter++;
			if (ByteCounter > 10) //let 100usec (2 I2C clockes) before declering error
				break; //TODO: error
			//shortDelay(10); //delay for 10uSec (1/5 I2C clock pulse)
			break;
		}
		I2C_Send7bitAddress(I2C1, RxMessage.Address << 1, I2C_Direction_Receiver);
		I2C_ITConfig(I2C1, I2C_IT_BUF, ENABLE);
		I2C_State = I2C_PrepareDataRead;
		break;

	case I2C_PrepareDataRead: //Address sent - get ready to receive data
		if (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))  //EV6(for rx) - BUSY, MSL, ADDR
			break; //error

		ByteCounter = 0;
		if (RxMessage.Length <= 1)
		{
			//there is only one byte to receive - NACK this byte and genereate STOP
			I2C_AcknowledgeConfig(I2C1, DISABLE); //Disable acknowledgement
			I2C_GenerateSTOP(I2C1, ENABLE); //Send STOP Condition
			I2C_State = I2C_LastDataRead;
		}
		else
		{
			I2C_State = I2C_DataRead;
		}
		break;


	case I2C_DataRead: //receive data
		if (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED))  //EV7 - BUSY, MSL, RXNE
			break; //error

		data = I2C_ReceiveData(I2C1);
		RxMessage.pData[ByteCounter] = data ;
		ByteCounter++;

		if (ByteCounter >= RxMessage.Length - 1) //received one before last byte
		{
			//NACK the last byte and genereate STOP
			I2C_AcknowledgeConfig(I2C1, DISABLE); //Disable acknowledgement
			I2C_GenerateSTOP(I2C1, ENABLE); //Send STOP Condition
			I2C_State = I2C_LastDataRead;
		}
		break;

	case I2C_LastDataRead:
		//if (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED))  //EV7 - BUSY, MSL, RXNE
		if (!I2C_CheckEvent(I2C1,  I2C_FLAG_RXNE&0x00FFFFFF))
			break; //error

		data = I2C_ReceiveData(I2C1);
		//I2C_ITConfig(I2C1, I2C_IT_EVT | I2C_IT_BUF | I2C_IT_ERR, DISABLE);
		RxMessage.pData[ByteCounter] = data ;
                if(RxMessage.timeStamp != NULL)
                  *(RxMessage.timeStamp) = getTimerTicks();
		I2C_State = I2C_Idle;
		MsgStatus = MsgReady;
		break;

	case I2C_Stop: //all data transmitted - generate STOP
		if (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) //EV8_2 - TRA, BUSY, MSL, TXE and BTF flags
			break; //error
		I2C_GenerateSTOP(I2C1, ENABLE);
		//I2C_ITConfig(I2C1, I2C_IT_EVT | I2C_IT_BUF | I2C_IT_ERR, DISABLE);
		I2C_State = I2C_Idle;
		MsgStatus = Idle;
		break;

	default:
		//BUG
		Reset(); //reset I2C module
		break;
	}



}

void I2C_DRV::I2C_ERROR_Handler()
{
	uint32_t lastevent;
	int bNoStop = true;

	lastevent = I2C_GetLastEvent(I2C1);
	I2C_ITConfig(I2C1, I2C_IT_EVT | I2C_IT_BUF | I2C_IT_ERR, DISABLE);
	I2C_State = I2C_Idle;
	MsgStatus = Idle;

	if (bIsFlagsSet(lastevent, I2C_FLAG_TIMEOUT)){ //Timeout Error
		//TODO: timeout should be enabled for Smbus communication and disabled for any other
		I2C_Last_Error = I2C_Timeout;
		I2C_ClearFlag(I2C1, I2C_FLAG_TIMEOUT);
                bNoStop = false;
	}
	if (bIsFlagsSet(lastevent , I2C_FLAG_AF)){
		I2C_Last_Error = I2C_Nack;
		I2C_ClearFlag(I2C1, I2C_FLAG_AF);
                bNoStop = false;
	}
	if (bIsFlagsSet(lastevent , I2C_FLAG_ARLO)){
		I2C_Last_Error = I2C_ArbitrationLost;
		I2C_ClearFlag(I2C1, I2C_FLAG_ARLO);
		bNoStop = true;
		Init();
	}
	if (bIsFlagsSet(lastevent , I2C_FLAG_BERR)){
		I2C_Last_Error = I2C_BusError;
		I2C_ClearFlag(I2C1, I2C_FLAG_BERR);
                bNoStop = true;
	}
	if (!bNoStop)
	{
		I2C_GenerateSTOP(I2C1, ENABLE); //TODO: check that stop actualy generated
	}
	NVIC_ClearPendingIRQ(I2C1_EV_IRQn);

}

uint8_t I2C_DRV::ScanBUS(uint8_t Address){
	__IO uint16_t Timeout = 0xFFF;
	if (IsBusy())
		return 0x80;
	while (Address <= 0x7F){
		Timed(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));
		I2C_GenerateSTART(I2C1, ENABLE);
		Timed(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
		// Send Address  EV5
		I2C_Send7bitAddress(I2C1, (Address << 1), I2C_Direction_Transmitter);
		Timeout = 0xFFF;
		while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) {
			if (--Timeout == 0) break;
		}
		I2C_GenerateSTOP(I2C1, ENABLE);
		if (Timeout != 0) {
			Timed(I2C_GetFlagStatus(I2C1, I2C_FLAG_STOPF));
			return Address;
		}
		Timed(I2C_GetFlagStatus(I2C1, I2C_FLAG_STOPF));
		Address++;
	}
	return Address;

	errReturn:
	return 0x80;
}

BusyStatus I2C_DRV::PollAddress(uint8_t Address){
	__IO uint16_t Timeout = 0xFFF;
	if (IsBusy())
		return SET;
	Timed(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));
	I2C_GenerateSTART(I2C1, ENABLE);
	Timed(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
	// Send Address  EV5
	I2C_Send7bitAddress(I2C1, (Address << 1), I2C_Direction_Transmitter);
	Timeout = 0xFF;
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) {
		if (--Timeout == 0) break;
	}
	I2C_GenerateSTOP(I2C1, ENABLE);
	if (Timeout != 0) {
		Timed(I2C_GetFlagStatus(I2C1, I2C_FLAG_STOPF));
		return RESET;
	}
	Timed(I2C_GetFlagStatus(I2C1, I2C_FLAG_STOPF));
	return SET;

	errReturn:
	return SET;

}

void I2C_DRV::Reset(){
	I2C_ITConfig(I2C1, I2C_IT_EVT | I2C_IT_BUF | I2C_IT_ERR, DISABLE); //disable I2C interrupts
	I2C_State = I2C_Idle;
	MsgStatus = Idle;
	I2C_Last_Error = I2C_NoError;

	Init();

}



void I2C_DRV::BusReset(){

	GPIO_InitTypeDef GPIO_InitStructure;
	int i;
	//Timer_S *Timer = Timer_S::Get_Instance();

	I2C_ITConfig(I2C1, I2C_IT_EVT | I2C_IT_BUF | I2C_IT_ERR, DISABLE); //disable I2C interrupts

	SET_SCL(1);
	SET_SDA(1);

	//Configurate I2C pin
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Pin = I2C_SCL_Pin;
	GPIO_Init(I2C_SCL_Port, &GPIO_InitStructure);

	//Configurate I2C pin
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Pin = I2C_SDA_Pin;
	GPIO_Init(I2C_SDA_Port, &GPIO_InitStructure);

	do{
		//wait for SCL to be released 100usec at most
		for (i=0; i<10; i++)
		{
			if (READ_SCL)
				break;
			//Timer->shortDelay(10);
		}

		if (!READ_SCL)
			break; //error - SCL held low by external device

		//if SDA held low - generate SCL pulses (7 at most)
		for (i=0; i<7; i++)
		{
			if (!READ_SDA)
			{
				Delay(25);
				SET_SCL(0);
				Delay(25);
				SET_SCL(1);
			}
		}

		if (!READ_SDA)
			break; //error - SDA held low by external device

		//generate START
		Delay(25);
		SET_SDA(0);
		Delay(25);
		SET_SCL(0);

		//generate STOP
		Delay(25);
		SET_SCL(1);
		Delay(25);
		SET_SDA(1);
	}while (false);

	//DO NOT restor the control over GPIO pins to the I2C module!!!
	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	//GPIO_Init(I2C_GPIO_PORT, &GPIO_InitStructure);

}
void I2C_DRV::FullReset(){

}

ErrorStatus I2C_DRV::Init(void)
{
	int status = SUCCESS;
	I2C_InitTypeDef I2C_InitStruct;


	/* Enable the I2C1 clock*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
        I2C_SoftwareResetCmd(I2C1,FunctionalState::ENABLE);
        NVIC_ClearPendingIRQ(I2C1_EV_IRQn);
        NVIC_ClearPendingIRQ(I2C1_ER_IRQn);
        I2C_SoftwareResetCmd(I2C1,FunctionalState::DISABLE);

	// Reset the I2C1
	BusReset();
	I2C_DeInit(I2C1);

	I2C_StructInit(&I2C_InitStruct);

	I2C_InitStruct.I2C_ClockSpeed = 400000;									/* initialize the I2C_ClockSpeed member */
	I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2; 	  					/* Initialize the I2C_DutyCycle member */
	I2C_InitStruct.I2C_OwnAddress1 = 0; 	  								/* Initialize the I2C_OwnAddress1 member */
	I2C_InitStruct.I2C_Ack = I2C_Ack_Disable; 	  							/* Initialize the I2C_Ack member */
	I2C_InitStruct.I2C_AcknowledgedAddress = 0x4700;//I2C_AcknowledgedAddress_7bit;	/* Initialize the I2C_AcknowledgedAddress member */
	I2C_Init(I2C1, &I2C_InitStruct);
	status &= GPIO_INIT();
	I2C_Cmd(I2C1, ENABLE);

	status &= IT_INIT();

	return((ErrorStatus)status);

}


ErrorStatus I2C_DRV::GPIO_INIT(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	//Configurate I2C pin
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = I2C_SCL_Pin;
	GPIO_Init(I2C_SCL_Port, &GPIO_InitStructure);

	//Configurate I2C pin
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStructure.GPIO_Pin = I2C_SDA_Pin;
	GPIO_Init(I2C_SDA_Port, &GPIO_InitStructure);

	return(SUCCESS);

}


ErrorStatus I2C_DRV::IT_INIT(void)
{
	NVIC_InitTypeDef  NVIC_InitStructure;


	// Configure and enable I2C event interrupt
	NVIC_InitStructure.NVIC_IRQChannel = I2C1_EV_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //DONE: what this for?
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; //DONE: what this for?
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	// Configure and enable I2C error interrupt
	NVIC_InitStructure.NVIC_IRQChannel = I2C1_ER_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //DONE: what this for?
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; //DONE: what this for?
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	return(SUCCESS);
}

void I2C1_EV_IRQHandler(void)
{
	I2C_DRV::Get_Instance()->I2C_Handler();
}

void I2C1_ER_IRQHandler(void)
{
	I2C_DRV::Get_Instance()->I2C_ERROR_Handler();
}

