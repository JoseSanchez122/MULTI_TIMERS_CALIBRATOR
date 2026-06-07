#ifndef LS7366R_H
#define LS7366R_H

#include "driver/spi_master.h"

//Registers
#define MDR0 0x8
#define MDR1 0X10
#define DTR  0X18
#define CNTR 0X20 // real time counter register
#define OTR  0X28 // CNTR Back up register, for storing CNTR values without stoping CNTR to count.
#define STR  0X30 // status register, for cheking counter configurations and events that happend

//Instructions
#define CLEAR       0x00   // 00000000
#define READ_FROM   0x40   // 01000000
#define WRITE_TO    0x80   // 10000000
#define LOAD_TO     0xC0   // 11000000

//MDR1 Configurations
#define FLAG_ON_IDX         0x10 // flags activated when INDEX interruption ocurrs 
#define SET_OVERFLOW_FLAG   0x80 // FLAG on overflow

//MDR0 configurations
#define non_quadrature          0x00
#define free_runing             0x00
#define INDEX_LOAD_OTR          0x30



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

//Bit comunication info size
#define Bits_8 8
#define Bits_16 16
#define Bits_32 32

typedef spi_device_handle_t ls7366r_handle_t;

// SPI pin configuration structure
typedef struct {
    uint8_t miso_pin;
    uint8_t mosi_pin;
    uint8_t sclk_pin;
    uint8_t cs_pin;
    int frequency;

} ls7366r_spi_conf;

esp_err_t init_ls7366r_spi_com(ls7366r_spi_conf *conf, ls7366r_handle_t *ls7366r_handle);
esp_err_t LS7366R_WRITE_COMAND(uint8_t command, ls7366r_handle_t ls7366r_handle);
esp_err_t LS7366R_WRITE_COMAND_AND_DATA(uint8_t command, uint32_t data, uint8_t bits_length, ls7366r_handle_t ls7366r_handle);
uint32_t LS7366R_READ(uint8_t command, uint8_t bits_length, ls7366r_handle_t ls7366r_handle);
    
#endif