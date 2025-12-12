/** @file
 *
 * config file for unit_zzz_demo.c.
 *
 *  Created on: 14.01.2022
 *      Author: pr
 */

#ifndef APP_CONFIG_ZZZ_DEMO_2022A_H_
#define APP_CONFIG_ZZZ_DEMO_2022A_H_

#define UNIT_DEMO

#define VSTR_UNIT_CONFIG   "2022a"


// Name to display on Home Screen (default is ATN-Control):
#define UNIT_DISPLAY_NAME  "FeatureDEMO"

// Directives for generator script:
// [GENERATE use] unit_zzz_demo_params.ini
// [GENERATE use] example_settings.ini
// [GENERATE use] module_process_settings.ini
// [!GENERATE use] module_demo_clock_settings.ini <-- needs implementation of s:hms unit format

// IO-Connections:
#define HARDWARE_CONFIG_FILE  "atnc_hwconfig-uc02a-io02d.h" // first production combination of uC-Board and IO-Board
#define IO_ADAPTER            IO_ADAPTER_NONE

#define IO_EXTERN_MAPPING  IO_EXTERN_MAPPING_CONFIG  // (Function <=> IO) mappings are defined in this config file
#define VSTR_IO_MAPPING    "CFG"  // name this io mapping

// CONTROL-STD - Base IOs
#define OUT_PowerOn                      CON_CONTROL_STD_03  // Device is booted completely and running.
#define OUT_Ready                        CON_CONTROL_STD_04  // Ready to start Process. Low in PS_BUSY and PS_MANUAL state or Flux activated via FluxCont or Serial.
#define OUT_Busy                         CON_CONTROL_STD_05  // High in PS_BUSY state
#define OUT_Error                        CON_CONTROL_STD_06  // High in PS_ERROR state
#define OUT_Warning                      CON_CONTROL_STD_07  // TODO Warning

#define IN_ParaBit1/*IN_ParaLow*/        CON_CONTROL_STD_15  // select parameter set 0..3: low bit
#define IN_ParaBit2/*IN_ParaHigh*/       CON_CONTROL_STD_16  // select parameter set 0..3: high bit
#define IN_ParaBit3                      DI_none
#define IN_StartProcess                  CON_CONTROL_STD_21  // Start Process
#define IN_StartProcess2                 CON_CONTROL_STD_20  // Start Process2 (Dispensing)
#define IN_StopProcess                   CON_CONTROL_STD_22  // Stop Process => External error
#define IN_Reset                         CON_CONTROL_STD_23  // Hardware Reset (no software use)
#define IN_ErrorQuit                     CON_CONTROL_STD_24  // Quit Error State


// Preferences:
//#define PROCESS_TIME_SHOW_REMAINING
#define PROCESS_TIME_SHOW_ELAPSED

#define USE_PROCESS_2

// Enable to send buffer content to the line
// above the button bar with UI_SetStatusBarLine():
#define USE_STATUS_BAR


// Timer Module:
#define TIMER_NUM_TIMERS 2  // number of standard timers

// Menu "Tool Settings" visible also for Setup mode:
#define MENU_ACCESS_MODE_TOOL_SETTINGS  (AM_FACTORY_RW | AM_SETUP_RW)

#define NUM_EXTRA_EEPROM_VARS     100

/// @brief Number of Parameter Sets.
#define NUM_PARAMSETS 4


#define DEFAULT_PHASE_A1_TIME     50 //  5.0 s
#define DEFAULT_PHASE_B1_TIME    200 // 20.0 s
#define DEFAULT_PHASE_A1_POWER    10
#define DEFAULT_PHASE_B1_POWER    10
#define DEFAULT_PROCESS2_TIME    200 // 20.0 s
#define DEFAULT_PROCESS2_POWER    10

//*********************************************************************
//*********************************************************************
//* Serial Communication with PC *
//********************************


#define SERIAL_BAUDRATE                   9600
#define SERIAL_INTERBYTE_TIMEOUT_SECONDS  0.1

// Legacy Serial config

/// @brief Legacy Serial: Use parameter set number.
///
/// Use Parameter-sets configured with NUM_PARAMSETS in legacy serial protocol.
/// If SERL_USE_PSET is 0, the part with the Parameter-set Number (PS)
/// in square brackets is omitted in the following commands:
/// -   <STX>8<CR>[PS]...        (Set all parameters [in parameter set PS])
/// -   <STX>19<CR>0<CR>[PS<CR>] (Start process [with parameter set PS])
/// -   <STX>32<CR>[PS<CR>]      (Get all parameters [from parameter set PS])
#define SERL_USE_PSET 1

/// @brief Legacy Serial: Send parameter set number ASCII encoded.
///
/// If SERL_PSET_ALWAYS_ASCII is 1, the parameter set number (PS) is send
/// as ASCII, terminated with <CR> (0x0D) also in the <STX>8<CR> command.
/// In this case, the command's syntax is <STX>8<CR>PS<CR>P1P2..PN
/// where PS is the ASCII encoded number of the parameter set and
/// P1, P2, ...PN are the values for the Parameters binary encoded.
/// If SERL_PSET_ALWAYS_ASCII is 0, only the PS of the <STX>8<CR>
/// command is send binary encoded without a <CR> termination. The other
/// commands (19, 32) still use ASCII encoded parameter set numbers.
#define SERL_PSET_ALWAYS_ASCII 0


//*********************************************************************
//* Activate Module Functionality *
//*********************************
// Unused SPI-Addon Board
#define SPI_DEVICE_TYPE_ADDON2	 SPI_NO_DEVICE_TYPE
#define SPI_DEVICE_5_CS_PIN_PORT GPIOA
#define SPI_DEVICE_5_CS_PIN 	 	 GPIO_PIN_1

// W5500 - Ethernet Module
#define _WIZCHIP_ 		W5500
#define _WIZCHIP_BAUDRATE_ 10000000

/// Hardware Defines ///
#define SPI_DEVICE_TYPE_ADDON1	 WIZNET_W5500
#define SPI_DEVICE_4_CS_PIN_PORT GPIOH
#define SPI_DEVICE_4_CS_PIN 	 	 GPIO_PIN_3

/// Socket Defines ///
// Default: 2kB per Socket; total must add up to WIZ_MAX_BUFFER_SIZE
#define WIZ_MAX_BUFFER_SIZE 16384
#define WIZ_TX_BUFFER_SIZES {2, 2, 2, 2, 2, 2, 2, 2}
#define WIZ_RX_BUFFER_SIZES {2, 2, 2, 2, 2, 2, 2, 2}

/// Network Defines ///
#define WIZ_IP 					{169, 254, 90, 120}
#define WIZ_MAC   			{0xC0, 0xE0, 0x07, 0x11, 0x11, 0x11}
#define WIZ_PORT 				80
#define WIZ_SUBNETMASK  {255, 255, 0, 0}

// Enable / Disable Ethernet Debug Port
#define ETHERNET_DBGOUT

#ifdef ETHERNET_DBGOUT
#define DEBUG_PORT 5000
#define DEBUG_BROADCAST_IP {255, 255, 255, 255}
#endif


//*********************************************************************
//*********************************************************************
// IN/OUT Signal Redirects and Forwarding
#define USE_SignalForwarding  1


//*********************************************************************
//*********************************************************************

#endif /* APP_CONFIG_ZZZ_DEMO_2022A_H_ */
