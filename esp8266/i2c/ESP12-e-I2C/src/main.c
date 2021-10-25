#include "osapi.h"
#include "user_interface.h"
#include "driver/i2c_master.h"
#include "pcf8574.h"


#define READ        0x01
#define WRITE       0x00

uint32 ICACHE_FLASH_ATTR
user_rf_cal_sector_set(void)
{
    enum flash_size_map size_map = system_get_flash_size_map();
    uint32 rf_cal_sec = 0;
 
    switch (size_map) {
        case FLASH_SIZE_4M_MAP_256_256:
            rf_cal_sec = 128 - 5;
            break;
 
        case FLASH_SIZE_8M_MAP_512_512:
            rf_cal_sec = 256 - 5;
            break;
 
        case FLASH_SIZE_16M_MAP_512_512:
        case FLASH_SIZE_16M_MAP_1024_1024:
            rf_cal_sec = 512 - 5;
            break;
 
        case FLASH_SIZE_32M_MAP_512_512:
        case FLASH_SIZE_32M_MAP_1024_1024:
            rf_cal_sec = 1024 - 5;
            break;
 
        case FLASH_SIZE_64M_MAP_1024_1024:
            rf_cal_sec = 2048 - 5;
            break;
        case FLASH_SIZE_128M_MAP_1024_1024:
            rf_cal_sec = 4096 - 5;
            break;
        default:
            rf_cal_sec = 0;
            break;
    }
    return rf_cal_sec;
}
void ICACHE_FLASH_ATTR user_init(void)
{
    gpio_init();
    i2c_master_gpio_init();
    i2c_master_init();
    i2c_master_wait (100);

    i2c_master_start();
    i2c_master_writeByte(I2C_PCF8574_ADR | WRITE);
    i2c_master_checkAck();
    i2c_master_writeByte(0xFF);
     i2c_master_checkAck();
    i2c_master_stop(); 
}
