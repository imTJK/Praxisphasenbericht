/*
 * spi_device.h
 *
 *  Created on: Nov 28, 2024
 *      Author: TK
 */

#ifndef INC_SPI_DEVICE_H_
#define INC_SPI_DEVICE_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "generic_bus_device.h"
#include "spi.h"
#include "dma.h"


#ifdef OOP_USE_RTOS
#include "cmsis_os.h"
#endif

typedef struct {
	// General
	uint32_t direction;
	uint32_t dataSize;
	uint32_t firstBit;

	// Mode
	uint32_t clockPolarity;
	uint32_t clockPhase;

	// Baudrate
	uint32_t baudRate = 0;
} SPIConfig;


// Class Definition
class SPIDevice : private GenericBusDevice {
protected:
	// Constructors and Destructors
	SPIDevice(
			SPI_HandleTypeDef*  handle,
			SPI_TypeDef*        instance,
			GPIO_TypeDef*       cs_port,
			uint16_t       			cs_pin,
			GPIO_PinState				cs_state,
			DMA_HandleTypeDef* 	dma_rx_h = nullptr,
			DMA_HandleTypeDef* 	dma_tx_h = nullptr
	);

	explicit SPIDevice(spi_devicetype_t device_id);

	explicit SPIDevice(SPIDevice* spi_device);

	explicit SPIDevice(spi_device_id_t spi_device_id);

	~SPIDevice() = default;;

	// (Re-)Configure SPI before use
	void configureSPI(const SPIConfig&);

	// Chip Select, can be overwritten if necessary
	void select(void);
	void deselect(void);


	// Read/Write
	/**
	 * \brief Reads variable length of Data from SPI into a buffer
	 *
	 * @param[out] rx_buffer pointer to uint8_t read-buffer
	 * @param[in] len length of the data to be read
	 * @returns HAL-Status
	 */
	HAL_StatusTypeDef read(uint8_t* rx_buffer, uint16_t len);

#ifdef OOP_USE_RTOS
	/**
	 * \brief Reads variable length of Data from SPI into a buffer.
	 * Waits for Completion via Semaphore afterwards.
	 *
	 * @param[out] rx_buffer pointer to uint8_t read-buffer
	 * @param[in] len length of the data to be read
	 * @param[in] semaphore FreeRTOS-Sempahore Handle
	 * @returns HAL-Status
	 */
	HAL_StatusTypeDef read(uint8_t* rx_buffer, uint16_t len, osSemaphoreId_t sempahore);
#endif

	/**
	 * \brief Writes variable length of Data from a buffer into SPI.
	 *
	 * @param tx_buffer pointer to uint8_t write-buffer
	 * @param len length of the data to be written
	 * @returns HAL-Status
	 */
	HAL_StatusTypeDef write(uint8_t* tx_buffer, uint16_t len);

#ifdef OOP_USE_RTOS
	/**
	 * \brief Writes variable length of Data from a buffer into SPI.
	 * Waits for Completion via Semaphore afterwards.
	 *
	 * @param tx_buffer pointer to uint8_t write-buffer
	 * @param len length of the data to be read
	 * @param semaphore FreeRTOS-Sempahore Handle
	 * @returns HAL-Status
	 */
	HAL_StatusTypeDef write(uint8_t* tx_buffer, uint16_t len, osSemaphoreId_t sempahore);
#endif

	/**
	 * \brief Writes variable length of Data from a buffer into SPI,
	 * then reads variable length of data from SPI into a buffer
	 *
	 * @param[in] tx_buffer pointer to uint8_t write-buffer
	 * @param[in] tx_len length of the data to be written
	 * @param[out] rx_buffer pointer to uint8_t read-buffer
	 * @param[in] rx_len length of the data to be read
	 * @returns HAL-Status
	 */
	HAL_StatusTypeDef writeThenRead(uint8_t* tx_buffer, uint16_t tx_len, uint8_t* rx_buffer, uint16_t rx_len);


	/**
	 * \brief Writes variable length of Data from a buffer into SPI,
	 * while reading the same length of data from SPI into a buffer.
	 *
	 * @param[in] tx_buffer pointer to uint8_t write-buffer
	 * @param[out] rx_buffer pointer to uint8_t read-buffer
	 * @param[in] len length of the data to be written/read
	 * @returns HAL-Status
	 */
	HAL_StatusTypeDef writeWhileRead(uint8_t* tx_buffer, uint8_t* rx_buffer, uint16_t len);

private:
	// Variables
	SPI_HandleTypeDef*  spi_handle;
	SPI_TypeDef *       spi_instance;
	GPIO_TypeDef*       spi_cs_port;
	uint16_t            spi_cs_pin;
	GPIO_PinState 		  spi_cs_state;
	DMA_HandleTypeDef* 	dma_rx_handle;
	DMA_HandleTypeDef* 	dma_tx_handle;

	uint32_t            pclk_freq;
};

#ifdef __cplusplus
}
#endif

#endif /* INC_SPI_DEVICE_H_ */

