// **********************************************************************
// **********************************************************************
// Copyright 2011 by Avi Owshanko. All rights reserved.
// **********************************************************************
// **********************************************************************

#ifndef PERSISTENT_ETHERNET_SETTINGS_H
#define PERSISTENT_ETHERNET_SETTINGS_H

#include "persistentObject.h"
#include "globalTypes.h"

// opcodes
#define PERSISTENT_ETHERNET_PORT_OPCODE             (0x01 | SINGLE_DATA_SINGLE_WORD_MASK)

// **************************************************************
// The ethernet settings persistent object
// **************************************************************
class PersistentEthernetSettings : public PersistentObject
{
public:
  PersistentEthernetSettings();

  void updatePortSettings(SwitchPortTypes port, SwitchPortConnectionType);
  SwitchPortConnectionType getPortSettings(SwitchPortTypes port) const;

protected:
  virtual void writeFullData();
  virtual void processNextEntry(const PersistentSingleData&);

private:
  void readPortData(const PersistentSingleData&);
  void writePortData(SwitchPortTypes port, bool bSingle);

  SwitchPortConnectionType mPortSettings[SWITCH_PORT_COUNT];
};


#endif // PERSISTENT_ETHERNET_SETTINGS_H