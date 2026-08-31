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
#include "driver/spi_master.h"

#define mosi_pin 6
#define sclk_pin 5
#define cs_pin 7

#define NOT_USED -1

spi_device_handle_t MAX7221_HANDLE;

void app_main(void)
{
    gpio_set_direction(1, GPIO_MODE_OUTPUT);
    gpio_set_level(1, 1);
    
    spi_bus_config_t spi_bus_config = {
        .mosi_io_num = mosi_pin,
        .sclk_io_num = sclk_pin,
        .quadwp_io_num = NOT_USED,   
        .quadhd_io_num = NOT_USED,   
        .data4_io_num = NOT_USED,      
        .data5_io_num = NOT_USED,    
        .data6_io_num = NOT_USED,    
        .data7_io_num = NOT_USED,    
        .max_transfer_sz = SOC_SPI_MAXIMUM_BUFFER_SIZE,

    };

    spi_device_interface_config_t spi_device_interface_config = {
        .command_bits = 0,
        .address_bits = 0,
        .dummy_bits = 0,
        .mode = 0,
        .duty_cycle_pos = 128, 
        .clock_speed_hz = 10 * 1000000,
        .input_delay_ns = 0,
        .spics_io_num = cs_pin,
        .flags = 0,
        .queue_size = 9,
        
    };

    esp_err_t ret = spi_bus_initialize(SPI3_HOST, &spi_bus_config, SPI_DMA_DISABLED);
    vTaskDelay(pdMS_TO_TICKS(10));

    if (ret != ESP_OK) { 
        printf("Error al inicializar el bus SPI: %s\n", esp_err_to_name(ret)); 
        return; 
    }
    printf("Bus SPI inicializado correctamente en SPI3_HOST\n");

    ret = spi_bus_add_device(SPI3_HOST, &spi_device_interface_config, &MAX7221_HANDLE);
    vTaskDelay(pdMS_TO_TICKS(10));

    if (ret != ESP_OK) { 
        printf("Error al agregar el dispositivo SPI: %s\n", esp_err_to_name(ret)); 
        return; 
    }
    printf("Dispositivo MAX7221 agregado correctamente. Handle: %p\n", (void *)MAX7221_HANDLE);
    printf("Pines configurados -> MOSI: %d, SCLK: %d, CS: %d\n", mosi_pin, sclk_pin, cs_pin);
    
    vTaskDelay(pdMS_TO_TICKS(100));

    // --- CORRECCIÓN 1: Usar arrays de 2 bytes (no uint16_t) ---
    uint8_t despertar[2] = {0x0C, 0x01};   // Shutdown: Normal Operation (Despierta el chip)
    uint8_t test[2] = {0x0F, 0x01};        // Display Test: Activar (Enciende todos los LEDs)
    uint8_t apagar[2] = {0x0C, 0x00};      // Shutdown: Apagar
    uint8_t desactivar_test[2] = {0x0F, 0x00};

    // --- CORRECCIÓN 2: Enviar primero el comando para DESPERTAR, no para apagar ---
    spi_transaction_t transaction_despertar = {
        .length = 16,         
        .tx_buffer = despertar,  
        .rxlength = 0,
    };

    spi_transaction_t transaction_test = {
        .length = 16,         
        .tx_buffer = test,  
        .rxlength = 0,
    };

    spi_transaction_t transaction_apagar = {
        .length = 16,         
        .tx_buffer = apagar,  
        .rxlength = 0,
    };

    spi_transaction_t trans_desactivar_test = {
        .length = 16, 
        .tx_buffer = desactivar_test, 
        .rxlength = 0
    };
   
   
    while (1) {

        spi_device_acquire_bus(MAX7221_HANDLE, portMAX_DELAY);

        for (int i = 0; i < 2; i++) {
            spi_device_transmit(MAX7221_HANDLE, &trans_desactivar_test);
            spi_device_transmit(MAX7221_HANDLE, &transaction_apagar);
            vTaskDelay(pdMS_TO_TICKS(5000));
        }
        
        spi_device_release_bus(MAX7221_HANDLE);
        
        spi_device_acquire_bus(MAX7221_HANDLE, portMAX_DELAY);

        for (int i = 0; i < 2; i++) {
            spi_device_transmit(MAX7221_HANDLE, &transaction_despertar);
            spi_device_transmit(MAX7221_HANDLE, &transaction_test);
            vTaskDelay(pdMS_TO_TICKS(5000));
        }
        spi_device_release_bus(MAX7221_HANDLE);

        
        

    }

}

