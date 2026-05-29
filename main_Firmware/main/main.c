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


void app_main(void)
{
    ls7366r_spi_conf conf = {
        .miso_pin = MISO,
        .mosi_pin = MOSI,
        .sclk_pin = SCLK,
        .cs_pin = CS,
        .frequency = FREQ_8M
    };

    esp_err_t error;
    error = init_ls7366r_spi_com(&conf, &LS7366R_1);

    printf("error: %d\n", error); 
    vTaskDelay(pdMS_TO_TICKS(100));

    LS7366R_WRITE_COMAND_AND_DATA(      // writing to MDR0 REG, non-quadrature mode, free runing mode, disabled index
        WRITE_TO | MDR0, 0x0, 8, LS7366R_1);
        
        
    vTaskDelay(pdMS_TO_TICKS(10));
    
    
    LS7366R_WRITE_COMAND_AND_DATA(WRITE_TO | MDR1, SET_OVERFLOW_FLAG, 8, LS7366R_1);  
    vTaskDelay(pdMS_TO_TICKS(10));

    LS7366R_WRITE_COMAND(CLEAR | CNTR, LS7366R_1);  
    vTaskDelay(pdMS_TO_TICKS(10));

    uint32_t pulsos_acumulados = 0;
    
    while (1) {
        pulsos_acumulados = LS7366R_READ(READ_FROM | CNTR, 32, LS7366R_1);
        
        
        printf("Pulsos acumulados: %lu\n", pulsos_acumulados);
        
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

}