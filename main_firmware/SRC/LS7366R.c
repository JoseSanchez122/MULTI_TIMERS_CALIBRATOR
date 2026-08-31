#ifndef LS7366R_C
#define LS7366R_C
 
#include "LS7366R.h"

#define NOT_USED -1

esp_err_t init_ls7366r_spi_com(ls7366r_spi_conf *conf, ls7366r_handle_t *ls7366r_handle){
    
    esp_err_t error;

    spi_bus_config_t spi_bus_config = {
        .miso_io_num = conf -> miso_pin,
        .mosi_io_num = conf -> mosi_pin,
        .sclk_io_num = conf -> sclk_pin,
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
        .clock_speed_hz = conf -> frequency,
        .input_delay_ns = 0,
        .spics_io_num = conf -> cs_pin,
        .flags = SPI_DEVICE_HALFDUPLEX,
        .queue_size = 1,
        
    };

    error = spi_bus_initialize(SPI2_HOST, &spi_bus_config, SPI_DMA_DISABLED);
    error = spi_bus_add_device(SPI2_HOST, &spi_device_interface_config, ls7366r_handle);

    return error;
}

esp_err_t LS7366R_WRITE_COMAND(uint8_t command, ls7366r_handle_t ls7366r_handle) {
    spi_transaction_t transaction = {
        .cmd = command,      
        .length = 0,         
    };

    return spi_device_transmit(ls7366r_handle, &transaction);
}

esp_err_t LS7366R_WRITE_COMAND_AND_DATA(uint8_t command, uint32_t data, uint8_t bits_length, ls7366r_handle_t ls7366r_handle) {
    spi_transaction_t transaction = {
        .cmd = command,      
        .length = bits_length,         
        .tx_buffer = &data,  
    };

    return spi_device_transmit(ls7366r_handle, &transaction);
}

uint32_t LS7366R_READ(uint8_t command, uint8_t bits_length, ls7366r_handle_t ls7366r_handle) {
    uint32_t rx_data = 0;
    
    spi_transaction_t transaction = {
        .cmd = command,      
        .length = 0,                //0 porque no se envia nada            
        .rxlength = bits_length,    //tamaño 
        .rx_buffer = &rx_data,      //donde se recivira lo guardado
    };
    
    spi_device_transmit(ls7366r_handle, &transaction);
    return __builtin_bswap32(rx_data);  //data shifted from Big endian to little endian 
}

#endif

//-------------------------------------------------------------------------------------------------------------//
/*
#include "sdkconfig.h"

#include <freertos/FreeRTOS.h>
#include <esp_check.h>

#include "max7219_7221.h"

const char* TAG = "max72[19|21]_main";

//
// NOTE: For maximum performance, prefer IO MUX over GPIO Matrix routing
//  * See https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/spi_master.html#gpio-matrix-routing
//

// SPI Host ID
const spi_host_device_t SPI_HOSTID = SPI2_HOST;

// SPI pins - Depends on the chip and the board
#if CONFIG_IDF_TARGET_ESP32
const gpio_num_t CS_LOAD_PIN = GPIO_NUM_19;
const gpio_num_t CLK_PIN = GPIO_NUM_18;
const gpio_num_t DIN_PIN = GPIO_NUM_16;
#else
#if CONFIG_IDF_TARGET_ESP32S3
const gpio_num_t CS_LOAD_PIN = GPIO_NUM_10;
const gpio_num_t CLK_PIN = GPIO_NUM_12;
const gpio_num_t DIN_PIN = GPIO_NUM_11;
#else
#if CONFIG_IDF_TARGET_ESP32C3
const gpio_num_t CS_LOAD_PIN = GPIO_NUM_1;
const gpio_num_t CLK_PIN = GPIO_NUM_2;
const gpio_num_t DIN_PIN = GPIO_NUM_3;
#else
#if CONFIG_IDF_TARGET_ESP32H2
const gpio_num_t CS_LOAD_PIN = GPIO_NUM_12;
const gpio_num_t CLK_PIN = GPIO_NUM_22;
const gpio_num_t DIN_PIN = GPIO_NUM_25;
#endif
#endif
#endif
#endif

// Number of devices MAX7219 / MAX7221 in the chain
const uint8_t ChainLength = 3;

// Time between two display update
const TickType_t DelayBetweenUpdates = pdMS_TO_TICKS(1000);



// Handle to the  MAX7219 / MAX7221 driver
led_driver_max7219_handle_t led_max7219_handle = NULL;



void app_main(void) {
    // Configure SPI bus to communicate with MAX7219 / MAX7221
    spi_bus_config_t spiBusConfig = {
        .mosi_io_num = DIN_PIN,
        .miso_io_num = GPIO_NUM_NC,
        .sclk_io_num = CLK_PIN,

        .data2_io_num = GPIO_NUM_NC,
        .data3_io_num = GPIO_NUM_NC,

        .max_transfer_sz = SOC_SPI_MAXIMUM_BUFFER_SIZE,
        .flags = SPICOMMON_BUSFLAG_MASTER,
        .isr_cpu_id = ESP_INTR_CPU_AFFINITY_AUTO
    };
    ESP_ERROR_CHECK(spi_bus_initialize(SPI_HOSTID, &spiBusConfig, SPI_DMA_CH_AUTO));

    do {
        // Initialize the MAX7219 / MAX7221 driver
        max7219_config_t max7219InitConfig = {
            .spi_cfg = {
                .host_id = SPI_HOSTID,

                .clock_source = SPI_CLK_SRC_DEFAULT,
                .clock_speed_hz = 10 * 1000000,

                .spics_io_num = CS_LOAD_PIN,
                .queue_size = 8
            },
            .hw_config = {
                .chain_length = ChainLength
            }
        };
        ESP_LOGI(TAG, "Initialize MAX 7219 / 7221 driver");
        ESP_ERROR_CHECK(led_driver_max7219_init(&max7219InitConfig, &led_max7219_handle));
        // NOTE: On power on, the MAX7219 / MAX7221 starts in shutdown mode - All blank, scan mode is 1 digit, no CODE B decode, intensity is minimum

        // Switch to 'test' mode - This turns all segments on all displays ON at maximum intensity
        ESP_LOGI(TAG, "Set Test mode");
        ESP_ERROR_CHECK(led_driver_max7219_set_chain_mode(led_max7219_handle, MAX7219_TEST_MODE));

        // Configure scan limit on all devices
        ESP_LOGI(TAG, "Configure scan limit to all digits (8)");
        ESP_ERROR_CHECK(led_driver_max7219_configure_chain_scan_limit(led_max7219_handle, 8));

        // Configure decode mode to 'decode for all digits'
        ESP_LOGI(TAG, "Configure decode for Code B on all digits in the chain");
        ESP_ERROR_CHECK(led_driver_max7219_configure_chain_decode(led_max7219_handle, MAX7219_CODE_B_DECODE_ALL));

        // Set intensity on all devices - MAX7219_INTENSITY_DUTY_CYCLE_STEP_2 is dim
        ESP_LOGI(TAG, "Set intensity to 'MAX7219_INTENSITY_DUTY_CYCLE_STEP_2' on all devices in the chain");
        ESP_ERROR_CHECK(led_driver_max7219_set_chain_intensity(led_max7219_handle, MAX7219_INTENSITY_DUTY_CYCLE_STEP_2));

        // Reset all digits to 'blank' for a clean visual effect - We use MAX7219_CODE_B_BLANK since we configured Code B decode
        // When the MAX7219 / MAX7221 is put in test mode, it preserves whatever digits were programmed before
        // If no digits were programmed before entering test mode, the MAX7219 / MAX7221 will load '8' in all digits
        ESP_LOGI(TAG, "Set all digits to blank");
        ESP_ERROR_CHECK(led_driver_max7219_set_chain_digit(led_max7219_handle, MAX7219_CODE_B_BLANK));


        // Hold 'test' mode for a little while
        vTaskDelay(DelayBetweenUpdates);

        // Switch to 'normal' mode so digits can be displayed and hold 'all blank' for a little while
        ESP_LOGI(TAG, "Set Normal mode");
        ESP_ERROR_CHECK(led_driver_max7219_set_chain_mode(led_max7219_handle, MAX7219_NORMAL_MODE));
        vTaskDelay(DelayBetweenUpdates);


        // Display '8' sequentially on all digits of all devices
        for (uint8_t chainId = 1; chainId <= ChainLength; chainId++) {
            for (uint8_t digitId = MAX7219_MIN_DIGIT; digitId <= MAX7219_MAX_DIGIT; digitId++) {
                ESP_LOGI(TAG, "Device %d: Set digit index %d to 'MAX7219_CODE_B_8'", chainId, digitId);
                ESP_ERROR_CHECK(led_driver_max7219_set_digit(led_max7219_handle, chainId, digitId, MAX7219_CODE_B_8));
                vTaskDelay(DelayBetweenUpdates);
            }
        }
        
        vTaskDelay(2 * DelayBetweenUpdates);

        // Configure decode mode to 'direct addressing'
        ESP_LOGI(TAG, "Configure decode for Direct Addressing on all digits in the chain");
        ESP_ERROR_CHECK(led_driver_max7219_configure_chain_decode(led_max7219_handle, MAX7219_CODE_B_DECODE_NONE));

        // Blank all digits on the chain
        ESP_ERROR_CHECK(led_driver_max7219_set_chain_digit(led_max7219_handle, MAX7219_DIRECT_ADDRESSING_BLANK));

        // Show lower case direct addressing symbols
        uint8_t digits[] = {
            MAX7219_DIRECT_ADDRESSING_b,
            MAX7219_DIRECT_ADDRESSING_d,
            MAX7219_DIRECT_ADDRESSING_h,
            MAX7219_DIRECT_ADDRESSING_o,
            MAX7219_DIRECT_ADDRESSING_r,
            MAX7219_DIRECT_ADDRESSING_t,
            MAX7219_DIRECT_ADDRESSING_u,
            MAX7219_DIRECT_ADDRESSING_y
        };
        const uint8_t digitCount = sizeof(digits) / sizeof(digits[0]);
        ESP_ERROR_CHECK(led_driver_max7219_set_digits(led_max7219_handle, 1, 1, digits, digitCount));

         vTaskDelay(2 * DelayBetweenUpdates);

        // Blank all digits on the chain
        ESP_ERROR_CHECK(led_driver_max7219_set_chain_digit(led_max7219_handle, MAX7219_DIRECT_ADDRESSING_BLANK));

        // Shutdown MAX7219 / MAX7221 driver and SPI bus
        ESP_ERROR_CHECK(led_driver_max7219_free(led_max7219_handle));
        led_max7219_handle = NULL;
    } while (true);

    ESP_ERROR_CHECK(spi_bus_free(SPI_HOSTID));
} */