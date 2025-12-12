void SPIDevice::configureSPI(const SPIConfig& config) {
    // General configuration
    spi_handle->Init.Direction = config.direction;
    spi_handle->Init.DataSize = config.dataSize;
    spi_handle->Init.FirstBit = config.firstBit;

    // Clock mode
    spi_handle->Init.CLKPhase = config.clockPhase;
    spi_handle->Init.CLKPolarity = config.clockPolarity;

    // Baudrate calculation
    if(config.baudRate != 0) {
        uint32_t prescaler_value = (pclk_freq + config.baudRate - 1) 
                                   / config.baudRate;
        
        // Find power of 2: 2^n where n is between 1 and 8
        uint32_t power = 1;
        while ((1U << power) < prescaler_value && power < 8) {
            power++;
        }
        // Convert to HAL constant
        spi_handle->Init.BaudRatePrescaler = (power - 1) << SPI_CR1_BR_Pos;
    }
    else {
        spi_handle->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
    }

    HAL_SPI_Init(spi_handle);
}
