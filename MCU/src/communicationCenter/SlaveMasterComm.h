#ifndef SLAVE_MASTER_COMM_H
#define SLAVE_MASTER_COMM_H

#include "SPI.h"


class SlaveMasterComm
{
public:
  SlaveMasterComm();
  ErrorStatus xfetIoData(void* dataIn, void* dataOut);
  ErrorStatus xfetIoDataWait(void* dataIn, void* dataOut);
private:
  SPI_DRV& mSPI;

};

#endif
