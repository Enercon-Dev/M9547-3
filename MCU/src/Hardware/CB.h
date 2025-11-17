#ifndef _CB_H_
#define _CB_H_

#include "I2C.h"
#include "constants.h"
#include "stm32f10x.h"
#include "misc.h"
//#include "Timing.h"
#include "string.h"
#include "globalTypes.h"
#include "persistentSystemData.h"

#define CB_BUFFER_LENGTH (20)

/* Switch Pins&Ports declarations */
#define SW_ON1_Pin GPIO_Pin_0
#define SW_ON1_Port GPIOE
#define SW_ON2_Pin GPIO_Pin_1
#define SW_ON2_Port GPIOE
#define SW_ON3_Pin GPIO_Pin_2
#define SW_ON3_Port GPIOE
#define SW_ON4_Pin GPIO_Pin_3
#define SW_ON4_Port GPIOE
#define SW_ON5_Pin GPIO_Pin_4
#define SW_ON5_Port GPIOE
#define SW_ON6_Pin GPIO_Pin_5
#define SW_ON6_Port GPIOE
#define SW_ON7_Pin GPIO_Pin_6
#define SW_ON7_Port GPIOE
#define SW_ON8_Pin GPIO_Pin_7
#define SW_ON8_Port GPIOE
#define SW_ON9_Pin GPIO_Pin_8
#define SW_ON9_Port GPIOE
#define SW_ON10_Pin GPIO_Pin_9
#define SW_ON10_Port GPIOE
#define SW_ON11_Pin GPIO_Pin_10
#define SW_ON11_Port GPIOE
#define SW_ON12_Pin GPIO_Pin_11
#define SW_ON12_Port GPIOE
#define SW_ON13_Pin GPIO_Pin_12
#define SW_ON13_Port GPIOE
#define SW_ON14_Pin GPIO_Pin_13
#define SW_ON14_Port GPIOE
#define SW_ON15_Pin GPIO_Pin_14
#define SW_ON15_Port GPIOE
#define SW_ON16_Pin GPIO_Pin_15
#define SW_ON16_Port GPIOE

const uint16_t Termal_Max = 5000;
const uint16_t current_limit = 125*16;
const uint8_t IndexToAddress[NUM_OF_CB]        = {0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
const uint8_t IndexToCB[NUM_OF_CB]             = {1 ,2 ,3 ,4 ,5 ,6 ,7 ,8 ,9 ,10 ,11 ,12 ,13 ,14 ,15, 16 };        /* According to the active wires in each switch */
GPIO_TypeDef* const SwOnPort[NUM_OF_CB]        = { SW_ON1_Port, SW_ON2_Port, SW_ON3_Port, SW_ON4_Port, SW_ON5_Port, SW_ON6_Port, SW_ON7_Port ,SW_ON8_Port,SW_ON9_Port,SW_ON10_Port,SW_ON11_Port,SW_ON12_Port,SW_ON13_Port,SW_ON14_Port,SW_ON15_Port,SW_ON16_Port};
const uint16_t SwOnPin[NUM_OF_CB]              = { SW_ON1_Pin, SW_ON2_Pin, SW_ON3_Pin, SW_ON4_Pin, SW_ON5_Pin, SW_ON6_Pin, SW_ON7_Pin, SW_ON8_Pin, SW_ON9_Pin, SW_ON10_Pin, SW_ON11_Pin, SW_ON12_Pin, SW_ON13_Pin, SW_ON14_Pin, SW_ON15_Pin, SW_ON16_Pin};

const uint16_t output_max_overload_current[NUM_OF_CB]    = {(uint16_t)(30*16), (uint16_t)(30*16), (uint16_t)(30*16), (uint16_t)(30*16), 
                                                            (uint16_t)(15*16), (uint16_t)(15*16), (uint16_t)(15*16), (uint16_t)(15*16), 
                                                            (uint16_t)(15*16), (uint16_t)(15*16), (uint16_t)(15*16), (uint16_t)(15*16), 
                                                            (uint16_t)(30*16), (uint16_t)(30*16), (uint16_t)(30*16), (uint16_t)(30*16)};

#pragma pack(push)
#pragma pack(1)
typedef enum{CB_CONTROL_DIS =0, CB_CONTROL_EN= 1, CB_CONTROL_TRIPRST = 2}CB_En_t;
#pragma pack(pop)

#pragma pack(push)
#pragma pack(1)
typedef struct
{
  CB_En_t       EN:2;
  uint8_t       Clear_Reset : 2;
  uint8_t	Battel_Override :4;
}CB_Control_t;
#pragma pack(pop)

#pragma pack(push)
#pragma pack(1)
typedef struct
{
  uint16_t	Over_Load_Limit;
  uint32_t	I2T_Limit :24;
  uint16_t	Short_Circuit_Limit;

}CB_Config_t;
#pragma pack(pop)

typedef union {
  struct {
    uint8_t  RST       : 1;
    uint8_t  SW        : 1;
    uint8_t  TRIP      : 1;
    uint8_t  ERR_Short : 1;
    uint8_t  Alive     : 4;
  };
  uint8_t    ToInt;
}CB_State_t, CB_Status_t;

typedef struct {
  uint16_t     Meas_Current;
  uint16_t     Meas_Voltage_Drop;
  uint16_t     Meas_Voltage;
  uint16_t     Meas_Temperature;
  CB_State_t   Meas_Status;
  uint32_t     Meas_I2T; 
  CB_Control_t Control;
  CB_Config_t  Config;
  uint16_t     Thermal_Const;
  uint8_t      GroupNum;
}CB_Data;

typedef struct {
  uint16_t     Meas_Current;
  uint16_t     Meas_Voltage;
  uint16_t     Meas_Temperature;
  CB_State_t   Meas_Status;
  uint32_t     Meas_I2T; 
  CB_Control_t Control;
  CB_Config_t  Config;
  uint16_t     Thermal_Const;
}Group_Data;

#pragma pack(push)
#pragma pack(1)
typedef struct
{
  CB_Status_t Status;
  uint16_t    Current;
  uint16_t    Voltage;
  uint16_t    Temperature;
  uint32_t    I2T         : 24;
  uint8_t     CONTROL;
  uint16_t    OLCL;
  uint32_t    I2T_LIM     : 24;
  uint16_t    SCCL;
  
}CB_Full_Status_t;
#pragma pack(pop)

typedef enum {
  Periodic_Begin   = 0   ,
  Periodic_Status        ,
  Periodic_Config        , 
  Periodic_Control       ,
  Periodic_Group_Handling,
}Periodic_States_t;

class CB_Int {
public:
  
  static CB_Int*	Select(uint8_t Index);
  static CB_Int*        SelectOutput(uint8_t output);
  static ErrorStatus Init();
  ErrorStatus SetConfig(uint16_t overLoadSetting, uint16_t shortCircuitSetting, uint16_t thermalConstSetting); // Sets the selected CB Thermal_Const & Config
  ErrorStatus Control(CB_En_t enSetting, uint8_t battleOverrideSetting); // Sets the selected CB Control
  
  static FunctionalState Hold;
  static void shutdown_CBs(FunctionalState input);
  
  void Power(FunctionalState En);
  
  ErrorStatus setGroup(uint8_t group_number);
  
  static void periodicCall(uint16_t InVoltage);
  uint16_t Get_CB_Current();
  uint16_t Get_CB_Voltage();
  uint16_t Get_CB_Temperature();
  ErrorStatus Get_Config(Raw_Config_t *pConf);
  
  static uint8_t getGroupOfSpecificCB(uint8_t cb);
  
  /*---------------------------------------------*/
  /* Conroller Interface : PDU<->SERVER messages */
  /*---------------------------------------------*/
  /* 4-4.1.1 GET_STATUS */
  /* Some of the functions are implemented here and some of the functions are implemented at HardwareCenter */
  static uint_cb HandleGetStatusSWState (void);
  static uint_cb HandleGetStatusSWCmd   (void);
  static void    HandleGetStatusTripStat(uint8_t* trip_stat);
  static uint_cb HandleGetStatusBtlMode (void);
  static void    HandleGetStatusOutVolt (int16_t* out_volt); 
  static void    HandleGetStatusOutCurr (int16_t* out_cur);
  static void    HandleGetStatusChannelGroup(uint8_t* channel_groups); // TODO: Implement
  /* 4-4.1.2 GET_PDU_INFO */ 
  void HandleGetPDUInfo(void);      // TODO: Implement
    
  /* 4-4.2.1 SET_OUTPUT */
  void           HandleSetSingleOutput(CB_En_t val);
  void           HandleSetGroupOutput (CB_En_t val);
  /* 4-4.2.2 TRIP_RESET */
  void           HandleSingleTripReset(void);
  void           HandleGroupTripReset (void);
  /* 4-4.2.3 RESET */
  void           HandleReset          (void); // TODO: Implement
  /* 4-4.2.1 SET_BATTLE */
  void           HandleSetSingleBattle(uint8_t val);
  void           HandleSetGroupBattle (uint8_t val);
  /* 4-4.3.1 SET_LIMIT */
  ErrorStatus           HandleSetGroupLimits (uint16_t ThrmConst, uint16_t Overload, uint16_t CurrLimit);
  ErrorStatus           HandleSetSingleLimits(uint16_t ThrmConst, uint16_t Overload, uint16_t CurrLimit);
  /* 4-4.3.2 SET_GROUP */
  static ErrorStatus    handleSetGroup(uint8_t group, uint8_t* ChannelMask);
private:
  CB_Int() {};					// Private so that it can  not be called
  CB_Int(CB_Int const&){};             	// copy constructor is private
  CB_Int& operator=(CB_Int const&){return *this;}; 	// assignment operator is private
  static CB_Int* m_pInstance;
  static I2C_DRV *I2C;
  static uint8_t CB_Index;
  static ErrorStatus GPIO_INIT(void);
  static Periodic_States_t periodic_state;
  inline static BusyStatus IsBusy(){
    return I2C->IsBusy();
  };
  ErrorStatus Config_Write(const CB_Config_t* cmd);
  ErrorStatus Full_Status_Request();
  ErrorStatus Full_Status_Read(CB_Full_Status_t* cmd) const;
  ErrorStatus Control_Write(const CB_Control_t* cmd);

  ErrorStatus Write(uint8_t op, const void* cmd, uint8_t len);
  
  uint8_t OutputBuffer[CB_BUFFER_LENGTH]  = {0};
  uint8_t InputBuffer [CB_BUFFER_LENGTH]  = {0};
  
  void Update_CB_Configuration(Raw_Config_t *pConfig);
  
  /* Groups interface */
  static void addOutIntoGroup       (uint8_t group);
  static void addChannelIntoGroup   (uint8_t channel, uint8_t group);
  static void reorderGroups         (uint8_t changed_index);
  static void removeOutFromGroup    ();
  static void removeChannelFromGroup(uint8_t channel);
  static void transferOutToGroup    (uint8_t new_group);
  static void transferChannelToGroup(uint8_t channel, uint8_t new_group);
  static void firstAndLastPosOfGroup(uint8_t group, uint8_t* first, uint8_t* last);
  static uint8_t switchesInGroupsMask(uint8_t group);
  
  static CB_Data CBs[NUM_OF_CB];
  static Group_Data Groups[NUM_OF_GROUPS];
  static uint8_t CB_Groups_Int[NUM_OF_CB];
  static uint_cb ConfigMask;
  static uint_cb EnMask;
  
};

#endif //_CB_H_
