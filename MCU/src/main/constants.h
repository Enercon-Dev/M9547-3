#ifndef CONSTANTS_H
#define CONSTANTS_H

#define NUM_OF_CB 16
#define NUM_OF_GROUPS 8
#define NUM_OF_Slave_Ports 5
#define ADC_TEMP_SHUTDOWN 105
#define ADC_TEMP_WARNING 85

#if (NUM_OF_CB < 9)
	#define uint_cb uint8_t
	#define CB_BYTE 1
#elif (NUM_OF_CB < 17)
	#define uint_cb uint16_t
	#define CB_BYTE 2
#elif (NUM_OF_CB < 33)
	#define uint_cb uint32_t
	#define CB_BYTE 4
#else 
	#define uint_cb uint64_t
	#define CB_BYTE 8
#endif

#endif // CONSTANTS_H
