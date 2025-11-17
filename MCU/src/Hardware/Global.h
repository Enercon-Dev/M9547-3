#ifndef _GLOBAL_H
#define _GLOBAL_H

#include "stm32f10x.h"

typedef union{
	struct
	{
		uint16_t PMDS_no_go : 1;
		uint16_t PMDS_degraded : 1;
		uint16_t SSPC_no_go : 1;
		uint16_t SSPC_degraded : 1;
		uint16_t WDG_fail : 1;
		uint16_t power_down_inFlight :1;
		uint16_t spare1 : 1;
		uint16_t sp10_fail : 1;
		uint16_t sp11_fail : 1;
		uint16_t i2c_near_fail :1;
		uint16_t i2c_far_fail : 1;
		uint16_t adc_ref_fail : 1;
		uint16_t eth_fail : 1;
		uint16_t spare2 : 3;
	}; 
	uint16_t ToUint16;
}Status_Word_1_T;

typedef union{
	struct 
	{
		uint16_t arinc_tx_fail : 1;
		uint16_t arinc_rx1_fail : 1;
		uint16_t arinc_rx2_fail : 1;
		uint16_t int_can_fail : 1;
		uint16_t ext_can_fail : 1;
		uint16_t temp_not_in_range :1;
		uint16_t int_can_tx_err : 1;
		uint16_t int_can_rx_err : 1;
		uint16_t int_can_busoff : 1;
		uint16_t ext_can_tx_err :1;
		uint16_t ext_can_rx_err : 1;
		uint16_t ext_can_busoff_err : 1;
		uint16_t spare1 : 1;
		uint16_t app_crc_fail: 1;
		uint16_t spare2 : 2;
	}; 
	uint16_t ToUint16;
}Status_Word_2_T;

typedef union{
	struct 
	{
		uint16_t spare1 : 1;
		uint16_t ext_nvm_1_fail : 1;
		uint16_t ext_nvm_2_fail : 1;
		uint16_t side_strap_fail : 1;
		uint16_t disc_io_rw_fail : 1;
		uint16_t disc_io_mux1_fail :1;
		uint16_t disc_io_mux2_fail : 1;
		uint16_t disc_io_mux3_fail : 1;
		uint16_t disc_io_mux4_fail : 1;
		uint16_t chA_fail_disc :1;
		uint16_t chB_fail_disc : 1;
		uint16_t adc_mux_fail : 1;
		uint16_t sspcA_power_fail : 1;
		uint16_t sspcB_power_fail: 1;
		uint16_t spare2 : 2;
	}; 
	uint16_t ToUint16;
}Status_Word_3_T;

typedef union{
	struct 
	{
		uint16_t rccb_ch1_fail : 1;
		uint16_t rccb_ch2_fail : 1;
		uint16_t sspc_module_conf_fail : 1;
		uint16_t sspc_config_partnum_fail : 1;
		uint16_t config_int_crc_fail : 1;
		uint16_t buddy_config_crc_fail :1;
		uint16_t surplus_sspc_conf_fail : 1;
		uint16_t missing_sspc_conf_fail : 1;
		uint16_t fuse1_fail : 1;
		uint16_t fuse2_fail :1;
		uint16_t rs232_fail : 1;
		uint16_t rs232_timeout : 1;
		uint16_t spare : 4;
	}; 
	uint16_t ToUint16;
}Status_Word_4_T;

typedef union{
	struct 
	{
		uint16_t adc_vcc1_low_fail : 1;
		uint16_t adc_vcc1_high_fail : 1;
		uint16_t adc_vcc2_low_fail : 1;
		uint16_t adc_vcc2_high_fail : 1;
		uint16_t adc_vcc3_low_fail : 1;
		uint16_t adc_vcc3_high_fail : 1;
		uint16_t sspcA12V_low_fail : 1;
		uint16_t sspcA12V_high_fail : 1;
		uint16_t sspcA8V_low_fail : 1;
		uint16_t sspcA8V_high_fail : 1;
		uint16_t sspcB12V_low_fail : 1;
		uint16_t sspcB12V_high_fail : 1;
		uint16_t sspcB8V_low_fail : 1;
		uint16_t sspcB8V_high_fail : 1;
		uint16_t spare : 2;
	}; 
	uint16_t ToUint16;
}Status_Word_5_T;

typedef union{
	struct
	{
		uint16_t int_sram_fail : 1;
		uint16_t boot_crc_fail : 1;
		uint16_t dio_exp1_fail : 1;
		uint16_t dio_exp2_fail : 1;
		uint16_t disc_add_parity_fail : 1;
		uint16_t mcdu1_fail : 1;
		uint16_t mcdu2_fail : 1;
		uint16_t spare : 9;
	}; 
	uint16_t ToUint16;
}Status_Word_6_T;

class GlobalParams
{
  public:
	static uint16_t *Status[6];
	static Status_Word_1_T Status_Word_1;
	static Status_Word_2_T Status_Word_2;
	static Status_Word_3_T Status_Word_3;
	static Status_Word_4_T Status_Word_4;
	static Status_Word_5_T Status_Word_5;
	static Status_Word_6_T Status_Word_6;
        
};

#endif //_GLOBAL_H
