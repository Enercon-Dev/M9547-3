// **********************************************************************
// **********************************************************************
// Copyright 2011 by Avi Owshanko. All rights reserved.
// **********************************************************************
// **********************************************************************

#include "persistentEthernetSettings.h"
#include "persistentData.h"

// **********************************************************************
// **********************************************************************
// *****************   Persistent Ethernet Settings     *****************
// **********************************************************************
// **********************************************************************

// **************************************************************
// constructor
// **************************************************************
PersistentEthernetSettings::PersistentEthernetSettings() :
  PersistentObject(PERSISTENT_ETHERNET_SETTINGS, PERSISTENT_OBJECT_VERSION)
{
  // default is auto negotiation
  uint8_t i;
  for (i=0; i < SWITCH_PORT_COUNT; i++) {
    mPortSettings[i] = SWITCH_CONNECTION_AUTO_NEG;
  }
}

// **************************************************************
// pure virtual method implementation
// writes the full data at the head of a page
// **************************************************************
void PersistentEthernetSettings::writeFullData()
{
  resetFirstCopy();

  uint8_t i;
  for (i=0; i < SWITCH_PORT_COUNT; i++) {
    writePortData((SwitchPortTypes)i, true);
  }

  resetOtherCopies();
}

// **************************************************************
// reads next persistent entry
// **************************************************************
void PersistentEthernetSettings::processNextEntry(const PersistentSingleData& data)
{
  switch (data.opcode()) {
  case PERSISTENT_ETHERNET_PORT_OPCODE:
    readPortData(data);
    break;
  default:
    break;
  }
}

// **************************************************************
// decodes a 3 long words as an IP data
// **************************************************************
void PersistentEthernetSettings::readPortData(const PersistentSingleData& data)
{
  uint16_t info = data.singleData();
  uint8_t portIndex = info >> 8;
  SwitchPortConnectionType portSettings = SwitchPortConnectionType(info & 0x00FF);

  if (portIndex < SWITCH_PORT_COUNT) {
    mPortSettings[portIndex] = portSettings;
  }
}

// **************************************************************
// writes a single entry holding 3 long words representing the IP data
// **************************************************************
void PersistentEthernetSettings::writePortData(SwitchPortTypes port, bool bSingle)
{
  uint8_t portSettings  = (uint8_t)(mPortSettings[(uint8_t)port]);
  uint16_t info = (uint8_t)port;
  info = info << 8;
  info += portSettings;

  PersistentSingleData dataObject(PERSISTENT_ETHERNET_PORT_OPCODE,        // opcode
                                  info);                                  // base

  if (bSingle) {
    writeFirstCopyDataObject(dataObject);
  }
  else {
    writeDataObject(dataObject);
  }
}

// **************************************************************
// updates the ethernet port settings
// **************************************************************
void PersistentEthernetSettings::updatePortSettings(SwitchPortTypes port,
                                                    SwitchPortConnectionType newType)
{
  if (((uint8_t)port) >= SWITCH_PORT_COUNT) {
    return;
  }

  if (newType == mPortSettings[(uint8_t)port]) {
    return;
  }

  mPortSettings[(uint8_t)port] = newType;

  writePortData(port, false);
}

// **************************************************************
// queries the ethernet port settings
// **************************************************************
SwitchPortConnectionType PersistentEthernetSettings::getPortSettings(SwitchPortTypes port) const
{
  if (((uint8_t)port) >= SWITCH_PORT_COUNT) {
    return SWITCH_CONNECTION_UNKNOWN;
  }

  return mPortSettings[(uint8_t)port];
}