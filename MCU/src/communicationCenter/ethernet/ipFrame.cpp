// **********************************************************************
// **********************************************************************
// Copyright 2011 by Avi Owshanko. All rights reserved.
// **********************************************************************
// **********************************************************************

#include "ipFrame.h"
#include "globalInterface.h"
#include "arp.h"
#include "persistentDataCenter.h"
#include "../communicationCenter.h"
#include "mac.h"
#include "systemManagement.h"
#include "stm32Interface.h"
#include <string.h>

// **********************************************************************
// **********************************************************************
// **************************   IP Header     ***************************
// **********************************************************************
// **********************************************************************

// **************************************************************
// IP header constructor.
// Reads the IP header, verifies all data, and registers all that is needed.
// **************************************************************
IPHeader::IPHeader(InFrame& inFrame, Arp& arp) :
  mFrameType(IP_INVALID_TYPE),
  mHeaderBase(inFrame.getCurDataPtr())
{
  if (!readLength(inFrame)) {
    return;
  }

  // we can discard with the datagram identification, but we must verify
  // that this is not a fragmented message. Discard message if it is.
  uint32_t fragment = inFrame.readNextHostU32();
  if ((fragment & 0xFFBF0000) != 0) {
    return;
  }

  uint16_t protocol = inFrame.readNextHostU16();

  inFrame.readNextHostU16();        // skip checksum field.
  /*if ((calcChecksum(mHeaderBase, mHeaderLen, 0, 0) != 0)) {
    return;
  }
*/
  if (!readIPs(inFrame)) {
    return;
  }

  decodeOptions(inFrame);
  arp.updateArpEntry(mSrcIP, inFrame.getSrcMAC());

  // sets frame type to valid as last step.
  decodeFrameType(protocol);
}

// **************************************************************
// reads the following data:
// 1. frame length (updates 'mFrameLen')
// 2. service (verifies V4)
// 3. header length
// 4. data length (updates 'mDataLen')
// returns 'false' on error. 'true' otherwise.
// **************************************************************
bool IPHeader::readLength(InFrame& inFrame)
{
  unsigned int frameLen = inFrame.getDataLen();
  if (frameLen < 20) {
    // ipHeader is at least 20 octets.
    return false;
  }

  uint16_t service = inFrame.readNextHostU16();
  // verify that the message is V4
  if ((service & 0x00F0) != 0x0040) {
    return false;
  }

  // read the header length
  unsigned int headerLen = (service & 0x000F) * 4;
  if ((headerLen < 20) || (headerLen > frameLen)) {
    return false;
  }

  uint16_t ipLen = inFrame.readNextNetworkU16();
  if ((ipLen < headerLen) || (ipLen > frameLen)) {
    return false;
  }

  mDataLen = ipLen - headerLen;
  mHeaderLen = headerLen;

  return true;
}

// **************************************************************
// reads source IP
// returns 'false' if not in subnet and not the gateway.
// **************************************************************
bool IPHeader::readIPs(InFrame& inFrame)
{
  //source lagit
  mSrcIP = inFrame.readNextIP();
  //const IPData& myIPData = SystemManagement::getCommunicationCenter().getIPData();
  /*if ((!myIPData.isSameSubnet(mSrcIP)) &&
      (!myIPData.isGateway(mSrcIP)))
  {
    return false;
  }
*/
  //dest lagit
  mDstIP = inFrame.readNextIP();
  

  if ( !isToMe(mDstIP) || mDstIP == 0) {  
    return false;
  }

  return true;
}

bool IPHeader::isToMe( uint32_t IP){
  //is broadcast in subnet?
  const IPData& myIPData = SystemManagement::getCommunicationCenter().getIPData();
  uint8_t flag = 0;
  for (int i =0 ; i<32 ;i+=8){
    if (flag){
      if ((IP >> i) & 0xFF != (myIPData.mAddr >> i) & 0xFF)
        return false;
    }
    else if ((IP >> i) & 0xFF != 0xFF)
      flag =1;
    
  }
  return true;
}

// **************************************************************
// skip over all the options
// **************************************************************
void IPHeader::decodeOptions(InFrame& inFrame)
{
  unsigned int count;
  for (count = 0; count < (mHeaderLen-20); count+=4) {
    inFrame.readNextHostU32();
  }
}

// **************************************************************
// decodes frame type
// **************************************************************
void IPHeader::decodeFrameType(uint16_t protocol)
{
  protocol = protocol & 0x0000FF00;

  switch (protocol) {
  case 0x0100:
    mFrameType = IP_ICMP_TYPE;
    break;
  case 0x0600:
    mFrameType = IP_TCP_TYPE;
    break;
  case 0x1100:
    mFrameType = IP_UDP_TYPE;
    break;
  default:
    mFrameType = IP_INVALID_TYPE;
  }
}


// **********************************************************************
// **********************************************************************
// **********************   Outbound IP Messge   ************************
// **********************************************************************
// **********************************************************************

uint16_t OutIPMsg::mIdentification = 1;

// **************************************************************
// Initiates an IP output message.
// Will write the physical data, and IP header
// IP checksum is left empty.
// Checksums will be updates when sending message.
// **************************************************************
OutIPMsg::OutIPMsg(uint32_t targetIP,
                   IPFrameType frameType,
                   CommunicationCenter& communicationCenter) :
  OutFrame(SystemManagement::getCommunicationCenter().getIPData().getFirsthop(targetIP),
           communicationCenter),
  mIPHeaderBase(NULL)
{
  if (isBad()) {
    return;
  }

  mIPHeaderBase = getCurPtr();
  writeIPHeader(targetIP, frameType, 0, NULL);
}

// **************************************************************
// Initiates an IP ping reply.
// Will write the physical data, and IP header.
// IP checksum is left empty.
// Checksums will be updates when sending message.
// **************************************************************
OutIPMsg::OutIPMsg(uint32_t targetIP,
                   unsigned int optionsLen,
                   const char* optionsBase,
                   CommunicationCenter& communicationCenter) :
  OutFrame(SystemManagement::getCommunicationCenter().getIPData().getFirsthop(targetIP),
           communicationCenter),
  mIPHeaderBase(NULL)
{
  if (isBad()) {
    return;
  }

  mIPHeaderBase = getCurPtr();
  writeIPHeader(targetIP, IP_ICMP_TYPE, optionsLen, optionsBase);
}

// **************************************************************
// Invokes the physical layer 'getCurPtr' method
// **************************************************************
char* OutIPMsg::getCurPtr()
{
  return OutFrame::getCurPtr();
}

// **************************************************************
// Invokes the physical layer 'writeHostU16' method
// **************************************************************
int OutIPMsg::writeHostU16(uint16_t val)
{
  return OutFrame::writeHostU16(val);
}

// **************************************************************
// Invokes the physical layer 'appendData' method
// **************************************************************
int OutIPMsg::appendData(const char* blockBase, unsigned int blockSize)
{
  return OutFrame::appendData(blockBase, blockSize);
}

// **************************************************************
// Invokes the physical layer 'appendData' method
// **************************************************************
int OutIPMsg::appendData(const Buffer& src, unsigned int blockSize)
{
  return OutFrame::appendData(src, blockSize);
}

// **************************************************************
// Invokes the physical layer 'appendData' method
// **************************************************************
int OutIPMsg::appendData(const Buffer& src, unsigned int bufferOffset, unsigned int blockSize)
{
  return OutFrame::appendData(src, bufferOffset, blockSize);
}

// **************************************************************
// Updates the length field and checksum for this IP message after appending all data.
// **************************************************************
int OutIPMsg::sendMsg()
{
  updateIPMsgLen();

  return OutFrame::sendMsg();
}

// **************************************************************
// Updates the IP header
// updates the length field, and calculates the checksum field.
// **************************************************************
void OutIPMsg::updateIPMsgLen()
{
  if (isBad() || (mIPHeaderBase == NULL)) {
    setBad();
    return;
  }

  uint16_t msgLen = getCurPtr() - mIPHeaderBase;
  msgLen = htons(msgLen);
  *(uint16_t*)(mIPHeaderBase + 2) = msgLen;

  uint16_t* crcAddr = (uint16_t*)(mIPHeaderBase + 10);
  unsigned int headerLen = 4 * (mIPHeaderBase[0] & 0x0F);
  *crcAddr = calcChecksum(mIPHeaderBase, headerLen, 0, *crcAddr);
}

// **************************************************************
// Appends the IP layer data to 'answer' message.
// **************************************************************
void OutIPMsg::writeIPHeader(uint32_t targetIP,
                             IPFrameType frameType,
                             unsigned int optionsLen,
                             const char* optionsBase)
{
  if ((optionsLen > 40) || ((optionsLen % 4) != 0)) {
    // we add IP options only for ping. Hence size must
    // be alligned to 4 bytes.
    setBad();
    return;
  }
  uint16_t headerLen = 0x0045;        // set version to '4', and length to '20'.
  headerLen += (optionsLen / 4);
  writeHostU16(headerLen);

  writeHostU16(0);
  writeNetworkU16(getIdentification());
  // set the "do not fragment" bit.
  writeHostU16(0x0040);
  writeFrameType(frameType);
  writeHostU16(0x0000);
  writeIP(SystemManagement::getCommunicationCenter().getIPAddr());
  writeIP(targetIP);

  if ((optionsLen > 0) && isGood()) {
    char* optionsReplyBase = getCurPtr();
    appendData(optionsBase, optionsLen);
    parseOptions(optionsReplyBase, optionsLen);
  }
}

// **************************************************************
// Updates the frame type in outbound message.
// **************************************************************
void OutIPMsg::writeFrameType(IPFrameType frameType)
{
  switch (frameType) {
  case IP_TCP_TYPE:
    writeHostU16(0x0680);
    break;
  case IP_UDP_TYPE:
    writeHostU16(0x1180);
    break;
  case IP_ICMP_TYPE:
    writeHostU16(0x0180);
    break;
  default:
    setBad();
    break;
  }
}

// **************************************************************
// Returns a unique IP identification nubmer.
// **************************************************************
void OutIPMsg::parseOptions(char* optionsBase,
                            unsigned int optionsLen)
{
  unsigned int offset = 0;
  while (offset < optionsLen) {
    unsigned int optionLen = 0;
    // discard option 'copy' bit
    char nextOption = optionsBase[offset] & 0x7F;

    switch (nextOption) {
    case 0:
      // End of option list
      return;
    case 1:
      // NOP
      optionLen = 1;
      break;
    case 2:
      // Security
      optionLen = 11;
      break;
    case 3:
      // loose source route - ignore
      optionLen = optionsBase[offset + 1];
      break;
    case 7:
      updateRecordRoute(optionsBase, optionsLen, offset);
      optionLen = optionsBase[offset + 1];
      break;
    case 8:
      // obsolete
      optionLen = 4;
      break;
    case 9:
      // strict source route
      optionLen = optionsBase[offset + 1];
      break;
    case 11:
    case 12:
      // MTU probe / reply
      optionLen = 4;
      break;
    case 20:
      // router alert
      optionLen = 4;
      break;
    case 68:
      // Internet timestamp
      updateInternetTimestamp(optionsBase, optionsLen, offset);
      optionLen = optionsBase[offset + 1];
      break;
    case 82:
      // Traceroute program
      optionLen = optionsBase[offset + 1];
      break;
    default:
      // exit on unknown options
      return;
    }

    if (optionLen == 0) {
      return;
    }
    offset += optionLen;
  }
}

// **************************************************************
// Updates the route for 'ping'
// **************************************************************
void OutIPMsg::updateRecordRoute(char* optionsBase,
                                 unsigned int optionsLen,
                                 unsigned int offset)
{
  unsigned int length = optionsBase[offset + 1];
  unsigned int pointer = optionsBase[offset + 2] - 1;
  uint32_t* replyBase = (uint32_t*)(optionsBase + offset + pointer);

  if (length + offset > optionsLen) {
    // this one is a message error
    return;
  }
  if (pointer + 4 > length) {
    // list is full
    return;
  }

  // append my IP address and let 'pointer' point to next free entry.
  uint32_t myIP = SystemManagement::getCommunicationCenter().getIPAddr();
  *replyBase = myIP;
  optionsBase[offset + 2] += 4;
}

// **************************************************************
// Updates the timestamp for 'ping'
// **************************************************************
void OutIPMsg::updateInternetTimestamp(char* optionsBase,
                                       unsigned int optionsLen,
                                       unsigned int offset)
{
  unsigned int length = optionsBase[offset + 1];
  unsigned int pointer = optionsBase[offset + 2] - 1;
  uint32_t* replyBase = (uint32_t*)(optionsBase + offset + pointer);
  char flags = optionsBase[offset + 3];
  char len = ((flags & 0x0F) == 0) ? 4 : 8;

  if (length + offset > optionsLen) {
    // this one is a message error
    return;
  }
  if (pointer + len > length) {
    // list is full
    if ((flags & 0xF0) != 0xF0) {
      // update the overflow count
      optionsBase[offset + 3] += 0x10;
    }
    return;
  }

  if ((flags & 0x0F) != 0) {
    // append my IP address and let 'pointer' point to next free entry.
    uint32_t myIP = SystemManagement::getCommunicationCenter().getIPAddr();
    *replyBase = myIP;
    replyBase++;
  }

  uint32_t time = getTimerMilliSecs(0);
  time |= 0x80000000;                   // set high bit, as this is not universal time.
  *replyBase = htonl(time);

  optionsBase[offset + 2] += len;
}

// **************************************************************
// Returns a unique IP identification nubmer.
// **************************************************************
uint16_t OutIPMsg::getIdentification()
{
  if (mIdentification == 0xFFFF) {
    mIdentification = 0;
  }
  mIdentification++;
  return mIdentification;
}

// **********************************************************************
// **********************************************************************
// **************************   IP Frame    *****************************
// **********************************************************************
// **********************************************************************

// **************************************************************
// IP frame constructor.
// **************************************************************
IPFrame::IPFrame(InFrame& inFrame, Arp& arp) :
  IPHeader(inFrame, arp),
  mBase(inFrame.getCurDataPtr()),
  mOffset(0)
{
}
