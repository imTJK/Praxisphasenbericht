/*
 * spi_devices.c
 *
 *  Created on: 04.08.2021
 *      Author: tw
 */


#include "spi_devices.h"
#include "spi.h"
#include "stdbool.h"

// Helper Functions
static inline bool spi_clk_is_plck1(SPI_TypeDef *spi_inst) {
  if (spi_inst == SPI1 || spi_inst == SPI4 ||
      spi_inst == SPI5 || spi_inst == SPI6) {
    return true;
  }
  return false;
}

static inline uint8_t get_spi_index(SPI_TypeDef *spi_inst) {
  if (spi_inst == SPI1) return 0;
  if (spi_inst == SPI2) return 1;
  if (spi_inst == SPI3) return 2;
  if (spi_inst == SPI4) return 3;
  if (spi_inst == SPI5) return 4;
  if (spi_inst == SPI6) return 5;
  return -1; // error
}

// SPI Semaphores
static osSemaphoreId semaphores[6];

void SPI_DeviceBusInit(spi_device_t* spi_device)
{
  GPIO_InitTypeDef GPIO_Init_struct = {0};

  SPI_TypeDef *  spi_inst = spi_device->spi_instance;
  // Init SPI
  spi_device->spi_h = SPI_Init(spi_inst);

  //Init Chip Select SPI  (low active)
  GPIO_Init_struct.Pin = spi_device->spi_cs_pin;
  GPIO_Init_struct.Mode = GPIO_MODE_OUTPUT_PP;//GPIO_MODE_OUTPUT_OD;
  GPIO_Init_struct.Pull = GPIO_PULLUP;
  GPIO_Init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(spi_device->spi_cs_port,&GPIO_Init_struct);

  HAL_GPIO_WritePin(spi_device->spi_cs_port,spi_device->spi_cs_pin, GPIO_PIN_SET); // set high
  // attach function pointer for r/w access to bus device
  spi_device->device_read = SPI_DeviceRead;
  spi_device->device_write = SPI_DeviceWrite;
  spi_device->device_write_then_read = SPI_Device_WriteThenRead;
  spi_device->device_write_while_read = SPI_Device_WriteWhileRead;
  spi_device->config_spi = SPI_Device_ConfigSPI;

  // Static semaphore Init || Needs to be changed when SPI3 and further is used
  uint8_t spi_idx = get_spi_index(spi_device->spi_instance);

  if(semaphores[spi_idx] == NULL) {
    osSemaphoreDef(temp_sem);
    semaphores[spi_idx] = osSemaphoreCreate(osSemaphore(temp_sem), 1);
  }
  spi_device->semaphore_id = semaphores[spi_idx];
}

uint8_t SPI_Device_ConfigSPI(void* device_h)
{
  spi_device_t* device = (spi_device_t*) device_h;

  device->spi_h->Init.DataSize = device->spi_settings.DataSize;
  device->spi_h->Init.CLKPolarity = device->spi_settings.CLKPolarity;
  device->spi_h->Init.CLKPhase = device->spi_settings.CLKPhase;
  device->spi_h->Init.FirstBit = device->spi_settings.FirstBit;

  // Overwrite BaudRatePrescaler Value if target baud rate is set
  if(device->spi_settings.BaudRate !=  0) {
    uint32_t spi_clock_hz;

    // Determine SPI clock frequency based on SPI instance
    if(spi_clk_is_plck1(device->spi_instance))
    {
      spi_clock_hz = HAL_RCC_GetPCLK2Freq(); // APB2 clock
    }
    else // SPI2, SPI3, etc.
    {
      spi_clock_hz = HAL_RCC_GetPCLK1Freq(); // APB1 clock
    }

    uint32_t prescaler_value = (spi_clock_hz + device->spi_settings.BaudRate - 1) / device->spi_settings.BaudRate;

    // Find the power of 2: 2^n where n is between 1 and 8 (prescaler 2 to 256)
    uint32_t power = 1; // Start at 2^1 = 2
    while ((1U << power) < prescaler_value && power < 8) {
      power++;
    }

    // Convert to HAL constant: power=1 maps to 0, power=2 maps to 1, etc.
    device->spi_h->Init.BaudRatePrescaler = (power - 1) << SPI_CR1_BR_Pos;
  }
  else {
    device->spi_h->Init.BaudRatePrescaler = device->spi_settings.BaudRatePrescaler;
  }

  return HAL_SPI_Init(device->spi_h);
}

// Override __weak HAL-Functions for Interrupt based SPI Communication
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi) {
  SPI_GiveSemaphore(semaphores[get_spi_index(hspi->Instance)]);
}


void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi) {
  SPI_GiveSemaphore(semaphores[get_spi_index(hspi->Instance)]);
  // TODO: Implement
}


void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi) {
  SPI_GiveSemaphore(semaphores[get_spi_index(hspi->Instance)]);
  // TODO: Implement
}

void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi);
void HAL_SPI_AbortCpltCallback(SPI_HandleTypeDef *hspi);


/**
 *
 * @param device_h
 * @param RX_buffer
 * @param data_count
 * @return
  HAL_OK       = 0x00U,
  HAL_ERROR    = 0x01U,
  HAL_BUSY     = 0x02U,
  HAL_TIMEOUT  = 0x03U
 */
uint8_t SPI_DeviceRead(void* device_h, uint8_t* RX_buffer, uint8_t data_count)
{
  spi_device_t* device = (spi_device_t*) device_h;
  uint8_t ret = 0;

  // activate chip-select
  spi_device_activate_cs(device->spi_cs_pin, device->spi_cs_port);

  HAL_SPI_Receive(device->spi_h, RX_buffer, data_count, 1000); // read

  // deactivate chip-select
  spi_device_deactivate_cs(device->spi_cs_pin, device->spi_cs_port);

  return ret;
}

/**
 *
 * @param device_h
 * @param RX_buffer
 * @param data_count
 * @return
  HAL_OK       = 0x00U,
  HAL_ERROR    = 0x01U,
  HAL_BUSY     = 0x02U,
  HAL_TIMEOUT  = 0x03U
 */
uint8_t SPI_DeviceRead_async(void* device_h, uint8_t* RX_buffer, uint8_t data_count)
{
  spi_device_t* device = (spi_device_t*) device_h;
  uint8_t ret = 0;

  SPI_TakeSemaphore(device->semaphore_id);

  // activate chip-select
  spi_device_activate_cs(device->spi_cs_pin, device->spi_cs_port);

  if(device->spi_h->hdmarx == NULL || device->spi_h->hdmatx == NULL) {
    ret = HAL_SPI_Receive_IT(device->spi_h, RX_buffer, data_count); // read
  }
  else {
    ret = HAL_SPI_Receive_DMA(device->spi_h, RX_buffer, data_count); // read
  }

  SPI_GiveTakeSemaphore(device->semaphore_id);

  // deactivate chip-select
  spi_device_deactivate_cs(device->spi_cs_pin, device->spi_cs_port);

  return ret;
}

/**
 *
 * @param device_h
 * @param RX_buffer
 * @param data_count
 * @return
  HAL_OK       = 0x00U,
  HAL_ERROR    = 0x01U,
  HAL_BUSY     = 0x02U,
  HAL_TIMEOUT  = 0x03U
 */
uint8_t SPI_DeviceRead_NoCS(void* device_h, uint8_t* RX_buffer, uint8_t data_count)
{
  spi_device_t* device = (spi_device_t*) device_h;
  device->config_spi(device);

  uint8_t ret = 0;
  //spi_device_t* device_h = GetSPIDeviceHandleFromID(ID);
  if(HAL_SPI_Receive(device->spi_h, RX_buffer, data_count, 1000) != HAL_OK) {
    return HAL_ERROR;
  };

  return ret;
}

/**
 *
 * @param device_h
 * @param RX_buffer
 * @param data_count
 * @return
  HAL_OK       = 0x00U,
  HAL_ERROR    = 0x01U,
  HAL_BUSY     = 0x02U,
  HAL_TIMEOUT  = 0x03U
 */
uint8_t SPI_DeviceRead_NoCS_async(void* device_h, uint8_t* RX_buffer, uint8_t data_count)
{
  spi_device_t* device = (spi_device_t*) device_h;
  device->config_spi(device);

  SPI_TakeSemaphore(device->semaphore_id);

  uint8_t ret = 0;
  //spi_device_t* device_h = GetSPIDeviceHandleFromID(ID);

  if(device->spi_h->hdmarx != NULL && device->spi_h->hdmatx != NULL) {
    ret = (uint8_t) HAL_SPI_Receive_DMA(device->spi_h, RX_buffer, data_count);
  }
  else {
    ret = (uint8_t) HAL_SPI_Receive_IT(device->spi_h,RX_buffer,data_count);
  }

  SPI_GiveTakeSemaphore(device->semaphore_id);

  return ret;
}

/**
 *
 * @param device_h
 * @param TX_buffer
 * @param data_count
 * @return
  HAL_OK       = 0x00U,
  HAL_ERROR    = 0x01U,
  HAL_BUSY     = 0x02U,
  HAL_TIMEOUT  = 0x03U
 */
uint8_t SPI_DeviceWrite(void* device_h, uint8_t* TX_buffer, uint8_t data_count)
{
  spi_device_t* device = (spi_device_t*) device_h;
  uint8_t ret = 0;

  // activate Chip-Select
  spi_device_activate_cs(device->spi_cs_pin, device->spi_cs_port);

  if(HAL_SPI_Transmit(device->spi_h, TX_buffer, data_count, 1000) != HAL_OK) {
    return HAL_ERROR;
  }

  // deactivate Chip-Select
  spi_device_deactivate_cs(device->spi_cs_pin, device->spi_cs_port);

  return ret;
}

/**
 *
 * @param device_h
 * @param TX_buffer
 * @param data_count
 * @return
  HAL_OK       = 0x00U,
  HAL_ERROR    = 0x01U,
  HAL_BUSY     = 0x02U,
  HAL_TIMEOUT  = 0x03U
 */
uint8_t SPI_DeviceWrite_async(void* device_h, uint8_t* TX_buffer, uint8_t data_count)
{
  spi_device_t* device = (spi_device_t*) device_h;
  uint8_t ret = 0;

  SPI_TakeSemaphore(device->semaphore_id);

  // activate Chip-Select
  spi_device_activate_cs(device->spi_cs_pin, device->spi_cs_port);

  if(device->spi_h->hdmarx == NULL || device->spi_h->hdmatx == NULL) {
    ret = HAL_SPI_Transmit_IT(device->spi_h, TX_buffer, data_count);  // write
  }
  else {
    ret = HAL_SPI_Transmit_DMA(device->spi_h, TX_buffer, data_count); // write
  }

  SPI_GiveTakeSemaphore(device->semaphore_id);

  // deactivate Chip-Select
  spi_device_deactivate_cs(device->spi_cs_pin, device->spi_cs_port);

  return ret;
}

/**
 *
 * @param device_h
 * @param TX_buffer
 * @param data_count
 * @return
  HAL_OK       = 0x00U,
  HAL_ERROR    = 0x01U,
  HAL_BUSY     = 0x02U,
  HAL_TIMEOUT  = 0x03U
 */
uint8_t SPI_DeviceWrite_NoCS(void* device_h, uint8_t* TX_buffer, uint8_t data_count)
{
  spi_device_t* device = (spi_device_t*) device_h;
  device->config_spi(device);

  uint8_t ret = 0;
  if(HAL_SPI_Transmit(device->spi_h, TX_buffer, data_count, 1000) != HAL_OK) {
    return HAL_ERROR;
  }

  return ret;
}

/**
 *
 * @param device_h
 * @param TX_buffer
 * @param data_count
 * @return
  HAL_OK       = 0x00U,
  HAL_ERROR    = 0x01U,
  HAL_BUSY     = 0x02U,
  HAL_TIMEOUT  = 0x03U
 */
uint8_t SPI_DeviceWrite_NoCS_async(void* device_h, uint8_t* TX_buffer, uint8_t data_count)
{
  spi_device_t* device = (spi_device_t*) device_h;
  device->config_spi(device);

  SPI_TakeSemaphore(device->semaphore_id);

  uint8_t ret = 0;
  if(device->spi_h->hdmarx != NULL && device->spi_h->hdmatx != NULL) {
    ret = (uint8_t) HAL_SPI_Transmit_DMA(device->spi_h, TX_buffer, data_count);
  }
  else {
    ret = (uint8_t) HAL_SPI_Transmit_IT(device->spi_h,TX_buffer,data_count);
  }

  SPI_GiveTakeSemaphore(device->semaphore_id);

  return ret;
}

/**
 * write then read to device without releasing Chip-Select
 * @param device_h
 * @param tx_buffer
 * @param tx_len
 * @param rx_buffer
 * @param rx_len
 * @return
  HAL_OK       = 0x00U,
  HAL_ERROR    = 0x01U,
  HAL_BUSY     = 0x02U,
  HAL_TIMEOUT  = 0x03U
 */
uint8_t SPI_Device_WriteThenRead(void* device_h, uint8_t* tx_buffer, uint8_t tx_len, uint8_t* rx_buffer, uint8_t rx_len)
{
  spi_device_t* device = (spi_device_t*) device_h;
  uint8_t ret = 0;

  // activate Chip-Select
  spi_device_activate_cs(device->spi_cs_pin, device->spi_cs_port);

  if(device->spi_h->hdmarx == NULL || device->spi_h->hdmatx == NULL) {
    ret = HAL_SPI_Transmit_IT(device->spi_h, tx_buffer, tx_len); // write
    ret = HAL_SPI_Receive_IT(device->spi_h, rx_buffer, rx_len);  // read
  }
  else{
    ret = HAL_SPI_Transmit_DMA(device->spi_h, tx_buffer, tx_len); // write
    ret = HAL_SPI_Receive_DMA(device->spi_h, rx_buffer, rx_len);  // read
  }

  // deactivate Chip-Select
  spi_device_deactivate_cs(device->spi_cs_pin, device->spi_cs_port);

  return ret;
}

uint8_t SPI_Device_WriteWhileRead(void* device_h, uint8_t* tx_buffer, uint8_t* rx_buffer, uint8_t txrx_len)
{
  spi_device_t* device = (spi_device_t*) device_h;
  uint8_t ret = 0;

  // activate Chip-Select
  spi_device_activate_cs(device->spi_cs_pin, device->spi_cs_port);

  // same time write & read
  if(device->spi_h->hdmarx == NULL || device->spi_h->hdmatx == NULL) {
    ret = HAL_SPI_TransmitReceive_IT(device->spi_h, tx_buffer, rx_buffer, txrx_len);
  }
  else {
    ret = HAL_SPI_TransmitReceive_DMA(device->spi_h, tx_buffer, rx_buffer, txrx_len);
  }

  // deactivate Chip-Select
  spi_device_deactivate_cs(device->spi_cs_pin, device->spi_cs_port);

  return ret;
}

void spi_device_activate_cs(uint16_t pin, GPIO_TypeDef* pin_port)
{
  // Chip-Select low active
  HAL_GPIO_WritePin(pin_port,pin,GPIO_PIN_RESET);
}

void spi_device_deactivate_cs(uint16_t pin, GPIO_TypeDef* pin_port)
{
  // Chip-Select low active
  HAL_GPIO_WritePin(pin_port,pin,GPIO_PIN_SET);
}

void SPI_TakeSemaphore(osSemaphoreId semaphore_id) {
  // Take the Semaphore
  volatile osStatus result = (osStatus) osSemaphoreWait(semaphore_id, SPI_RTOS_TIMEOUT_MS);

  // TODO Error Handling
  if(result != osOK) {
    return;
  }
}

void SPI_GiveSemaphore(osSemaphoreId semaphore_id) {
  // Release the Semaphore
  volatile osStatus result = osSemaphoreRelease(semaphore_id);

  if (result != osOK){
    // TODO Error Handling
    return;
  }
}

// Used to make sure async transactions don't deselect CS before completion.
void SPI_GiveTakeSemaphore(osSemaphoreId semaphore_id) {
  // Take the Semaphore
  volatile osStatus result = (osStatus) osSemaphoreWait(semaphore_id, SPI_RTOS_TIMEOUT_MS);

  if(result != osOK) {
    // TODO Error Handling
    return;
  }

  // Release the Semaphore
  result = osSemaphoreRelease(semaphore_id);

  if (result != osOK){
    // TODO Error Handling
    return;
  }
}
