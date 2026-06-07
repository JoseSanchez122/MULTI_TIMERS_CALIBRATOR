// set generator to 1.65 vpp
// set offset to 825 mv

//MY generator vpp = 2.6, offs = 1.2

#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "freertos/task.h"
#include "LS7366R.h"
#include "esp_task_wdt.h"

#define MISO 13
#define MOSI 11
#define SCLK 12
#define CS 10

#define LFLAG_PIN GPIO_NUM_5

ls7366r_handle_t LS7366R_1;
volatile BaseType_t  ISR_HAPPENED = false;

static void IRAM_ATTR isr_handler(void *arg)
{
    ISR_HAPPENED = true;
}

void app_main(void)
{
    ESP_ERROR_CHECK( esp_task_wdt_deinit() );

    ls7366r_spi_conf conf = {
        .miso_pin = MISO,
        .mosi_pin = MOSI,
        .sclk_pin = SCLK,
        .cs_pin = CS,
        .frequency = FREQ_10M
    };


    init_ls7366r_spi_com(&conf, &LS7366R_1);
    vTaskDelay(pdMS_TO_TICKS(100));

    LS7366R_WRITE_COMAND_AND_DATA(      // writing to MDR0 REG, non-quadrature mode, free runing mode, tansfer_CNTR_to_OTR
        (WRITE_TO | MDR0), 
        INDEX_LOAD_OTR, 
        Bits_8, 
        LS7366R_1);
        
    vTaskDelay(pdMS_TO_TICKS(10));
    
    LS7366R_WRITE_COMAND_AND_DATA(
        WRITE_TO | MDR1, 
        FLAG_ON_IDX | SET_OVERFLOW_FLAG, 
        Bits_8, 
        LS7366R_1); 

    vTaskDelay(pdMS_TO_TICKS(10));

    LS7366R_WRITE_COMAND(CLEAR | CNTR, LS7366R_1);  
    vTaskDelay(pdMS_TO_TICKS(10));

    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << LFLAG_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_NEGEDGE,  
    };
    gpio_config(&io_conf);
    
    // Instalar el servicio de interrupciones
    gpio_install_isr_service(ESP_INTR_FLAG_IRAM);
    
    // Asignar el handler al pin
    gpio_isr_handler_add(LFLAG_PIN, isr_handler, NULL);

    uint32_t pulsos_acumulados = 0;
    
    while (1) {

        if (ISR_HAPPENED) {
            ISR_HAPPENED = false;
            pulsos_acumulados = LS7366R_READ(READ_FROM | OTR, Bits_32, LS7366R_1);
            printf("Pulsos acumulados: %lu\n", pulsos_acumulados);
            LS7366R_WRITE_COMAND(CLEAR | STR, LS7366R_1); 
        }
          
        //vTaskDelay(pdMS_TO_TICKS(500));
    }

}