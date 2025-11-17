// **********************************************************************
// **********************************************************************
// Copyright 2011 by Avi Owshanko. All rights reserved.
// **********************************************************************
// **********************************************************************
#include "general/globalTypes.h"
#include "constants.h"

#ifndef _HANDLERS_COMMON_H
#define _HANDLERS_COMMON_H

#define CONTROL_CLIENT_PORT     (0x8813)
#define CONTROL_SERVER_PORT     (0x8813)
#define CONTROL_TOPOLOGY_PORT   (0xBA13)
#define CONTROL_KEPPALIVE_PORT  (0x9213)
#define CONTROL_ALERT_PORT      (0x9C13)


typedef enum {ACK = 0, NACK = 1, EEPROM_ERROR = 2} COMMAND_SUCCESS;


typedef enum {
        CONTROL_OPCODE_GET_STATUS	= 0x74,
        CONTROL_OPCODE_STATUS		= 0x75,
        CONTROL_OPCODE_GET_PDU_INFO	= 0x76,
        CONTROL_OPCODE_PDU_INFO		= 0x77,
        CONTROL_OPCODE_SET_OUTPUT	= 0x80,
        CONTROL_OPCODE_OUTPUT_ACK	= 0x81,
        CONTROL_OPCODE_TRIP_RESET	= 0x82,
        CONTROL_OPCODE_TRIP_RESET_ACK	= 0x83,
        CONTROL_OPCODE_RESET		= 0x84,
        CONTROL_OPCODE_RESET_ACK	= 0x85,
        CONTROL_OPCODE_SET_BATTLE	= 0x86,
        CONTROL_OPCODE_BATTLE_ACK	= 0x87,
        CONTROL_OPCODE_SET_LIMIT	= 0x90,
        CONTROL_OPCODE_LIMIT_ACK    	= 0x91,
        CONTROL_OPCODE_SET_GROUP        = 0x92,
        CONTROL_OPCODE_GROUP_ACK        = 0x93,
        CONTROL_OPCODE_SET_POR_MACRO	= 0x94,
        CONTROL_OPCODE_POR_ACK		= 0x95,
        CONTROL_OPCODE_SET_DCI_MACRO    = 0x96,
        CONTROL_OPCODE_DCI_ACK          = 0x97,
        //CONTROL_OPCODE_CALIBRATE	= 0x96,
        //CONTROL_OPCODE_CALIBRATE_ACK	= 0x97,
        CONTROL_OPCODE_SET_IP_MODE      = 0x98,
        CONTROL_OPCODE_IP_MODE_ACK      = 0x99,
        CONTROL_OPCODE_GET_CONFIG       = 0x9A,
        CONTROL_OPCODE_CONFIG           = 0x9B,
        CONTROL_OPCODE_ERASE_DATA	= 0x9C,
        CONTROL_OPCODE_ERASE_ACK	= 0x9D,
        CONTROL_OPCODE_KEEPALIVE	= 0x5A,
        CONTROL_OPCODE_SET_DEBUG	= 0xB0,
        CONTROL_OPCODE_DEBUG_ACK	= 0xB1,
        CONTROL_OPCODE_GET_SW_INFO	= 0xCA,
        CONTROL_OPCODE_SW_INFO		= 0xCB,
        CONTROL_OPCODE_SET_SW_UPDATE	= 0xCC,
        CONTROL_OPCODE_SW_UPDATE_ACK	= 0xCD,
        //CONTROL_OPCODE_GET_FW_INFO	= 0xDA,
        //CONTROL_OPCODE_FW_INFO	= 0xDB,
        //CONTROL_OPCODE_SET_FW_UPDATE	= 0xDC,
        //CONTROL_OPCODE_FW_UPDATE_ACK	= 0xDD,
        CONTROL_OPCODE_ERR              = 0xFF
} CONTROL_OPCODE;

#pragma pack(push)
#pragma pack(1)
typedef struct {
  const uint16_t        Length  = ( ( (4 * CB_BYTE) + (6 * NUM_OF_CB) + 7 + 3) << 8);// 3 -> 1 for opcode and 2 for length.
  uint_cb               SW_STATE;
  uint_cb               SW_CMD;
  uint8_t               TRIP_STAT[NUM_OF_CB];
  uint_cb               BTL_MODE;
  uint_cb               BIT;
  int16_t               OUT_VOLT[NUM_OF_CB];
  int16_t               OUT_CURR[NUM_OF_CB];
  uint8_t               CHANNEL_GROUP[NUM_OF_CB] ;
  int16_t		INPUT_VOLT;
  int16_t		INPUT_CURR;
  int16_t		TEMP;
  uint8_t		ERROR_STAT;
    
} Status_Msg_T;
#pragma pack(pop)

#pragma pack(push)
#pragma pack(1)
typedef struct {
  const uint16_t        Length  = (27 << 8);
  IpSettings_t          IP_SETTINGS;
  Raw_Config_t          CHANNEL_CONFIG[NUM_OF_CB];
  Por_Order_t           POR_ORDER[NUM_OF_CB];
  DCI_Order_t           DCI_ORDER[NUM_OF_CB]; 
} Config_Msg_T;        
#pragma pack(pop)

#endif // _HANDLERS_COMMON_H
