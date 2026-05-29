// set generator to 1.65 vpp
// set offset to 825 mv

#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "freertos/task.h"

#include "LS7366R.h"

#define MISO 13
#define MOSI 11
#define SCLK 12
#define CS 10


ls7366r_handle_t LS7366R_1;



static void SPI_WRITE_COMAND(uint8_t command) {
    spi_transaction_t transaction = {
        .cmd = command,      
        .length = 0,         
    };

    spi_device_transmit(LS7366R_1, &transaction);
}

static void SPI_WRITE_COMAND_AND_DATA(uint8_t command, uint32_t data, uint8_t bits_length) {
    spi_transaction_t transaction = {
        .cmd = command,      
        .length = bits_length,         
        .tx_buffer = &data,  
    };

    spi_device_transmit(LS7366R_1, &transaction);
}

static uint32_t SPI_READ(uint8_t command, uint8_t bits_length) {
    uint32_t rx_data = 0;
    
    spi_transaction_t transaction = {
        .cmd = command,      
        .length = 0,                //0 porque no se envia nada            
        .rxlength = bits_length,    //tamaño 
        .rx_buffer = &rx_data,      //donde se recivira lo guardado
    };
    
    spi_device_transmit(LS7366R_1, &transaction);
    return __builtin_bswap32(rx_data);;
}

void app_main(void)
{
    esp_err_t error;
    error = init_spi();

    printf("error: %d\n", error); 
    vTaskDelay(pdMS_TO_TICKS(100));

    SPI_WRITE_COMAND_AND_DATA(      // writing to MDR0 REG, non-quadrature mode, free runing mode, disabled index
        WRITE_TO | MDR0, 0x0, 8);
        
        
    vTaskDelay(pdMS_TO_TICKS(10));
    
    
    SPI_WRITE_COMAND_AND_DATA(WRITE_TO | MDR1, SET_OVERFLOW_FLAG, 8);  
    vTaskDelay(pdMS_TO_TICKS(10));

    SPI_WRITE_COMAND_AND_DATA(CLEAR | CNTR, SET_OVERFLOW_FLAG, 8);  
    vTaskDelay(pdMS_TO_TICKS(10));

    uint32_t pulsos_acumulados = 0;
    
    while (1) {
        pulsos_acumulados = SPI_READ(READ_FROM | CNTR, 32);
        
        
        printf("Pulsos acumulados: %lu\n", pulsos_acumulados);
        
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

}