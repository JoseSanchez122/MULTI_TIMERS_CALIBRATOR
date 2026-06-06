// set generator to 1.65 vpp
// set offset to 825 mv

//MY generator vpp = 2.6, offs = 1.2

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
        .frequency = FREQ_10M
    };

    esp_err_t error;
    error = init_ls7366r_spi_com(&conf, &LS7366R_1);

    printf("error: %d\n", error); 
    vTaskDelay(pdMS_TO_TICKS(100));

    LS7366R_WRITE_COMAND_AND_DATA(      // writing to MDR0 REG, non-quadrature mode, free runing mode, tansfer_CNTR_to_OTR
        (WRITE_TO | MDR0), 
        INDEX_LOAD_OTR, 
        Bits_8, 
        LS7366R_1);
        
    vTaskDelay(pdMS_TO_TICKS(10));
    
    LS7366R_WRITE_COMAND_AND_DATA(WRITE_TO | MDR1, SET_OVERFLOW_FLAG, Bits_8, LS7366R_1);  
    vTaskDelay(pdMS_TO_TICKS(10));

    LS7366R_WRITE_COMAND(CLEAR | CNTR, LS7366R_1);  
    vTaskDelay(pdMS_TO_TICKS(10));
    LS7366R_WRITE_COMAND(CLEAR | OTR, LS7366R_1);  

    uint32_t pulsos_acumulados = 152;
    
    while (1) {
        pulsos_acumulados = LS7366R_READ(READ_FROM | OTR, Bits_32, LS7366R_1);
        
        
        printf("Pulsos acumulados: %lu\n", pulsos_acumulados);
        
        vTaskDelay(pdMS_TO_TICKS(500));
    }

}