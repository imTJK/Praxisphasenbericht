/*
 * spi_device_config.h
 *
 *  Created on: Jun 12, 2025
 *      Author: TK
 */

#ifndef INC_SPI_DEVICE_CONFIG_H_
#define INC_SPI_DEVICE_CONFIG_H_

#include "spi.h"
#include "gpio.h"


// Simplified device ID enum - only what is actually used
enum SPIDeviceID {
    SPI_IO_MCP23S08 = 0,        // MCP23S08 - but we don't need to store this info
    SPI_ADC_AD7324,         // AD7324 - device behavior is in the driver classes
    SPI_DAC_AD5724,         // AD5724 - not in the base SPI device class
    SPI_ETH_W5500,          // W5500
    SPI_ADDON1,             // Configurable
    SPI_ADDON2,             // Configurable
    SPI_DEVICE_COUNT
};
using spi_device_id_t = SPIDeviceID;

// Single configuration struct instead of multiple arrays
typedef struct {
    SPI_TypeDef* spi_instance;
    GPIO_TypeDef* cs_port;
    uint16_t cs_pin;
    GPIO_PinState cs_active_state;  // Some devices use active high, others low
    uint32_t baudrate = 0;
} spi_device_config_t;

// Single configuration table - much cleaner than multiple arrays
static const spi_device_config_t spi_device_configs[SPI_DEVICE_COUNT] = {
    // SPI_IO_MCP23S08
    {
        .spi_instance = SPI1,
        .cs_port = GPIOI,
        .cs_pin = GPIO_PIN_1,
        .cs_active_state = GPIO_PIN_RESET,
				.baudrate = 10000000 // 10 MHz
    },
    // SPI_ADC_AD7324
    {
        .spi_instance = SPI1,
        .cs_port = GPIOI,
        .cs_pin = GPIO_PIN_4,
        .cs_active_state = GPIO_PIN_RESET,
				.baudrate = 10000000 // 10 MHz
    },
    // SPI_DAC_AD5724
    {
        .spi_instance = SPI1,
        .cs_port = GPIOI,
        .cs_pin = GPIO_PIN_5,
        .cs_active_state = GPIO_PIN_RESET,
				.baudrate = 10000000 // 10 MHz
    },
    // SPI_ETH_W5500
    {
        .spi_instance = SPI2,
        .cs_port = GPIOH,
        .cs_pin = GPIO_PIN_3,
        .cs_active_state = GPIO_PIN_RESET,
				.baudrate = 25000000 // 25 MHz
    },
    // SPI_ADDON1
    {
        .spi_instance = SPI2,
        .cs_port = GPIOH,
        .cs_pin = GPIO_PIN_3,
        .cs_active_state = GPIO_PIN_RESET
    },
    // SPI_ADDON2
    {
        .spi_instance = SPI2,
        .cs_port = GPIOA,
        .cs_pin = GPIO_PIN_2,
        .cs_active_state = GPIO_PIN_RESET
    }
};



#endif /* INC_SPI_DEVICE_CONFIG_H_ */
