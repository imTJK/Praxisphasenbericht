class SPIDevice : private GenericBusDevice {
protected:
    SPIDevice(
        SPI_HandleTypeDef*  handle,
        SPI_TypeDef*        instance,
        GPIO_TypeDef*       cs_port,
        uint16_t            cs_pin,
        GPIO_PinState       cs_state,
        DMA_HandleTypeDef*  dma_rx_h = nullptr,
        DMA_HandleTypeDef*  dma_tx_h = nullptr
    );

    void configureSPI(const SPIConfig& config);
    
    void select(void);    // Activate chip-select
    void deselect(void);  // Deactivate chip-select

    HAL_StatusTypeDef read(uint8_t* rx_buffer, uint16_t len);
    HAL_StatusTypeDef write(uint8_t* tx_buffer, uint16_t len);
    HAL_StatusTypeDef writeThenRead(uint8_t* tx_buf, uint16_t tx_len, 
                                    uint8_t* rx_buf, uint16_t rx_len);
    HAL_StatusTypeDef writeWhileRead(uint8_t* tx_buf, uint8_t* rx_buf, 
                                     uint16_t len);

private:
    SPI_HandleTypeDef*  spi_handle;
    SPI_TypeDef*        spi_instance;
    GPIO_TypeDef*       spi_cs_port;
    uint16_t            spi_cs_pin;
    GPIO_PinState       spi_cs_state;
    DMA_HandleTypeDef*  dma_rx_handle;
    DMA_HandleTypeDef*  dma_tx_handle;
    uint32_t            pclk_freq;
};
