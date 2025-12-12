const SPIConfig w5500_spi_config = {
    .direction = SPI_DIRECTION_2LINES,
    .dataSize = SPI_DATASIZE_8BIT,
    .firstBit = SPI_FIRSTBIT_MSB,
    .clockPolarity = SPI_POLARITY_LOW,
    .clockPhase = SPI_PHASE_1EDGE,
    .baudRate = 25000000  // 25 MHz
};
