#ifndef EEPROM_H
#define EEPROM_H

#include "I2C.h"
#include "constants.h"
#include "stm32f10x.h"
#include "misc.h"
//#include "Timing.h"
#include "string.h"

#define EEPROM_SLAVE_ADDRESS 0x50 
#define EEPROM_MAC_ADDRESS   0x58
#define MULTIPLE_16          0xF0
#define MAX_BUFFER_SIZE      256
#define MAX_WRITE_TIME       10          //5ms maximum write cycle time

class EEprom
{
public:
  EEprom                          (void);
  ErrorStatus WriteToEEprom       (void* Address, uint8_t length, const void *pData);
  ErrorStatus EraseEEprom         (void);
  ErrorStatus ReadFromEEprom      (void* Address, uint8_t length, void *pData);

private:
  FlagStatus  IsBusy              (void);
  ErrorStatus WaitBusy            (void);
  
  
  ErrorStatus WriteBufferToEeprom (void* address, uint8_t *buffer, uint8_t len);
};
#endif // EEPROM_H
