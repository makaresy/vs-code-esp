#include "pcf8574.h"
#include "driver/i2c_master.h"
#include "osapi.h"


void write_byte (uint8_t byte) {

    i2c_master_start();
    i2c_master_writeByte(I2C_PCF8574_ADR | WRITE);
    i2c_master_checkAck();
    i2c_master_writeByte(byte);
    i2c_master_checkAck();
    i2c_master_stop(); 

}

void ICACHE_FLASH_ATTR 
i2c_init (void){

    gpio_init();
    i2c_master_gpio_init();
    i2c_master_init();
    i2c_master_wait (100);
}