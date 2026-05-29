#ifndef LS7366R_H
#define LS7366R_H

#include "driver/spi_master.h"

//Registers
#define MDR0 0x8
#define MDR1 0X10
#define DTR  0X18
#define CNTR 0X20
#define OTR  0X28
#define STR  0X30

//Instructions
#define CLEAR       0x00   // 00000000
#define READ_FROM   0x40   // 01000000
#define WRITE_TO    0x80   // 10000000
#define LOAD_TO     0xC0   // 11000000

// Configurations
#define SET_OVERFLOW_FLAG 0x80 // FLAG on overflow

// Frequency for SPI comunication
#define FREQ_8M      (80 * 1000 * 1000 / 10)   ///< 8MHz
#define FREQ_9M      (80 * 1000 * 1000 / 9)    ///< 8.89MHz
#define FREQ_10M     (80 * 1000 * 1000 / 8)    ///< 10MHz
#define FREQ_11M     (80 * 1000 * 1000 / 7)    ///< 11.43MHz
#define FREQ_13M     (80 * 1000 * 1000 / 6)    ///< 13.33MHz
#define FREQ_16M     (80 * 1000 * 1000 / 5)    ///< 16MHz
#define FREQ_20M     (80 * 1000 * 1000 / 4)    ///< 20MHz
#define FREQ_26M     (80 * 1000 * 1000 / 3)    ///< 26.67MHz
#define FREQ_40M     (80 * 1000 * 1000 / 2)    ///< 40MHz
#define FREQ_80M     (80 * 1000 * 1000 / 1)    ///< 80MHz

typedef spi_device_handle_t ls7366r_handle_t;

// SPI pin configuration structure
typedef struct {
    uint8_t miso_pin;
    uint8_t mosi_pin;
    uint8_t sclk_pin;
    uint8_t cs_pin;
    int frequency;

} ls7366r_spi_conf;

esp_err_t init_ls7366r_spi_com(void);
    
#endif