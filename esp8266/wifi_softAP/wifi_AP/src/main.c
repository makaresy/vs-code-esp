#include <stdio.h>
#include "ets_sys.h"
#include "osapi.h"
#include "user_interface.h"
#include "gpio.h"
#include "driver/uart.h"
#include "espconn.h"
#include "sntp.h"
#include "driver/i2c_master.h"

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

uint32 ICACHE_FLASH_ATTR
start_wifi_AP(void){

    struct softap_config SoftApConfig;
    struct ip_info ipinfo;
    uint32_t ip;
    char ip_char[15];
    uint8_t mac_addr[6];
    
    wifi_set_opmode_current(STATIONAP_MODE);
    wifi_get_macaddr(SOFTAP_IF, mac_addr);

    sprintf(SoftApConfig.ssid, "%s", AP_SSID);
    sprintf(SoftApConfig.password, "%s", AP_PASS);
    SoftApConfig.authmode = AUTH_WPA_WPA2_PSK;
    SoftApConfig.channel = 7;
    SoftApConfig.max_connection = 4;
    SoftApConfig.ssid_hidden = 0;
    wifi_softap_set_config(&SoftApConfig);

    wifi_set_sleep_type(NONE_SLEEP_T);
    wifi_softap_dhcps_stop();
    
    snprintf(ip_char, sizeof(ip_char), "%s", WIFI_AP_IP);
    ip = ipaddr_addr(ip_char);
    memcpy(&ipinfo.ip, &ip, 4);
    snprintf(ip_char, sizeof(ip_char), "%s", WIFI_AP_GW);
    ip = ipaddr_addr(ip_char);
    memcpy(&ipinfo.gw, &ip, 4);
    snprintf(ip_char, sizeof(ip_char), "%s", WIFI_AP_NETMASK);
    ip = ipaddr_addr(ip_char);
    memcpy(&ipinfo.netmask, &ip, 4);
    wifi_set_ip_info(SOFTAP_IF, &ipinfo);

    struct dhcps_lease dhcp_lease;
    snprintf(ip_char, sizeof(ip_char), "%s", WIFI_AP_IP_CLIENT_START);
    ip = ipaddr_addr(ip_char);
    memcpy(&dhcp_lease.start_ip, &ip, 4);
    snprintf(ip_char , sizeof(ip_char), "%s", WIFI_AP_IP_CLIENT_END);
    ip = ipaddr_addr(ip_char);
    memcpy(&dhcp_lease.end_ip, &ip, 4);
    wifi_softap_set_dhcps_lease(&dhcp_lease);


    wifi_softap_dhcps_start();
    wifi_set_phy_mode  (PHY_MODE_11N); 

}

void ICACHE_FLASH_ATTR 
user_init(void)
{
    uart_init(BIT_RATE_115200, BIT_RATE_115200);
    wifi_status_led_install(LED, WIFI_LED_IO_MUX, WIFI_LED_IO_FUNC);
    os_timer_disarm(&ptimer);      
    os_timer_setfn(&ptimer, (os_timer_func_t*)start_wifi_AP, NULL); //Set timer callback function.
    os_timer_arm(&ptimer, 1000, 0);   


}

