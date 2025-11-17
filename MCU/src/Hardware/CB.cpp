#include "CB.h"
#include "stm32Interface.h"
#include "Global.h"
#include "systemManagement.h"
#include "constants.h"

const float Out_Curr_Calib_Factor = 1.017852688172F; // YakirZ - calculated from a current delta between current measurements from  SHANT and ADC, SHAMT = 29.993A, ADC = 29.0625 --> Ratio = factor

//const float Out_Curr_Calib_Factor = 1.029952688172F; // YakirZ - calculated from a current delta between current measurements from  SHANT and ADC, SHAMT = 29.993A, ADC = 29.0625 --> Ratio = factor
/* ---------------------- */
/* Static initializations */
/* ---------------------- */
CB_Int* CB_Int::m_pInstance               = NULL;
I2C_DRV* CB_Int::I2C                      = NULL;
uint8_t CB_Int::CB_Index                  = 0;
CB_Data CB_Int::CBs[NUM_OF_CB]           = {0};
Group_Data CB_Int::Groups[NUM_OF_GROUPS]  = {0};
uint8_t CB_Int::CB_Groups_Int[NUM_OF_CB] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
Periodic_States_t CB_Int::periodic_state  = Periodic_States_t::Periodic_Begin;
uint_cb CB_Int::ConfigMask = 0;
uint_cb CB_Int::EnMask = 0;
FunctionalState CB_Int::Hold = FunctionalState::DISABLE;


/* --------------- */
/*   CB - Groups   */
/* --------------- */

/* Function receives a grouped-switch, and add its id into the groups array */
ErrorStatus CB_Int::setGroup(uint8_t group_number)
{
  if(group_number == (uint8_t)-1)
  {
    CBs[CB_Index].Control.Battel_Override = Groups[CBs[CB_Index].GroupNum].Control.Battel_Override;
    CBs[CB_Index].GroupNum = -1;
  }
  else
  {
    CBs[CB_Index].Control.Battel_Override = 0xD;
    int i;
    for(i = 0; i < NUM_OF_CB && CBs[i].GroupNum != group_number; ++i);
    CBs[CB_Index].GroupNum = group_number;
    if (i <NUM_OF_CB)
    {
      CBs[CB_Index].Config = CBs[i].Config;
      CBs[CB_Index].Control = CBs[i].Control;
    }
  }
  
  reorderGroups(CB_Index);
  
  return CB_Int::Select(CB_Index)->HandleSetSingleLimits(CBs[CB_Index].Thermal_Const, CBs[CB_Index].Config.Over_Load_Limit, CBs[CB_Index].Config.Short_Circuit_Limit);
}

/* Function receives the last inserted index of groups array, and reordes it such that the switches are sorted by their grouping number */
void CB_Int::reorderGroups(uint8_t changed_index) {
  int i;
  uint8_t group_num = CBs[changed_index].GroupNum;
  for(i = 0; i < NUM_OF_CB && CB_Groups_Int[i] != changed_index; ++i);
  for(int k = i; k < NUM_OF_CB; ++k) {
    CB_Groups_Int[k] = CB_Groups_Int[k + 1];
  }
  for(i = 0; i < (NUM_OF_CB-1) && CBs[CB_Groups_Int[i]].GroupNum < group_num; ++i);
  for(int k = NUM_OF_CB - 1; k > i; --k) {
    CB_Groups_Int[k] = CB_Groups_Int[k - 1];
  }
  CB_Groups_Int[i] = changed_index;
}


/* Function receives a group number and stores the first and last occurrences in the groups array */
void CB_Int::firstAndLastPosOfGroup(uint8_t group, uint8_t* first, uint8_t* last) {
  *first = *last = 255;
  for (uint8_t i = 0; i < NUM_OF_CB; ++i) {
    if(CBs[CB_Groups_Int[i]].GroupNum == group) {
      if(*first == 255) {
        *first = i;
      } 
      *last=i;
    }
  }
}
/*
uint8_t CB_Int::switchesInGroupsMask(uint8_t group) 
{
  uint8_t grouped_switches_mask = 0;
  for (int i = 0; i < NUM_OF_CB; ++i)
  {
    if(CB_Groups_Int[i] == (uint8_t)-1 || CB_Groups_Int[i] != group)
      continue;
    else
      grouped_switches_mask |= (1 << i);
  }
  return grouped_switches_mask;  
  
}
*/
CB_Int* CB_Int::Select(uint8_t Index){
  if (!m_pInstance)   // Only allow one instance of class to be generated
    m_pInstance = new CB_Int;
  CB_Index = Index;
  return m_pInstance;
}

CB_Int* CB_Int::SelectOutput(uint8_t output){
  if (!m_pInstance)   // Only allow one instance of class to be generated
    m_pInstance = new CB_Int;
  for(int i = 0; i < NUM_OF_CB; ++i)
  {
    if(IndexToCB[i] == output)
    {
      CB_Index = i;
      return m_pInstance;    
    }
  }
  CB_Index = 0;
  return m_pInstance;
}

ErrorStatus CB_Int::Init()
{
  
  int rc = SUCCESS;
  I2C = I2C_DRV::Get_Instance();
  if (I2C == NULL)
    rc &= ERROR;
  rc &= GPIO_INIT();
  Raw_Config_t Data;
  for (int i=0; i< NUM_OF_CB; i++)
  {
    rc &= SystemManagement::getPersistentDataCenter().Get_CBSetting(i,&Data);
    CB_Int::Select(i)->Update_CB_Configuration(&Data);
    reorderGroups(i);
  }
  
  int8_t now = (int8_t)(getTimerTicks() & 0xFF);
  int8_t next = (int8_t)(((uint8_t)now + 20) & 0xFF);
  while ( (int8_t)(next - now) >=0)
    now = (int8_t)(getTimerTicks() & 0xFF);
  
  return (ErrorStatus)rc;
}

/* Sets the selected CB Thermal_Const & Config */
ErrorStatus CB_Int::SetConfig(uint16_t overLoadSetting, uint16_t shortCircuitSetting, uint16_t thermalConstSetting) { 
  CBs[CB_Int::CB_Index].Thermal_Const              = thermalConstSetting;
  CBs[CB_Int::CB_Index].Config.Over_Load_Limit     = overLoadSetting;
  CBs[CB_Int::CB_Index].Config.Short_Circuit_Limit = shortCircuitSetting;
  CBs[CB_Int::CB_Index].Config.I2T_Limit           = overLoadSetting * overLoadSetting * thermalConstSetting * 16 /1000; // formula to calculate i2t
  return ErrorStatus::SUCCESS;
}

ErrorStatus CB_Int::Get_Config(Raw_Config_t *pConf)
{
  pConf->Current_Limit = CBs[CB_Int::CB_Index].Config.Short_Circuit_Limit;
  pConf->GroupNum = CBs[CB_Int::CB_Index].GroupNum;
  pConf->Over_Load_Limit = CBs[CB_Int::CB_Index].Config.Over_Load_Limit;
  pConf->Thermal_Const = CBs[CB_Int::CB_Index].Thermal_Const;
  return SUCCESS;
}

/* Function recieves a CB and return its current group */
uint8_t CB_Int::getGroupOfSpecificCB(uint8_t cb)
{
  return CBs[cb].GroupNum;
}

/* 4-4.1.1 GET_STATUS */
/*
Intent: Sub-function for the GET_STATUS message
Description: Reading for each physical state of the output switches
*/
uint_cb CB_Int::HandleGetStatusSWState()
{
  uint_cb state = 0;
  for (int i=0; i< NUM_OF_CB; ++i) {
    state |= ((CBs[i].Meas_Status.SW) << i);
  }
  return state;
}

/*
Intent: Sub-function for the GET_STATUS message
Description: Reading for last command or automatic setting for each of the output switches
*/
uint_cb CB_Int::HandleGetStatusSWCmd()
{
  uint_cb cmd = 0;
  for (int i=0; i< NUM_OF_CB; ++i) 
    cmd |= ((CBs[i].Control.EN & 0x01 ) << i);
  return cmd;
}

/*
Intent: Sub-function for the GET_STATUS message
Description: Trip status for each of the outputs
*/
void CB_Int::HandleGetStatusTripStat(uint8_t* trip_stat)
{
  for(int i = 0; i < NUM_OF_CB; ++i)
    if (CBs[i].GroupNum == 255)
      trip_stat[i] = CBs[i].Meas_Status.ERR_Short ? 0x2 : CBs[i].Meas_Status.TRIP; // short circuit is 1 bits, so if it is on, place 2 according to the opcode docs
    else
      trip_stat[i] = Groups[CBs[i].GroupNum -1].Meas_Status.ERR_Short ? 0x2 : Groups[CBs[i].GroupNum -1].Meas_Status.TRIP;
    
  return;
}

/*
Intent: Sub-function for the GET_STATUS message
Description: Battle mode status for each output.
*/
uint_cb CB_Int::HandleGetStatusBtlMode()
{
  uint_cb battle_mode = 0;
  for(int i = 0; i < NUM_OF_CB; ++i)
    if (CBs[i].GroupNum == 255)
      battle_mode |= ((CBs[i].Control.Battel_Override == 0xD) << i);
    else
      battle_mode |= ((Groups[CBs[i].GroupNum -1].Control.Battel_Override == 0xD) << i);
  return battle_mode;
}

/*
Intent: Sub-function for the GET_STATUS message
Description: Output voltage reading for each channel 
*/  
void CB_Int::HandleGetStatusOutVolt(int16_t* out_volt)
{
  for(int i = 0; i < NUM_OF_CB; ++i)
    out_volt[i] = CBs[i].Meas_Voltage;
  return;
}

/*
Intent: Sub-function for the GET_STATUS message
Description: Current reading for each output 
*/ 
void CB_Int::HandleGetStatusOutCurr(int16_t* out_cur)
{
  for(int i = 0; i < NUM_OF_CB; ++i)
    out_cur[i] = CBs[i].Meas_Current;
  return;
}

/*
Intent: Sub-function for the GET_STATUS message
Description: Each channel�s Group 
*/ 
void CB_Int::HandleGetStatusChannelGroup(uint8_t* channel_groups) { return;} // use getGroupOfSpecificCB

/*
Intent: Sub-function for the SET_OUTPUT message
Description: Details which specific output to activate/deactivate. 
*/ 
void CB_Int::HandleSetSingleOutput(CB_En_t val)
{
  CBs[CB_Index].Control.EN = val;
  return;
}

/*
Intent: Sub-function for the SET_OUTPUT message
Description: Details which specific outputs inside of a group to activate/deactivate. 
*/ 
void CB_Int::HandleSetGroupOutput(CB_En_t val)
{
  uint8_t min = 0, max = 0;
  CB_Int::firstAndLastPosOfGroup(CB_Index, &min, &max);
  if (min >= NUM_OF_CB)
    return;
  if (Groups[CB_Index-1].Meas_Status.TRIP == 1 && val == CB_En_t::CB_CONTROL_EN)
    return;
  for(uint8_t i = min; i <= max; ++i) 
    CBs[CB_Groups_Int[i]].Control.EN = val;
  return;
}

/*
Intent: Sub-function for the TRIP_RESET message
Description: Details which specific channel to reset. 
*/ 
void CB_Int::HandleSingleTripReset(void)
{
  CBs[CB_Index].Control.EN = CB_En_t::CB_CONTROL_TRIPRST;
  return;
}

/*
Intent: Sub-function for the TRIP_RESET message
Description: Details which specific channels inside of a group to reset.
*/ 
void CB_Int::HandleGroupTripReset(void)
{
  Groups[CB_Index-1].Meas_Status.TRIP = 0;
  Groups[CB_Index-1].Meas_Status.ERR_Short = 0;
  return;
}

/*
Intent: Sub-function for the SET_BATTLE message
Description: Details which specific channel to be affected, and actual required command for the selected output
*/ 
void CB_Int::HandleSetSingleBattle(uint8_t val)
{
  CBs[CB_Index].Control.Battel_Override = val;
  return ;
}

/*
Intent: Sub-function for the SET_BATTLE message
Description: Details which specific channels to be affected and the actual required command for each selected output
*/ 
void CB_Int::HandleSetGroupBattle(uint8_t val)
{
  Groups[CB_Index-1].Control.Battel_Override = val;
  return;
}

/*
Intent: Sub-function for the SET_LIMIT message
Description: Set overload limit, current limit and thermal constant for selected group.
*/ 
ErrorStatus CB_Int::HandleSetGroupLimits(uint16_t ThrmConst, uint16_t Overload, uint16_t CurrLimit) { 
  uint8_t min = 0, max = 0;
  CB_Int::firstAndLastPosOfGroup(CB_Index, &min, &max);
  if (min >= NUM_OF_CB)
    return SUCCESS;
  uint8_t channels_in_group = max - min + 1;
  int rc = SUCCESS;
  for(uint8_t i = min; i <= max; ++i) 
    rc &= CB_Int::Select(CB_Groups_Int[i])->HandleSetSingleLimits(ThrmConst, (uint16_t)(Overload/channels_in_group), (uint16_t)(CurrLimit/channels_in_group));
  return (ErrorStatus)rc;
}

/*
Intent: Sub-function for the SET_LIMIT message
Description: Set overload limit, current limit and thermal constant for selected output.
*/ 
ErrorStatus CB_Int::HandleSetSingleLimits(uint16_t ThrmConst, uint16_t Overload, uint16_t CurrLimit) { 
  Raw_Config_t raw_conf;
  raw_conf.GroupNum        = CBs[CB_Int::CB_Index].GroupNum;
  raw_conf.Current_Limit   = (CurrLimit <current_limit )? CurrLimit : current_limit;
  raw_conf.Over_Load_Limit = (Overload > output_max_overload_current[CB_Int::CB_Index]) ? output_max_overload_current[CB_Int::CB_Index] : Overload;
  raw_conf.Thermal_Const   = ( ThrmConst<Termal_Max)? ThrmConst : Termal_Max;
  CBs[CB_Int::CB_Index].Config.Over_Load_Limit = raw_conf.Over_Load_Limit;
  CBs[CB_Int::CB_Index].Config.Short_Circuit_Limit = raw_conf.Current_Limit;
  CBs[CB_Int::CB_Index].Config.I2T_Limit = raw_conf.Over_Load_Limit * raw_conf.Over_Load_Limit * raw_conf.Thermal_Const * 16 /1000;
  
  return SystemManagement::getPersistentDataCenter().Set_CBSetting(CB_Int::CB_Index, &raw_conf);
  
}

/*
Intent: Sub-function for the SET_GROUP message
Description: Set outputs of groups to active/deactive
ChannelMask = 0 - not in group, 1 - in group
*/ 
ErrorStatus CB_Int::handleSetGroup(uint8_t group, uint8_t* ChannelMask)
{
  int i;
  int rc = SUCCESS;
  /* add all channels to the given group - the validation if the channel has already been assigned to a group is done at addChannelIntoGroup method */
  for(i = 0; i < NUM_OF_CB ; ++i)
    if (ChannelMask[i])
      rc &= CB_Int::Select(i)->setGroup(group);
    else if (CBs[i].GroupNum == group)
      rc &= CB_Int::Select(i)->setGroup(255);
    
    return (ErrorStatus)rc;
}

/* Sets the selected CB Control */
ErrorStatus CB_Int::Control(CB_En_t enSetting, uint8_t battleOverrideSetting) {
  CBs[CB_Int::CB_Index].Control.EN              = enSetting;
  CBs[CB_Int::CB_Index].Control.Battel_Override = battleOverrideSetting;
  CBs[CB_Int::CB_Index].Control.Clear_Reset     = 0;
  return ErrorStatus::SUCCESS;
}

/* This function should config specific switch according to the firmware */
void CB_Int::Update_CB_Configuration(Raw_Config_t *pConfig) {
  if(pConfig) {
    CBs[CB_Int::CB_Index].Config.Over_Load_Limit        = (pConfig->Over_Load_Limit > output_max_overload_current[CB_Int::CB_Index]) ? output_max_overload_current[CB_Int::CB_Index] : pConfig->Over_Load_Limit;
    CBs[CB_Int::CB_Index].Config.Short_Circuit_Limit    = pConfig->Current_Limit;
    CBs[CB_Int::CB_Index].Thermal_Const                 = pConfig->Thermal_Const;
    CBs[CB_Int::CB_Index].GroupNum                      = pConfig->GroupNum;
    CBs[CB_Int::CB_Index].Config.I2T_Limit              = (pConfig->Over_Load_Limit) * (pConfig->Over_Load_Limit) * (pConfig->Thermal_Const) * 16/1000;
    if (CBs[CB_Int::CB_Index].GroupNum != 255)
      CBs[CB_Int::CB_Index].Control.Battel_Override = 0xD;
  }
}

void CB_Int::shutdown_CBs(FunctionalState input)
{
  static uint32_t lastTurnOn = 0;
  uint32_t now = getTimerTicks();
  if(input == FunctionalState::ENABLE)
  {
    for(int i = 0; i < NUM_OF_CB; ++i)
    {
      CB_Int::Select(i)->Power(FunctionalState::DISABLE);
      CBs[i].Control.EN = CB_CONTROL_DIS;
    }
  }
  else if (now != lastTurnOn)
    for(int i = 0; i < NUM_OF_CB; ++i)
    {
      if(GPIO_ReadOutputDataBit(SwOnPort[i],SwOnPin[i]) == Bit_SET)
        continue;
      if (!CBs[i].Meas_Status.Alive)  //For RAT
      {
        CB_Int::Select(i)->Power(FunctionalState::ENABLE);
        now = lastTurnOn;
        break;
      }
    }
  return;
}


void CB_Int::periodicCall(uint16_t InVoltage)
{
  uint8_t min_pos_in_group = -1, max_pos_in_group = -1;
  static uint8_t sw_index = (uint8_t)0;

  switch(periodic_state) {
  case Periodic_States_t::Periodic_Begin:
    CB_Full_Status_t CB_Outputs;
    if (GPIO_ReadOutputDataBit(SwOnPort[sw_index],SwOnPin[sw_index]) && Select(sw_index)->Full_Status_Request() == ERROR) {
      return;
    } else {
      periodic_state = Periodic_States_t::Periodic_Status;
    }
  case Periodic_States_t::Periodic_Status:
    if (IsBusy() == SET)
      return;
    if(GPIO_ReadOutputDataBit(SwOnPort[sw_index],SwOnPin[sw_index]) && Select(sw_index)->Full_Status_Read(&CB_Outputs) == SUCCESS) {
      CBs[sw_index].Meas_Current     = (uint16_t)(CB_Outputs.Current * Out_Curr_Calib_Factor);
      CBs[sw_index].Meas_Voltage     = InVoltage - CB_Outputs.Voltage;
      CBs[sw_index].Meas_Temperature = CB_Outputs.Temperature; 
      CBs[sw_index].Meas_Status      = CB_Outputs.Status;
      CBs[sw_index].Meas_I2T         = CB_Outputs.I2T;
      if ((CB_Outputs.CONTROL & 0xF3) == 0xD1){
        CBs[sw_index].Meas_Status.ERR_Short = (CBs[sw_index].Meas_Status.SW) ? 0 : 1;
      }
      CBs[sw_index].Meas_Status.Alive = 0xF;
    } 
    else
    {
      CBs[sw_index].Meas_Status.Alive -= (CBs[sw_index].Meas_Status.Alive) ? 1 : 0;
      if (!CBs[sw_index].Meas_Status.Alive)
      {
        CBs[sw_index].Meas_Current     = 0;
        CBs[sw_index].Meas_Voltage     = 0;
        CBs[sw_index].Meas_Temperature = 25*16; 
        CBs[sw_index].Meas_Status      = {0, 0, 0, 0};
      }
      sw_index = (sw_index + 1) % NUM_OF_CB;
      if (sw_index == 0)
        periodic_state = Periodic_States_t::Periodic_Group_Handling;
      else
        periodic_state = Periodic_States_t::Periodic_Begin;
      return;
    }
    
    if( memcmp( &CBs[sw_index].Config ,(CB_Config_t *)(&CB_Outputs.OLCL), sizeof(CB_Config_t)) )
      ConfigMask |= 1 << sw_index;
    else
      ConfigMask &= ~(1 << sw_index);
    {
      CB_Control_t cnt = CBs[sw_index].Control;
      if (cnt.Battel_Override != 0xD && (Hold==ENABLE))
        cnt.EN = (CB_En_t)( (uint8_t)cnt.EN & 0xFE);
      if (*(uint8_t *)(&cnt) != CB_Outputs.CONTROL)
        EnMask |= 1 << sw_index;
      else
        EnMask &= ~(1 << sw_index);
    }
    sw_index = (sw_index + 1) % NUM_OF_CB;
    if (sw_index == 0)
      periodic_state = Periodic_States_t::Periodic_Group_Handling;
    else
    {
      periodic_state = Periodic_States_t::Periodic_Begin;
      return;
    }
    
  case Periodic_States_t::Periodic_Group_Handling:

    for (int gr=0; gr<NUM_OF_GROUPS; gr++)
    {
      //Groups[gr].Meas_Status.ToInt = 0;
      CB_Int::firstAndLastPosOfGroup(gr+1, &min_pos_in_group, &max_pos_in_group);
      if (min_pos_in_group == 255)
        continue;
      Groups[gr].Meas_I2T = 0;
      Groups[gr].Config.I2T_Limit = 0;
      uint8_t Short =0;
      for(int j = min_pos_in_group; j <= max_pos_in_group; ++j)
      {
        Groups[gr].Meas_I2T += CBs[CB_Groups_Int[j]].Meas_I2T;
        Groups[gr].Meas_Status.TRIP |= CBs[CB_Groups_Int[j]].Meas_Status.TRIP;
        Short |= CBs[CB_Groups_Int[j]].Meas_Status.ERR_Short;
        Groups[gr].Config.I2T_Limit += CBs[CB_Groups_Int[j]].Config.I2T_Limit;     
      }
      if ((Groups[gr].Meas_I2T > Groups[gr].Config.I2T_Limit || Groups[gr].Meas_Status.TRIP || Short) && Groups[gr].Control.Battel_Override != 0xD)
      {
        Groups[gr].Meas_Status.TRIP = 1;
        Groups[gr].Meas_Status.ERR_Short |= Short;
        for(int j = min_pos_in_group; j <= max_pos_in_group; ++j)
        {
          CBs[CB_Groups_Int[j]].Control.EN = CB_CONTROL_DIS;
          EnMask |= 1 << CB_Groups_Int[j];
        }
      }
    }
    
    periodic_state = Periodic_States_t::Periodic_Config;
    
  case Periodic_States_t::Periodic_Config:
    if (IsBusy() == BusyStatus::SET)
      return;
    for (; sw_index<NUM_OF_CB; sw_index++)
    {
      if (ConfigMask & (1 << sw_index))
      {
        Select(sw_index)->Config_Write(&CBs[sw_index].Config);
        sw_index = sw_index + 1;
        return;
      }
    }
    sw_index =0;
    periodic_state = Periodic_States_t::Periodic_Control;
    
  case Periodic_States_t::Periodic_Control:    
    if (IsBusy() == BusyStatus::SET)
      return;
    for (; sw_index<NUM_OF_CB; sw_index++)
    {
      if (EnMask & (1 << sw_index))
      {
        CB_Control_t cnt = CBs[sw_index].Control;
        if (cnt.Battel_Override != 0xD && (Hold==ENABLE))
          cnt.EN = (CB_En_t)( (uint8_t)cnt.EN & 0xFE);
        Select(sw_index)->Control_Write(&cnt);
        sw_index = sw_index + 1;
        return;
      }
    }
    sw_index =0;
    periodic_state = Periodic_States_t::Periodic_Begin;
  }
}

ErrorStatus CB_Int::Write(uint8_t op, const void* cmd, uint8_t len)
{
  if (I2C->IsBusy())
    return ERROR;
  memcpy(OutputBuffer+1 , cmd,len);
  OutputBuffer[0] = op;
  TxI2CMsg TxMsg;
  TxMsg.Address = IndexToAddress[CB_Index];
  TxMsg.Length = len+1;
  TxMsg.pData = OutputBuffer;
  return I2C->SendMessage(&TxMsg);
}

ErrorStatus CB_Int::Config_Write(const CB_Config_t* cmd)
{
  return Write(11,cmd,sizeof(CB_Config_t));
}

ErrorStatus CB_Int::Control_Write(const CB_Control_t* cmd)
{
  return Write(10,cmd,sizeof(CB_Control_t));
}

ErrorStatus CB_Int::Full_Status_Request()
{
  if (I2C->IsBusy())
    return ERROR;
  OutputBuffer[0] = 0;
  TxI2CMsg TxMsg;
  RxI2CMsg RxMsg;
  TxMsg.Address = IndexToAddress[CB_Index];
  TxMsg.Length = 1;
  TxMsg.pData = OutputBuffer;
  RxMsg.Address = IndexToAddress[CB_Index];
  RxMsg.Length = sizeof(CB_Full_Status_t);
  RxMsg.pData = InputBuffer;
  return I2C->SendRequestMessage(&TxMsg,&RxMsg);
}

ErrorStatus CB_Int::Full_Status_Read(CB_Full_Status_t* cmd) const
{
  if ((I2C->IsBusy() == SET) || I2C->I2C_Last_Error != I2C_DRV::I2C_NoError) return ERROR;
  memcpy(cmd ,InputBuffer,sizeof(CB_Full_Status_t));
  return SUCCESS;
}

ErrorStatus CB_Int::GPIO_INIT()
{
  GPIO_InitTypeDef GPIO_InitStructure;
  for (int i=0 ; i< NUM_OF_CB; i++)
  {
    //Configure GPIO pin : Switch On Pin 
    GPIO_InitStructure.GPIO_Pin = SwOnPin[i];
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(SwOnPort[i],&GPIO_InitStructure);
    
    //Configure GPIO pin Output Level 
    GPIO_WriteBit(SwOnPort[i],SwOnPin[i],Bit_RESET);
  }
  
  return(SUCCESS);
}
  
void CB_Int::Power(FunctionalState En)
{
  
  GPIO_WriteBit(SwOnPort[CB_Index],SwOnPin[CB_Index],En == ENABLE ? Bit_SET : Bit_RESET);
  return;
}

uint16_t CB_Int::Get_CB_Temperature()
{
  return CBs[CB_Index].Meas_Temperature;
}
