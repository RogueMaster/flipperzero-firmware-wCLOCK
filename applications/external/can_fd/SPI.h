/** 
  @defgroup CAN-DRIVER
  @brief This is an porting of Longan Labs [Longan_CANFD](https://github.com/Longan-Labs/Longan_CANFD) library to flipper zero board. 
  @{
  @defgroup CAN-DRIVER-ADAPTATION
  @brief Longan Lab's [Longan_CANFD](https://github.com/Longan-Labs/Longan_CANFD) library adaptation files.
  @{
 * @file SPI.h
 * @brief Bridge for porting Longan_CANFD to flipper zero without modifying code. A simple pull to the repo shall be OK.
*/
#pragma once
#include <stdint.h>

typedef enum {
    SPI_MODE_0,
    SPI_MODE_1,
    SPI_MODE_2,
    SPI_MODE_3
} e_spi_mode_t;

typedef enum {
    MSB_FIRST,
    LSB_FIRST
} e_spi_endiannes_t;

typedef uint8_t byte;
#define pinMode(X, Y)
#define digitalWrite(X, Y)
#define SPI_HAS_TRANSACTION
#define MSBFIRST  MSB_FIRST
#define SPI_MODE0 SPI_MODE_0

/**
 * @brief Unused structure passed to @ref SPIClass::beginTransaction kept to ensure compatibility with Longan Labs library. */
class SPISettings {
public:
    uint32_t freqHz;
    e_spi_endiannes_t endianness;
    e_spi_mode_t mode;
    SPISettings(uint32_t freqHz, e_spi_endiannes_t endianness, e_spi_mode_t mode);
};

/** 
 * @brief Arduino's SPIclass porting. Used for communication with mcp2518.*/
class SPIClass {
public:
    void beginTransaction(SPISettings settings);
    void endTransaction(void);
    void begin(void);
    uint8_t transfer(uint8_t tx_data);
};
extern SPIClass SPI;
