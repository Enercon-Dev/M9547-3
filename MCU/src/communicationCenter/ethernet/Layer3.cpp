// **********************************************************************
// **********************************************************************
// Copyright 2011 by Avi Owshanko. All rights reserved.
// **********************************************************************
// **********************************************************************

#include "layer3.h"
#include "ipFrame.h"
#include "ipLayer.h"
#include "systemManagement.h"
#include "../../persistentData/persistentDataCenter.h"
#include "globalInterface.h"
#include "../communicationCenter.h"
#include "stm32Interface.h"
#include <string.h>

// **************************************************************
// Calculates the virtual layer3 header CRC.
// fits both incoming and outbound messages, udp and tcp.
// **************************************************************
uint32_t Layer3Connection::calcBaseChecksum(uint32_t srcIP, uint32_t dstIP, bool isTCP)
{
 
  char dummy[10];
  *(uint32_t*)dummy = dstIP;
  *(uint32_t*)(dummy + 4) = srcIP;
  dummy[8] = 0;
  if (isTCP) {
    // tcp identifier
    dummy[9] = 0x06;
  }
  else {
    // udp identifier
    dummy[9] = 0x11;
  }

  uint32_t checksum = calcChecksum(dummy, 10, 0, 0);
  checksum = ~checksum;
  checksum = ntohs(checksum);
  return checksum;

}

// **********************************************************************
// **********************************************************************
// *********************   Layer 3 generic class    *********************
// **********************************************************************
// **********************************************************************

// **************************************************************
// Constructor
// **************************************************************
Layer3::Layer3(CommunicationCenter& communicationCenter) :
  mPeriodicIndex(0),
  mCommunicationCenter(communicationCenter)
{
  int i;
  for (i=0; i<PORT_TYPES_COUNT; i++) {
    mListener[i].mSocket = 0;
    mListener[i].mConnection = NULL;
  }
}

// **************************************************************
// Add a new listener to layer3.
// will return 'false' on failure
// **************************************************************
bool Layer3::addListener(uint16_t socket, Layer3Connection* connection)
{
  if ((socket == 0) || (connection == NULL)) {
    return false;
  }

  int i;
  for (i=0; i<PORT_TYPES_COUNT; i++) {
    if (mListener[i].mSocket == 0)  {
      mListener[i].mSocket = socket;
      mListener[i].mConnection = connection;
      initArpData(mListener[i]);
      return true;
    }
  }

  return false;
}

// **************************************************************
// Removes an existing listener from layer3.
// will return 'false' on failure
// **************************************************************
bool Layer3::removeListener(uint16_t socket)
{
  if (socket == 0) {
    return false;
  }

  int i;
  for (i=0; i<PORT_TYPES_COUNT; i++) {
    if (mListener[i].mSocket == socket)  {

      if ((mListener[i].mConnection != NULL) &&
          (!mListener[i].mConnection->isClosed()))
      {
        // connection must be closed before removing it.
        return false;
      }

      mListener[i].mSocket = 0;
      mListener[i].mConnection = NULL;
      return true;
    }
  }

  return false;
}


// **************************************************************
// This method handles incoming messages.
// manages the connections, and routes packets to relevant destination.
// **************************************************************
int Layer3::handleFrame(const IPFrame& ipFrame)
{
  unsigned int msgLen = ipFrame.getMsgLen();
  const char* msgBase = ipFrame.getCurDataPtr();
  if (msgLen < 4) {
    return -1;
  }

  uint16_t targetSocket = *(uint16_t*)(msgBase+2);
  int targetPortIndex = getSocketIndex(targetSocket);

  if (targetPortIndex < PORT_TYPES_COUNT) {
    Layer3Connection& connection = *(mListener[targetPortIndex].mConnection);
    return connection.handleIncomingMsg(ipFrame.getSrcIP(), ipFrame.getDstIP(),
                                        (const unsigned char*)msgBase,
                                         msgLen);
  }

  return -2;
}

// **************************************************************
// activates periodic call for all connections
// handles one connection each time it is invoked.
// **************************************************************
void Layer3::periodicCall()
{
  if (mListener[mPeriodicIndex].mConnection != NULL) {
    Layer3Connection& connection = *(mListener[mPeriodicIndex].mConnection);

    connection.periodicCall();
    //refreshArpEntry(mListener[mPeriodicIndex]);
  }

  mPeriodicIndex = (mPeriodicIndex + 1) % PORT_TYPES_COUNT;
}

// **************************************************************
// Checks whether the TCP layer has a connection for given port
// **************************************************************
bool Layer3::hasConnection(uint16_t targetSocket)
{
  return (getSocketIndex(targetSocket) < PORT_TYPES_COUNT);
}

// **************************************************************
// Returns 'targetPort' index in 'mConnections'.
// Returns 'PORT_TYPES_COUNT' if not found.
// **************************************************************
int Layer3::getSocketIndex(uint16_t targetSocket)
{
  if (targetSocket == 0) {
    return PORT_TYPES_COUNT;
  }

  int i;
  for (i=0; i<PORT_TYPES_COUNT; i++) {
    if (mListener[i].mSocket == targetSocket) {
      break;
    }
  }

  return i;
}

// **************************************************************
// refreshes ARP entry.
// verifies that the ARP will hold target (IP) entry
// **************************************************************
void Layer3::initArpData(Layer3SocketConnection& listener)
{
  listener.mArpLease = getTimerLease(0);
  listener.mArpTimeout = ARP_SEND_TIMEOUT;
}

// **************************************************************
// refreshes ARP entry.
// verifies that the ARP will hold target (IP) entry
// **************************************************************
/*void Layer3::refreshArpEntry(Layer3SocketConnection& listener)
{
  if (listener.mConnection == NULL) {
    return;
  }

  Layer3Connection& connection = *(listener.mConnection);
  if (connection.isClosed()) {
    // no need to refresh a closed connection...
    return;
  }

  uint32_t targetIP = connection.getOtherIP();
  if (targetIP == 0) {
    return;
  }

  if (getLeaseTime(listener.mArpLease) > 1) {
    return;
  }

  listener.mArpLease = getTimerLease(listener.mArpTimeout);
  if (listener.mArpTimeout < ARP_SEND_MAX_TIMEOUT) {
    // longest timeout - about 20 minutes.
    listener.mArpTimeout = 2*listener.mArpTimeout;
  }

  // following call will force ARP to fetch target IP.
  mCommunicationCenter.getMacAddr(targetIP);
}*/