#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "freertos/task.h"
#include "driver/spi_master.h"

#define MISO 13
#define MOSI 11
#define SCLK 12
#define CS 10
#define NOT_USED -1

spi_device_handle_t spi_handle;

//Registers

#define MDR0 0x8
#define MDR1 0X10
#define DTR  0X18
#define CNTR 0X20
#define OTR  0X28
#define STR  0X30

//Instructions

#define CLR   0x00   // 00000000
#define RD    0x40   // 01000000
#define WRITE_TO    0x80   // 10000000
#define LOAD  0xC0   // 11000000

static esp_err_t init_spi(void){

    spi_bus_config_t spi_bus_config = {
        .miso_io_num = MISO,
        .mosi_io_num = MOSI,
        .sclk_io_num = SCLK,
        .quadwp_io_num = NOT_USED,   
        .quadhd_io_num = NOT_USED,   
        .data4_io_num = NOT_USED,      
        .data5_io_num = NOT_USED,    
        .data6_io_num = NOT_USED,    
        .data7_io_num = NOT_USED,    
        .max_transfer_sz = 8,

    };

    spi_device_interface_config_t spi_device_interface_config = {
        .command_bits = 8,
        .address_bits = 0,
        .dummy_bits = 0,
        .mode = 0,
        .duty_cycle_pos = 128, 
        .clock_speed_hz = SPI_MASTER_FREQ_8M,
        .input_delay_ns = 0,
        .spics_io_num = CS,
        .flags = SPI_DEVICE_HALFDUPLEX,
        .queue_size = 1,
        
    };

    spi_bus_initialize(SPI2_HOST, &spi_bus_config, SPI_DMA_DISABLED);
    spi_bus_add_device(SPI2_HOST, &spi_device_interface_config, &spi_handle);

    return ESP_OK;
}

static void SPI_WRITE_COMAND(uint8_t command) {
    spi_transaction_t transaction = {
        .cmd = command,      
        .length = 0,         
    };

    spi_device_transmit(spi_handle, &transaction);
}

static void SPI_WRITE_COMAND_AND_DATA(uint8_t command, uint32_t data, uint8_t bits_length) {
    spi_transaction_t transaction = {
        .cmd = command,      
        .length = bits_length,         
        .tx_buffer = &data,  
    };

    spi_device_transmit(spi_handle, &transaction);
}

static uint32_t SPI_READ(uint8_t command, uint8_t bits_length) {
    uint32_t rx_data = 0;
    
    spi_transaction_t transaction = {
        .cmd = command,      
        .length = 0,                //0 porque no se envia nada            
        .rxlength = bits_length,    //tamaño 
        .rx_buffer = &rx_data,      //donde se recivira lo guardado
    };
    
    spi_device_transmit(spi_handle, &transaction);
    return rx_data;
}

void app_main(void)
{

    init_spi();
    vTaskDelay(pdMS_TO_TICKS(100));

    SPI_WRITE_COMAND_AND_DATA(      // writing to MDR REG, non-quadrature mode, free runing mode, disable index
        WRITE_TO | MDR0, 0x01, 8);
        
        
    vTaskDelay(pdMS_TO_TICKS(10));
    
    
    SPI_WRITE_COMAND_AND_DATA(0x90, 0xF0, 8);  
    vTaskDelay(pdMS_TO_TICKS(10));
    
    SPI_WRITE_COMAND(0x20);  
    vTaskDelay(pdMS_TO_TICKS(10));

    uint32_t pulsos_acumulados = 1000;
    
    while (1) {
        pulsos_acumulados = SPI_READ(0x60, 32);
        
        
        printf("Pulsos acumulados: %lu\n", pulsos_acumulados);
        
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

}