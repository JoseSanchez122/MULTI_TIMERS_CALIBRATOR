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