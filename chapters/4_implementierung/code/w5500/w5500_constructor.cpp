W5500::W5500() 
    : SPIDevice(&hspi2, SPI_DEVICE_4_SPI_INSTANCE, 
                SPI_DEVICE_4_CS_PIN_PORT, SPI_DEVICE_4_CS_PIN, 
                GPIO_PIN_RESET, hspi2.hdmarx, hspi2.hdmatx), 
      GPIO_EXTI_Handler(W5500_INTn_Pin) {

    configureSPI(w5500_spi_config);

    // Configure Interrupt-Line (Falling Edge)
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = W5500_INTn_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(W5500_INTn_GPIO_Port, &GPIO_InitStruct);

    // Initialize network configuration
    NetInfo netInfo = {
        .mac = WIZ_MAC,
        .ip =  WIZ_IP,
        .sn =  WIZ_SUBNETMASK,
        .gw =  WIZ_IP,
        .dns = WIZ_IP
    };
    setNetInfo(&netInfo);
    reset();

    // Configure socket Rx-/Tx-Buffer sizes
    for(int8_t i = 0; i < _SOCK_NUM_; i++) {
        setSn_TXBUF_SIZE(i, tx_buffer_sizes[i]);
        setSn_RXBUF_SIZE(i, rx_buffer_sizes[i]);
    }

    // Enable Interrupts
    setSIMR(SOCK0_INTERRUPT_ENABLE);
    setSn_IR(WIZ_SOCK_NUM, 0x1f);  // Clear all interrupts
    setSn_IMR(WIZ_SOCK_NUM, (Sn_IR_TIMEOUT | Sn_IR_RECV | Sn_IR_DISCON));
}
