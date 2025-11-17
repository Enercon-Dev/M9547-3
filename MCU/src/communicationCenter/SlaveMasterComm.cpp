
#include "SlaveMasterComm.h"
#include "constants.h"


SlaveMasterComm::SlaveMasterComm() : mSPI(SPI_DRV::Get_Instance())
{
}
ErrorStatus SlaveMasterComm::xfetIoData(void* dataIn, void* dataOut)
{
  return mSPI.SPI_TransmitReceiveMessage(dataIn, (NUM_OF_Slave_Ports *2) , dataOut, (NUM_OF_Slave_Ports *2) );
}

ErrorStatus SlaveMasterComm::xfetIoDataWait(void* dataIn, void* dataOut)
{
  if (mSPI.SPI_TransmitReceiveMessage(dataIn, (NUM_OF_Slave_Ports *2)+4, dataOut, (NUM_OF_Slave_Ports *2)+4) != SUCCESS)
	  return ERROR;
  while (mSPI.SPI_Busy());
  return SUCCESS;
}
