#include <stdio.h>
#include "ets_sys.h"
#include "osapi.h"
#include "user_interface.h"
#include "gpio.h"
#include "driver/uart.h"
#include "espconn.h"
#include "sntp.h"

#define LED     2

#define WIFI_LED_IO_MUX    PERIPHS_IO_MUX_GPIO2_U
#define WIFI_LED_IO_FUNC   FUNC_GPIO2  

static os_timer_t ptimer;




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

void ICACHE_FLASH_ATTR 
user_init(void)
{
    uart_init(BIT_RATE_115200, BIT_RATE_115200);
    
    struct softap_config SoftApConfig;
    os_memcpy(&SoftApConfig.ssid, SSID, os_strlen(SSID));
    os_memcpy(&SoftApConfig.password, PASS, os_strlen(PASS));

    wifi_set_opmode_current(STATIONAP_MODE);
    wifi_station_set_username(USERNAME, os_strlen(USERNAME));
    wifi_softap_set_config_current(&SoftApConfig);
    //wifi_wps_disable();
    wifi_station_connect();
    wifi_status_led_install(LED, WIFI_LED_IO_MUX, FUNC_GPIO2);
    os_timer_disarm(&ptimer);      
    //os_timer_setfn(&ptimer, (os_timer_func_t*)wifi_check_ip, NULL); //Set timer callback function.
    os_timer_arm(&ptimer, 1000, 0);   

    os_printf("USER_name_strLen %d \r\n", os_strlen("COVID_ESP"));
    os_printf("USER_name_strLen %d \r\n", sizeof("COVID_ESP"));
    os_printf("soft_ap_config.ssid %s \r\n", &SoftApConfig.ssid);
    os_printf("soft_ap_config.pass %s \r\n", &SoftApConfig.password);
}
