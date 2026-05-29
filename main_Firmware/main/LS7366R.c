#ifndef LS7366R_C
#define LS7366R_C

#include "LS7366R.h"

#define NOT_USED -1

esp_err_t init_ls7366r_spi_com(ls7366r_spi_conf *conf){
    
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
    error = spi_bus_add_device(SPI2_HOST, &spi_device_interface_config, &LS7366R_1);

    return error;
}


#endif