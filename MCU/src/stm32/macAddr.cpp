#include "stm32Interface.h"
#include "I2C.h"

#define MAC_ADDRESS_OFFSET 0x9A
#define EEPROM_SLAVE_ADDRESS 0x58

int getMacAddress(char* buffer)
{
  const uint8_t internal_add = MAC_ADDRESS_OFFSET;
  TxI2CMsg txmsg = { EEPROM_SLAVE_ADDRESS, &internal_add,1};
  RxI2CMsg rxmsg = {EEPROM_SLAVE_ADDRESS, (uint8_t *)buffer, 6};
  
   if (I2C_DRV::Get_Instance()->WaitBusy() == ERROR)
    return -1;
  I2C_DRV::Get_Instance()->SendRequestMessage(&txmsg,&rxmsg);
  //Timed(I2C_DRV::Get_Instance()->IsBusy() == BusyStatus::SET);
  if (I2C_DRV::Get_Instance()->WaitBusy() == ERROR)
    return -1;
  if (I2C_DRV::Get_Instance()->I2C_Last_Error == I2C_DRV::I2C_NoError)
    return 0;
  else
    return -1;
}