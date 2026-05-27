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
        
    };

    spi_bus_initialize(SPI2_HOST, &spi_bus_config, SPI_DMA_DISABLED);
    //spi_bus_add_device(SPI2_HOST, )

    return ESP_OK;
}

void app_main(void)
{



}