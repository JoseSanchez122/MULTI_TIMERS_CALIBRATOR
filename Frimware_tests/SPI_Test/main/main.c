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

static esp_err_t init_spi(void){

    spi_bus_config_t spi_bus_config = {
        
        .miso_io_num = MISO,
        .mosi_io_num = MOSI,
        .sclk_io_num = SCLK,
        
    }


    spi_bus_initialize(SPI2_HOST, )
    return ESP_OK
}

void app_main(void)
{



}