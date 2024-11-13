
/**
 * @file flipper_spi_wrapper.cpp
 * @brief SPIclass method implementation
 * @ingroup CAN-DRIVER-ADAPTATION
 * */
#include "SPI.h"
#include "furi_hal_spi.h"
#define SPI_TIMEOUT 1000

SPIClass SPI;

/** 
 * @brief Arduino's SPI.transfer porting. It sends (on MOSI) and receives (on MISO) one byte through @ref furi_hal_spi_bus_trx().
 * @param[in] tx_val : data byte to be sent.
 * @return returns data byte received.
 * */
uint8_t SPIClass::transfer(uint8_t tx_val) {
    uint8_t rx;
    furi_hal_spi_bus_trx(&furi_hal_spi_bus_handle_external, &tx_val, &rx, 1, SPI_TIMEOUT);
    return rx;
}

/** 
 * @brief Arduino's SPI.beginTransaction porting : It requests SPI ressource access ( @ref furi_hal_spi_bus_handle_external) by acquiring SPI related mutex through @ref furi_hal_spi_acquire().
 * @param[in] settings : This attribute is unused since SPI is already configured by FP0 firmware.
 * */
void SPIClass::beginTransaction(SPISettings __attribute__((unused)) settings) {
    furi_hal_spi_acquire(&furi_hal_spi_bus_handle_external);
    return;
}

/** 
 * @brief Arduino's SPI.endTransaction porting : It frees SPI ressource access by releasing SPI related mutex ( @ref furi_hal_spi_bus_handle_external) through @ref furi_hal_spi_release().
 * */
void SPIClass::endTransaction(void) {
    furi_hal_spi_release(&furi_hal_spi_bus_handle_external);
    return;
}

/** 
 * @brief Arduino's SPI.begin porting: It is an empty function kept to be compatible with Longan Lab's library.
 * */
void SPIClass::begin(void) {
    return;
}

/** 
 * @brief Arduino's SPISettings constructor definition. It is an empty function kept to be compatible with Longan Lab's library.
 * */
SPISettings::SPISettings(
    uint32_t __attribute__((unused)) freqHz,
    e_spi_endiannes_t __attribute__((unused)) endianness,
    e_spi_mode_t __attribute__((unused)) mode) {
}