#ifndef  PCF8574_H
#define  PCF8574_H

#define I2C_PCF8574_ADR     0x4E
#define PCF_8574_LED_ON     0x08
#define PCF_8574_LED_OFF    (PCF_8574_LED_ON & 0x00)




typedef struct
 {
   __IO uint32_t CRL;
   __IO uint32_t CRH;
   __IO uint32_t IDR;
   __IO uint32_t ODR;
   __IO uint32_t BSRR;
   __IO uint32_t BRR;
   __IO uint32_t LCKR;
} GPIO_TypeDef;

#define Lcd_PortType GPIO_TypeDef*

  Lcd_PortType ports[] = {
		  D4_GPIO_Port, D5_GPIO_Port, D6_GPIO_Port, D7_GPIO_Port 
  };

typedef struct {
	Lcd_PortType * data_port;
	Lcd_PinType * data_pin;

	Lcd_PortType rs_port;
	Lcd_PinType rs_pin;

	Lcd_PortType en_port;
	Lcd_PinType en_pin;

	Lcd_ModeTypeDef mode;

} Lcd_HandleTypeDef;

Lcd_HandleTypeDef Lcd_create(
		Lcd_PortType port[], Lcd_PinType pin[],
		Lcd_PortType rs_port, Lcd_PinType rs_pin,
		Lcd_PortType en_port, Lcd_PinType en_pin, Lcd_ModeTypeDef mode)
{
	Lcd_HandleTypeDef lcd;

	lcd.mode = mode;

	lcd.en_pin = en_pin;
	lcd.en_port = en_port;

	lcd.rs_pin = rs_pin;
	lcd.rs_port = rs_port;

	lcd.data_pin = pin;
	lcd.data_port = port;

	Lcd_init(&lcd);

	return lcd;
}


#endif

#define PERIPH_BASE           0x40000000U
#define AHB1PERIPH_BASE       (PERIPH_BASE + 0x00020000U)
#define GPIOD_BASE            (AHB1PERIPH_BASE + 0x0C00U)
#define GPIOD                 ((GPIO_TypeDef *) GPIOD_BASE)

#define D4_GPIO_Port GPIOD

  Lcd_PortType ports[] = {
		  D4_GPIO_Port, D5_GPIO_Port, D6_GPIO_Port, D7_GPIO_Port 
  };

  #define Lcd_PortType GPIO_TypeDef*

  typedef struct
 {
   __IO uint32_t CRL;
   __IO uint32_t CRH;
   __IO uint32_t IDR;
   __IO uint32_t ODR;
   __IO uint32_t BSRR;
   __IO uint32_t BRR;
   __IO uint32_t LCKR;
} GPIO_TypeDef;