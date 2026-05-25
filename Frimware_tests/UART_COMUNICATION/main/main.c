#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#define led_pin 2

void app_main(void)
{

    while(1){
        printf("Hello world!\n");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

}