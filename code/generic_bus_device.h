/*
 * generic_bus_device.h
 *
 *  Created on: 05.08.2021
 *      Author: tw
 */

#ifndef APP_INC_GENERIC_BUS_DEVICE_H_
#define APP_INC_GENERIC_BUS_DEVICE_H_

#include "atnc_config.h"
#include "generic_bus_device_datatypes.h"
#include "spi_devices.h"

#include "ad7324_adc.h"
#include "ad5724_dac.h"
#include "mcp23s08_io.h"
#include "max31865_rtd.h"
#include "tmc5160_stepper_cwrapper.h"
#include "W5500/w5500.h"





void Bus_Devices_InitALL(void); // init in main()
bus_device_t* Bus_Device_GetDevice(device_id_t device_id);



#endif /* APP_INC_GENERIC_BUS_DEVICE_H_ */
