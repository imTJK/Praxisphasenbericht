/*
 * spi_devices.h
 *
 *  Created on: 03.08.2021
 *      Author: tw
 */

#ifndef APP_INC_SPI_DEVICES_H_
#define APP_INC_SPI_DEVICES_H_

#include "atnc_config.h"
#include "cmsis_os.h"

#define SPI_RTOS_TIMEOUT_MS 1000

// Override __weak HAL-Functions for Interrupt based SPI Communication
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi);
void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi);
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi);
void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi);
void HAL_SPI_AbortCpltCallback(SPI_HandleTypeDef *hspi);

// RTOS Functions
void SPI_TakeSemaphore(osSemaphoreId semaphore_id);
void SPI_GiveSemaphore(osSemaphoreId semaphore_id);

//spi_device_t spi_device_handles[NUM_SPI_DEVICES];
uint8_t SPI_DeviceRead(void* device_h, uint8_t* rx_buffer, uint8_t data_count);
uint8_t SPI_DeviceRead_async(void* device_h, uint8_t* rx_buffer, uint8_t data_count);

uint8_t SPI_DeviceWrite(void* device_h, uint8_t* tx_buffer, uint8_t data_count);
uint8_t SPI_DeviceWrite_async(void* device_h, uint8_t* tx_buffer, uint8_t data_count);

uint8_t SPI_Device_WriteThenRead(void* device_h, uint8_t* tx_buffer, uint8_t tx_len, uint8_t* rx_buffer, uint8_t rx_len);
uint8_t SPI_Device_WriteThenRead_async(void* device_h, uint8_t* tx_buffer, uint8_t tx_len, uint8_t* rx_buffer, uint8_t rx_len);

uint8_t SPI_Device_WriteWhileRead(void* device_h, uint8_t* tx_buffer, uint8_t* rx_buffer, uint8_t txrx_len);
uint8_t SPI_Device_WriteWhileRead_async(void* device_h, uint8_t* tx_buffer, uint8_t* rx_buffer, uint8_t txrx_len);


uint8_t SPI_Device_ConfigSPI(void* device_h);

void spi_device_activate_cs(uint16_t pin, GPIO_TypeDef* pin_port);
void spi_device_deactivate_cs(uint16_t pin, GPIO_TypeDef* pin_port);

typedef uint8_t (*fp_spi_device_rw)(void* spi_device_h, uint8_t* rx_tx_buffer, uint8_t data_count);
typedef uint8_t (*fp_spi_device_rtw)(void* spi_device_h, uint8_t* tx_buffer, uint8_t tx_len, uint8_t* rx_buffer, uint8_t rx_len);
typedef uint8_t (*fp_spi_device_rww)(void* spi_device_h, uint8_t* tx_buffer, uint8_t* rx_buffer, uint8_t rx_len);
typedef uint8_t (*fp_spi_device_config_spi)(void* spi_device_h);
//spi_device_t* GetSPIDeviceHandleFromID(device_id_t ID);

typedef struct __SPI_Bus_Settings_TypeDef
{
	// Config SPI depending on device
	uint32_t DataSize;
	uint32_t CLKPolarity;
	uint32_t CLKPhase;
	uint32_t FirstBit;
	uint32_t BaudRatePrescaler;
  uint32_t BaudRate; // if != 0: configureSPI() ignores BaudRatePrescaler
}spi_settings_t;

typedef struct __SPI_Device_TypeDef
{
  device_id_t         bus_device_id;
  devicetype_t        device_type;
  SPI_HandleTypeDef*  spi_h;
  SPI_TypeDef *       spi_instance;
  GPIO_TypeDef*       spi_cs_port;
  int16_t             spi_cs_pin;
  spi_settings_t	  spi_settings;
  fp_spi_device_rw    device_read;
  fp_spi_device_rw    device_write;
  fp_spi_device_rtw   device_write_then_read;
  fp_spi_device_rww   device_write_while_read;
  fp_spi_device_config_spi config_spi;
  osSemaphoreId       semaphore_id;
}spi_device_t;

void SPI_DeviceBusInit(spi_device_t* spi_device);

#endif /* APP_INC_SPI_DEVICES_H_ */
