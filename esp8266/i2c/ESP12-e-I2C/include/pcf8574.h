#ifndef  PCF8574_H
#define  PCF8574_H

#include "c_types.h"

#define I2C_PCF8574_ADR     0x4E

#define PCF_8574_LED_ON     0x08
#define PCF_8574_LED_OFF    (PCF_8574_LED_ON & 0x00)

#define READ        0x01
#define WRITE       0x00


typedef enum {
	LCD_8_BIT = 0x40,
	LCD_4_BIT = 0x40
}Lcd_ModeTypeDef;


void write_byte (uint8_t byte);

void ICACHE_FLASH_ATTR 
i2c_init(void);

#endif

