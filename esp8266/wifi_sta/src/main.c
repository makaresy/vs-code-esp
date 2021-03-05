#include <stdio.h>
#include "ets_sys.h"
#include "osapi.h"
#include "user_interface.h"
#include "gpio.h"
#include "driver/uart.h"

#define LED     2
#define LED_ON  0
#define LED_OFF 1


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

static os_timer_t ptimer;

static uint8_t wifiStatus = STATION_IDLE;
static uint8_t connectStatus = 0;
static uint8_t led_state = 0;

static void ICACHE_FLASH_ATTR 
wifi_check_ip(void *arg){
    struct ip_info ipConfig;
    os_timer_disarm(&ptimer);
    wifiStatus = wifi_station_get_connect_status();
    if (wifiStatus == STATION_GOT_IP && ipConfig.ip.addr != 0) 
    {
        os_timer_setfn(&ptimer, (os_timer_func_t*)wifi_check_ip, NULL);   
        os_timer_arm(&ptimer, 2000, 0);
    if (connectStatus == 0)
        {
        os_printf("***************************STATION WIFI CONNECTED\r\n");
        connectStatus = 1;
        }
    else {    
        led_state = (led_state==0) ? 1 : 0;
        GPIO_OUTPUT_SET(LED, led_state);
        }
    }
    else {
            connectStatus = 0;
            GPIO_OUTPUT_SET(LED, LED_OFF);
        if (wifi_station_get_connect_status() == STATION_WRONG_PASSWORD)
            {
              os_printf("***************************icorrect password\r\n"); 
              wifi_station_connect(); 
            }
        else if (wifi_station_get_connect_status() == STATION_NO_AP_FOUND)
            {
              os_printf("***************************no AP found\r\n"); 
              wifi_station_connect();
            }
       else if (wifi_station_get_connect_status() == STATION_CONNECT_FAIL)
            {
              os_printf("***************************Connect Fail\r\n");   
              wifi_station_connect(); 
            }
        else {
               os_printf("*STATION_IDLE\r\n"); 
             }

        os_timer_setfn(&ptimer, (os_timer_func_t *)wifi_check_ip, NULL);
        os_timer_arm(&ptimer, 500, 0);
    }

}

void ICACHE_FLASH_ATTR 
user_init(void)
{
    uart_init(BIT_RATE_115200, BIT_RATE_115200);
    gpio_init();
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO2_U, FUNC_GPIO2);
    
    
    struct station_config stationConf;
    wifi_set_opmode_current(STATION_MODE);
    os_memset(&stationConf, 0, sizeof(struct station_config));                  //заполнение структуры конфигурации нулями
    os_sprintf(stationConf.ssid, "%s", WIFI_CLIENTSSID);                        //запись в структуру
    os_sprintf(stationConf.password, "%s", WIFI_CLIENTPASSWORD);                //
    wifi_station_set_config_current(&stationConf);                              //
    wifi_set_sleep_type(NONE_SLEEP_T);
    
    ets_delay_us(10000);
    os_printf("\r\n");
    
    os_timer_disarm(&ptimer);      
    os_timer_setfn(&ptimer, (os_timer_func_t*)wifi_check_ip, NULL); //Set timer callback function.
    os_timer_arm(&ptimer, 1000, 0);
    wifi_station_connect();

}
