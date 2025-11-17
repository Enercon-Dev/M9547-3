#include "eeprom.h"
#include "persistentData.h"
#include "stm32Interface.h"
#include "core_cm3.h"
#include <string.h>

EEprom::EEprom()
{
}

/* 
PURPOSE: 
Interface function to enable writing into the eeprom memory
PARAMETERS: 
address - location in the eeprom memory that the datat will be written
length - length of the data to be written.
pData - pointer to the data to be written to the eeprom
RETURN VALUE:
Success if writing is successfull, and error otherwise
ALGORITHM:
Data is stored into a local buffer and then moved to the eeprom memory (to avoid slicing, etc.) using WriteBufferToEeprom helper function
OTHERS:
The AT24MAC402/602 is capable of a 16-byte Page Write
*/
ErrorStatus EEprom::WriteToEEprom(void* address, uint8_t length, const void *pData)
{
  uint8_t write_buffer_[MAX_BUFFER_SIZE];
  write_buffer_[0] = (uint8_t*)address - (uint8_t *)0;
  memcpy(write_buffer_+1, pData, length);
  if (WaitBusy() == ErrorStatus::ERROR)
      return ErrorStatus::ERROR;
  if (WriteBufferToEeprom(address, write_buffer_, length) == ERROR)
    return ERROR;
  ReadFromEEprom(address, length, write_buffer_); /* Read data from the eeprom and put the data inside of write_buffer_ */
  int a =  (memcmp(pData, write_buffer_, length) ? ERROR : SUCCESS);
  return (ErrorStatus)a;
}

/* 
PURPOSE: 
Interface function to erase the eeprom memory
PARAMETERS: 
NONE
RETURN VALUE:
Success if erasure is successfull, and error otherwise
ALGORITHM:
Use the writeing method to zeroize addresses 00-FF
OTHERS:
NONE
*/
ErrorStatus EEprom::EraseEEprom()
{
  uint8_t erase_data[230] = {0};
  return WriteToEEprom(SYSTEM_DATA_BASE_ADDRESS, sizeof(erase_data), erase_data);
}

/* 
PURPOSE: 
Interface function to enable reading from eeprom memory
PARAMETERS: 
address - location in the eeprom memory that the data will be read from
length - length of the data to be read.
pData - pointer to the data to be written to the eeprom
RETURN VALUE:
Success if writing is successfull, and error otherwise
ALGORITHM:
OTHERS:
*/
ErrorStatus EEprom::ReadFromEEprom(void* address, uint8_t length, void *pData)
{
  uint8_t write_buffer_[1];
  RxI2CMsg rx_msg;
  TxI2CMsg command;
  rx_msg.Address = EEPROM_SLAVE_ADDRESS;
  rx_msg.Length = length;
  rx_msg.pData = (uint8_t*)pData;

  command.Address = EEPROM_SLAVE_ADDRESS; //address
  command.Length = 1;
  command.pData = write_buffer_;
  write_buffer_[0] = (uint8_t*)address - (uint8_t*)0;

  uint8_t rc = WaitBusy();
  rc &= I2C_DRV::Get_Instance()->SendRequestMessage(&command,&rx_msg);
  rc &= WaitBusy();
  return (ErrorStatus)rc;
}

/* 
PURPOSE: 
Helper function to WriteToEEprom function
PARAMETERS: 
address - location in the eeprom memory that the datat will be written
length - length of the data to be written.
RETURN VALUE:
Success if writing is successfull, and error otherwise
ALGORITHM:
If data can be stored in one row, send message. else, divide it into two messages.
OTHERS:
*/
ErrorStatus EEprom::WriteBufferToEeprom (void* address, uint8_t *buffer, uint8_t length)
{
  // is busy
  TxI2CMsg tx_msg;
  uint8_t part_one_length = 0;
  tx_msg.Address = EEPROM_SLAVE_ADDRESS;
  tx_msg.Length = length;
  tx_msg.pData = buffer;
  uint8_t eeprom_add = (uint8_t*)address - (uint8_t *)0;
  uint8_t buffer_head = 0;
  
  /* Check for memory boundries */
  /* If data should be written on more than one page, manipulate before the send to avoid data overriding */ 
  while(((eeprom_add + length - 1) & MULTIPLE_16) != (eeprom_add & MULTIPLE_16))
  {
    part_one_length = 16- (eeprom_add & 0xF);
    tx_msg.Length = part_one_length +1;
    if (I2C_DRV::Get_Instance()->SendMessage(&tx_msg) == ErrorStatus::ERROR)
      return ErrorStatus::ERROR;
    if (WaitBusy() == ErrorStatus::ERROR)
      return ErrorStatus::ERROR;
    
    tx_msg.pData += part_one_length;
    buffer_head += part_one_length;
    eeprom_add += part_one_length;
    buffer[buffer_head] = eeprom_add;
    length -= part_one_length;
    
  }
  /* If data can be written properly in one row, send it as is */
  tx_msg.Length = length +1;
  if (I2C_DRV::Get_Instance()->SendMessage(&tx_msg) == ErrorStatus::ERROR)
    return ErrorStatus::ERROR;  
  
  return WaitBusy();
}

/* 
PURPOSE: 
function to check if I2C is busy
PARAMETERS: 
RETURN VALUE:
Set if busy, else perform address polling
ALGORITHM:
OTHERS:
*/
FlagStatus EEprom::IsBusy(void)
{
  if (I2C_DRV::Get_Instance()->IsBusy())
    return SET;
  return I2C_DRV::Get_Instance()->PollAddress(EEPROM_SLAVE_ADDRESS); 
}

ErrorStatus EEprom::WaitBusy(void)
{

  int8_t now = (int8_t)(getTimerTicks() & 0xFF);
  int8_t next = (int8_t)(((uint8_t)now + MAX_WRITE_TIME) & 0xFF);
  while ( (int8_t)(next - now) >= 0)
  {
    if (IsBusy() == FlagStatus::RESET)
      return SUCCESS;
    now = (int8_t)(getTimerTicks() & 0xFF);
  }
  return ERROR;
}
