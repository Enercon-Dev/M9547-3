// **********************************************************************
// **********************************************************************
// Copyright 2011 by Avi Owshanko. All rights reserved.
// **********************************************************************
// **********************************************************************

#ifndef PERSISTENT_COMMON_H
#define PERSISTENT_COMMON_H

#include <stdint.h>
#include "stm32f10x.h"

// *******************************************************************
// general
// *******************************************************************
#define BLOCK_SIZE (0x800)

// *******************************************************************
// Persistent memory definitions
// *******************************************************************
#define FLASH_BLOCK_SIZE ((uint32_t)(0x800))
#define PERSISTENT_BLOCK_HIGH_WATER_MARK (((FLASH_BLOCK_SIZE  / 16) * 15))
#define PERSISTENT_BLOCK_LOW_WATER_MARK (((FLASH_BLOCK_SIZE  / 16) * 12))
#define PERSISTENT_SECTION_BASE ((uint32_t)(0x08001000))
#define PERSISTENT_SECTION_SIZE ((uint32_t)(0x1000))
#define PERSISTENT_BLOCK_COUNT (PERSISTENT_SECTION_SIZE/FLASH_BLOCK_SIZE)

#define PERSISTENT_BLOCK_BASE(var)                                  \
           (PERSISTENT_SECTION_BASE + (var * FLASH_BLOCK_SIZE))

#define PERSISTENT_MAGIC_NUM "M9547 SW a"
#define PERSISTENT_VERSION "20251117_003.000.000"
#define CUSTOMER_PN         "M9547-3.-"

// a single block holding all unchangable data, such as MAC address.
#define PERSISTENT_DATA_STATIC_BLOCK_A            (0)
#define PERSISTENT_DATA_SYSTEM_DATA               (1)

//#define PERSISTENT_DATA_STATIC_BLOCK_B            (1)
/*
#define PERSISTENT_DATA_GENERAL_BLOCK_BASE        (2)
#define PERSISTENT_DATA_PORT1_BLOCK_BASE          (5)
#define PERSISTENT_DATA_PORT2_BLOCK_BASE          (8)
#define PERSISTENT_DATA_PORT3_BLOCK_BASE          (11)
#define PERSISTENT_DATA_PORT4_BLOCK_BASE          (14)
#define PERSISTENT_DATA_ETHERNET_DEFS             (17)
#define PERSISTENT_DATA_SYSTEM_STATS              (20)
*/
#define PERSISTENT_ETHERNET_SETTINGS              (26)
#define MAGIC_NUM_OFFSET        (0x00)
#define CHIP_ID_OFFSET          (0x20)
#define VERSION_OFFSET          (0x40)
#define MAC_ADDR_OFFSET         (0x60)
#define SN_OFFSET               (0x80)

// *******************************************************************
// Reserved memory boundries
// *******************************************************************
//#define BOOT_BLOCK_BASE (0x08000000)
// This address marks the end of '.intvec', defined in 'startup_stm32f10x_cl.c'
// Address where we keep the local boot loader copy for safe transition.
//#define BOOT_COPY_BLOCK (0x08000800)
// Address for CRC data and other stuff.
//#define BOOT_CRC_BLOCK (0x08000C00)
// This address is the actual start of the boot loader section
#define BOOT_LOADER_BASE        (0x08000800)
#define BOOT_LOADER_TOP         (0x08001000)
#define ACTIVE_VERSION_BASE     (0x08002000)
#define INT_VEC_TOP (ACTIVE_VERSION_BASE + 0x150)
#define MAGIC_NUM_LOCATION      (INT_VEC_TOP)
#define VERSION_DOWNLOAD_BASE   (0x08020000)
#define MAX_VERSION_SIZE        (VERSION_DOWNLOAD_BASE - ACTIVE_VERSION_BASE)

//#define BOOT_CRC_BLOCK_SIZE (0x400)
//#define BOOT_BLOCK_SIZE (0x400)

// *******************************************************************
// Serial ports defaults
// *******************************************************************
#define SERIAL_PORT_COUNT (4)

// *******************************************************************
// Common functions
// *******************************************************************
             
#pragma pack(push)
#pragma pack(1)
typedef struct {
  char magicNumber[20];
  uint32_t bootLocation;
  uint32_t active_version_length;
  uint32_t new_version_length;
  char active_version_file[33];
  char new_version_file[33];
  char magicNumber2[20];
  uint32_t FPGA_version_length;
  char FPGA_version_file[33];
} Boot_Memory_Type;
#pragma pack(pop)

#ifdef __cplusplus
 extern "C" {
#endif

// Following used for compiler allocation.
int readDummyFlash();

#ifdef __cplusplus
 }
#endif







#endif // PERSISTENT_COMMON_H
