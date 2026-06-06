#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define LFLAG_PIN GPIO_NUM_4

// Variable para detectar la interrupción
volatile bool interrupcion_ocurrio = false;

// ISR (se ejecuta cuando el pin cambia de HIGH a LOW)
static void IRAM_ATTR isr_handler(void *arg)
{
    interrupcion_ocurrio = true;
}

void app_main(void)
{
    // Configurar el pin como entrada con pull-up
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << LFLAG_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_NEGEDGE,  // Flanco descendente (HIGH -> LOW)
    };
    gpio_config(&io_conf);
    
    // Instalar el servicio de interrupciones
    gpio_install_isr_service(ESP_INTR_FLAG_IRAM);
    
    // Asignar el handler al pin
    gpio_isr_handler_add(LFLAG_PIN, isr_handler, NULL);
    
    int num = 0;
    // Loop infinito
    while (1) {
        if (interrupcion_ocurrio) {
            //while(gpio_get_level(LFLAG_PIN) == 0);
            interrupcion_ocurrio = false;
            printf("¡INTERRUPCIÓN DETECTADA! %d \n", num++);
        }
        
        vTaskDelay(pdMS_TO_TICKS(10));  // Pequeña pausa para no saturar
    }
}