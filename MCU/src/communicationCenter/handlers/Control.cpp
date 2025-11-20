// **********************************************************************
// **********************************************************************
// Copyright 2011 by Avi Owshanko. All rights reserved.
// **********************************************************************
// **********************************************************************


//#include "globalInterface.h"
#include "stm32Interface.h"
#include "systemManagement.h"
#include "swUpgrade/upgradeManager.h"
//#include "genericClasses.h"
#include "communicationCenter/communicationCenter.h"
#include "../Hardware/HardwareCenter.h"
#include "Control.h"
#include "systemManagement.h"
#include "core_cm3.h"
#include "persistentDataCenter.h"
#include "persistentCommon.h"

// **********************************************************************
// **********************************************************************
// ************************   Controller        *************************
// **********************************************************************
// **********************************************************************

// **************************************************************
// constructor
// **************************************************************
Controller::Controller( Layer3& udpLayer, CommunicationCenter& comm_center) : 
mBuffers(UDP_PORT_COMMANDS),
mUDPConnection(mBuffers, udpLayer),
mCommunicationCenter(comm_center),
isReset(0)
{ 
}


// **************************************************************
// initialization call.
// notifies that persistent settings are loaded.
// **************************************************************
void Controller::openSocket(){
  mUDPConnection.openObject(CONTROL_CLIENT_PORT, CONTROL_SERVER_PORT, 0 );
}


// **************************************************************
// Sends Status Message
// notifies that persistent settings are loaded.
// **************************************************************
void Controller::SendStatus(CONTROL_OPCODE  op, uint8_t broadcast){
  if (op == CONTROL_OPCODE_KEEPALIVE) {
    mUDPConnection.changeTargetSocket(CONTROL_KEPPALIVE_PORT);
  }
  if (broadcast)
  {
    const IPData& myIPData = SystemManagement::getCommunicationCenter().getIPData();
    //mUDPConnection.changeTargetIP((myIPData.mAddr & myIPData.mMask) | (~myIPData.mMask) );
    mUDPConnection.changeTargetIP(0xFFFFFFFF);
  }
  mBuffers.getOutBuffer().writeChar(op);
  Status_Msg_T Status_Msg;
  HardwareCenter::GetInstance().GetStatus(Status_Msg);
  /* Every parameter with more than 1 byte length, has to be sent in a reverse order */
  Status_Msg.TEMP = htons(Status_Msg.TEMP);
  for (int i=0; i<NUM_OF_CB; i++)
  {
    Status_Msg.OUT_VOLT[i] = htons(Status_Msg.OUT_VOLT[i]);
    Status_Msg.OUT_CURR[i] = htons(Status_Msg.OUT_CURR[i]);
  }
  Status_Msg.INPUT_VOLT = htons(Status_Msg.INPUT_VOLT);
  Status_Msg.INPUT_CURR = htons(Status_Msg.INPUT_CURR);
  
  
  mBuffers.getOutBuffer().writeBlock(&Status_Msg , sizeof(Status_Msg_T));
  
}



// **************************************************************
// Periodic maintenance of the telnet connection if open.
// **************************************************************
void Controller::periodicCall(){
  static uint8_t sec_5_flag = 1;
  
  if (isClosed()) return;
  
  if(readBuffer())
  {
    mBuffers.getOutBuffer().advanceReadTop();
    return;
  }
  if(HardwareCenter::GetInstance().GetAlert())
  {
    mUDPConnection.changeTargetSocket(CONTROL_ALERT_PORT);
    SendStatus(CONTROL_OPCODE_STATUS,1);
    mBuffers.getOutBuffer().advanceReadTop();
    HardwareCenter::GetInstance().AlertAcknoledge();
    return;
  }
  
  if ( getTimerTicks() % KEEPALIVE_INTERVAL < 300 && sec_5_flag == 1){
    sec_5_flag = 0;
    mUDPConnection.changeTargetSocket(CONTROL_SERVER_PORT);
    SendStatus(CONTROL_OPCODE_KEEPALIVE,1);
    mBuffers.getOutBuffer().advanceReadTop();
    return;
  }
  else if ( getTimerTicks() % KEEPALIVE_INTERVAL > 300)
    sec_5_flag = 1;  
  
  
  // refresh outbound stream
  mBuffers.getOutBuffer().advanceReadTop();
  if (mBuffers.getOutBuffer().getDataSize() == 0 && isReset)
    SystemManagement::registerReboot();
}



// **************************************************************
// Tries to read from the inbound TCP stream
// **************************************************************
uint8_t Controller::readBuffer()
{
  // refresh incoming stream
  //mBuffers.advanceReadTop();
  
  unsigned int inSize = mBuffers.getInBuffer().getUnreadSize();
  uint8_t Op;
  while (inSize > 0) {
    mBuffers.getInBuffer().advanceReadTop(1);
    Op = mBuffers.getInBuffer().readChar(); //advance read base by 1
    if (Op != 0)
      break;
    inSize = mBuffers.getInBuffer().getUnreadSize();
    
  }
  if (inSize < 3) {
    mBuffers.getInBuffer().advanceReadTop(inSize-1);
    mBuffers.getInBuffer().advanceRead(mBuffers.getInBuffer().getDataSize());
    return 0;
  }
  
  // Read Length and opcode
  mBuffers.getInBuffer().advanceReadTop(2);
  mUDPConnection.changeTargetIP(mUDPConnection.LastRecievedIP);
  mUDPConnection.changeTargetSocket(mUDPConnection.LastRecievedSocket);// CONTROL_SERVER_PORT);
  //uint8_t Op = mBuffers.getInBuffer().readChar();
  uint16_t Length;
  mBuffers.getInBuffer().readBlock((char *)&Length, 2);
  Length = ntohs(Length);
  if (mBuffers.getInBuffer().getUnreadSize() < Length - 3)
  {
    mBuffers.getInBuffer().advanceReadTop();
    mBuffers.getInBuffer().advanceRead(mBuffers.getInBuffer().getDataSize());
    return 0;
  }
  mBuffers.getInBuffer().advanceReadTop(Length-3);
  
  
  switch (Op){
  case CONTROL_OPCODE_GET_STATUS:
    SendStatus(CONTROL_OPCODE_STATUS,0);
    break;
  case CONTROL_OPCODE_GET_PDU_INFO:
    handleGetPDUInfo( mBuffers.getOutBuffer() );
    break;
  case CONTROL_OPCODE_SET_OUTPUT:
    sendAckNack(CONTROL_OPCODE_OUTPUT_ACK, HardwareCenter::GetInstance().handleSetOutput(mBuffers.getInBuffer()));
    break;
  case CONTROL_OPCODE_TRIP_RESET:
    sendAckNack(CONTROL_OPCODE_TRIP_RESET_ACK, HardwareCenter::GetInstance().handleTripRest(mBuffers.getInBuffer()));
    break;
  case CONTROL_OPCODE_RESET:
    isReset = 1;
    sendAckNack(CONTROL_OPCODE_RESET_ACK, COMMAND_SUCCESS::ACK);
    break;
  case CONTROL_OPCODE_SET_BATTLE:
    sendAckNack(CONTROL_OPCODE_BATTLE_ACK, HardwareCenter::GetInstance().handleSetBattle(mBuffers.getInBuffer()));
    break;
  case CONTROL_OPCODE_SET_LIMIT:
    sendAckNack(CONTROL_OPCODE_LIMIT_ACK, HardwareCenter::GetInstance().handleSetLimit(mBuffers.getInBuffer()));
    break;
  case CONTROL_OPCODE_SET_GROUP:
    sendAckNack(CONTROL_OPCODE_GROUP_ACK,HardwareCenter::GetInstance().handleSetGroup(mBuffers.getInBuffer()));
    break;
  case CONTROL_OPCODE_SET_POR_MACRO:
    sendAckNack(CONTROL_OPCODE_POR_ACK, handleSetPor());
    break;
  case CONTROL_OPCODE_SET_DCI_MACRO:
    sendAckNack(CONTROL_OPCODE_DCI_ACK, handleSetDci());
    break;
  case CONTROL_OPCODE_ERASE_DATA:
    sendAckNack(CONTROL_OPCODE_ERASE_ACK, handleErase());
    break;
  case CONTROL_OPCODE_SET_IP_MODE:
    sendAckNack(CONTROL_OPCODE_IP_MODE_ACK, handleSetIP());
    break;  
  case CONTROL_OPCODE_INPUT_LIMIT:
	  mBuffers.getOutBuffer().writeChar(CONTROL_OPCODE_INPUT_LIMIT_ACK);
	  HardwareCenter::GetInstance().handleSetVinLimit(mBuffers.getInBuffer(),mBuffers.getOutBuffer());
	  break;
  case CONTROL_OPCODE_GET_SW_INFO:
    mCommunicationCenter.getUpgradeMan().handleGetSoftwareInfo(mBuffers.getOutBuffer());
    break;
  case CONTROL_OPCODE_SET_SW_UPDATE:
    sendAckNack(CONTROL_OPCODE_SW_UPDATE_ACK, mCommunicationCenter.getUpgradeMan().handleSetSoftwareUpdate(mBuffers.getInBuffer()));
    break;
  case CONTROL_OPCODE_GET_CONFIG:
    mBuffers.getOutBuffer().writeChar(CONTROL_OPCODE_CONFIG);
    SystemManagement::getPersistentDataCenter().handleGetConfig(mBuffers.getOutBuffer());
    break;
  case CONTROL_OPCODE_SET_DEBUG:
    handleSetDebug();
    break;
  default:
    break;
  }
  mBuffers.getInBuffer().advanceRead(mBuffers.getInBuffer().getDataSize());    
  return 1;
  
}

void Controller::sendAckNack(CONTROL_OPCODE OpCode, COMMAND_SUCCESS result)
{
  mBuffers.getOutBuffer().writeChar(OpCode);
  mBuffers.getOutBuffer().writeChar(0);
  mBuffers.getOutBuffer().writeChar(4);
  mBuffers.getOutBuffer().writeChar(result);
}


void Controller::handleGetPDUInfo( Buffer& DataOut ){
  DataOut.writeChar(CONTROL_OPCODE_PDU_INFO);
  DataOut.writeChar(0);
  DataOut.writeChar(59);
  
  DataOut.writeBlock(CUSTOMER_PN, strlen(CUSTOMER_PN)+1);
  DataOut.writeNChar(0, 16-strlen(CUSTOMER_PN)-1);
  DataOut.writeBlock(SystemManagement::getPersistentDataCenter().getSerialNumber(), 8);
  DataOut.writeBlock(SystemManagement::getPersistentDataCenter().getSystemVersion().SW_Version_Raw, 20);
  DataOut.writeNChar(0, 32-20);
  
}

COMMAND_SUCCESS Controller::handleSetDebug(){
  uint16_t length = mBuffers.getInBuffer().readShort();
  switch (mBuffers.getInBuffer().readChar())
  {
  case 0:
    {
    char Serial[8];
    if (length != 12)
    {
      sendAckNack(CONTROL_OPCODE_DEBUG_ACK, NACK);
      return NACK;
    }
    mBuffers.getInBuffer().readBlock(Serial,8);
    sendAckNack(CONTROL_OPCODE_DEBUG_ACK, (SystemManagement::getPersistentDataCenter().setSerialNumber(Serial) == 0) ? ACK : NACK);
    return ACK;
    }
  case 1:
    if (length != 4)
    {
      sendAckNack(CONTROL_OPCODE_DEBUG_ACK, NACK);
      return NACK;
    }
    mBuffers.getOutBuffer().writeChar(CONTROL_OPCODE_DEBUG_ACK);
    mBuffers.getOutBuffer().writeChar(0);
    mBuffers.getOutBuffer().writeChar(NUM_OF_CB*2+4);
    mBuffers.getOutBuffer().writeChar(1);
    HardwareCenter::GetInstance().handelGetOutsTemp(mBuffers.getOutBuffer());
    return ACK;
  default:
    return NACK;
  }
  
  
}

COMMAND_SUCCESS Controller::handleSetPor()
{
  if (mBuffers.getInBuffer().readShort()!= (NUM_OF_CB *3 +3)){
    return NACK;
  }
  uint8_t ChannelOrder[NUM_OF_CB];
  uint16_t ChannelDelay[NUM_OF_CB];
  for (int i=0; i< NUM_OF_CB; i++)
    ChannelOrder[i] = mBuffers.getInBuffer().readChar();
  for (int i=0; i< NUM_OF_CB; i++)
    ChannelDelay[i] = mBuffers.getInBuffer().readShort();
  return (SystemManagement::getPersistentDataCenter().Set_PorSetting(ChannelOrder,ChannelDelay) == SUCCESS) ? ACK : EEPROM_ERROR;
}


COMMAND_SUCCESS Controller::handleSetDci()
{
  if (mBuffers.getInBuffer().readShort()!= (NUM_OF_CB *4 +3)){
    return NACK;
  }
  uint8_t ChannelOrder[NUM_OF_CB];
  uint8_t ChannelOperation[NUM_OF_CB];
  uint16_t ChannelDelay[NUM_OF_CB];
  for (int i=0; i< NUM_OF_CB; i++)
    ChannelOrder[i] = mBuffers.getInBuffer().readChar();
  for (int i=0; i< NUM_OF_CB; i++)
    ChannelOperation[i] = mBuffers.getInBuffer().readChar();
  for (int i=0; i< NUM_OF_CB; i++)
    ChannelDelay[i] = mBuffers.getInBuffer().readShort();
  return (SystemManagement::getPersistentDataCenter().Set_DCISetting(ChannelOrder,ChannelOperation,ChannelDelay) == SUCCESS) ? ACK : EEPROM_ERROR;
}


COMMAND_SUCCESS Controller::handleErase()
{
  if (mBuffers.getInBuffer().readShort()!= 35){
    return NACK;
  }
  char str[32];
  mBuffers.getInBuffer().readBlock(str,32);
  if (strcmp(str, "ERASE EEPROM M9547") == 0){
    isReset = 1;
    return SystemManagement::getPersistentDataCenter().FactoryReset() == SUCCESS ? ACK : EEPROM_ERROR;
  }
  return NACK;
}

COMMAND_SUCCESS Controller::handleSetIP()
{
   if (mBuffers.getInBuffer().readShort()!= 12){
    return NACK;
  }
  int Mode = mBuffers.getInBuffer().readChar();
  if (Mode < 0 ) return NACK;
  uint32_t IP, mask;
  if (mBuffers.getInBuffer().readBlock((char*)&IP,4) < 0) return NACK;
  mBuffers.getInBuffer().advanceRead(4);
  if (mBuffers.getInBuffer().readBlock((char*)&mask,4) < 0) return NACK;
  return SystemManagement::getPersistentDataCenter().Set_IpSetting(Mode, IP, mask) == SUCCESS ? ACK : EEPROM_ERROR;
  
}



// **************************************************************
// marks the telnet server as closing.
// **************************************************************
void Controller::closeSocket()
{
  mUDPConnection.closeObject();
}

// **************************************************************
// returns an indication whether server is closed.
// server is closed only when connection is closed as well.
// **************************************************************
bool Controller::isClosed() const
{
  if (!mUDPConnection.isClosed()) {
    return false;
  }
  
  return true;
}

// **************************************************************
// Sends the discovery request to the web.
// **************************************************************
