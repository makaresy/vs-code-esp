#include <stdio.h>
#include "ets_sys.h"
#include "osapi.h"
#include "user_interface.h"
#include "gpio.h"
#include "driver/uart.h"
#include "espconn.h"
#include "sntp.h"

#define LED     2
#define LED_ON  0
#define LED_OFF 1

#define WIFI_LED_IO_MUX    PERIPHS_IO_MUX_GPIO2_U
#define WIFI_LED_IO_FUNC   FUNC_GPIO2     

static os_timer_t ptimer;


static uint8_t connectStatus = 0;
static uint8_t led_state = 0;

typedef enum{               //состояния подключения
    WIFI_CONNECTING,
    WIFI_CONNECTING_ERROR,
    WIFI_CONNECTED
}   tConnState;

struct packets
{
uint32_t time 
};

struct packets message_send;

static tConnState connState = WIFI_CONNECTING;

struct espconn pConn;
esp_udp ConnUDP;

static void ICACHE_FLASH_ATTR 
wifi_check_ip(void *arg);


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

static void ICACHE_FLASH_ATTR 
udp_client_udp_send_cb (void* arg){}

static void ICACHE_FLASH_ATTR 
udp_client_udp_recv_cb (void* arg, char* pusrdata, uint16_t length){
    uint16_t idx;
    //os_printf("UDP : DATA RECEIVED : LEN = %d\n", length);
   // os_printf("UDP : DATA: ");

      os_printf("dataTIME %d \r\n", pusrdata);
      for(idx = 0; idx<length; idx++)
  {
    //uart_tx_one_char(UART0, (char)pusrdata[idx]);
  }

   espconn_send(&pConn, (uint8_t*)"Hello from ESP8266!!!\r\n", 23);
}

static void ICACHE_FLASH_ATTR 
udp_connect() {
    uint32_t ip;
    char data[] = "CIPSNTPTIME?";
    char udpserverip[15] = {};

    os_timer_disarm(&ptimer);
    pConn.proto.udp = &ConnUDP;
    pConn.type = ESPCONN_UDP;
    pConn.state = ESPCONN_NONE;

    os_sprintf(udpserverip, "%s", UDPSERVERIP);
    ip = ipaddr_addr(udpserverip);                  //преобразует из строки в инт   

    os_memcpy(pConn.proto.udp->remote_ip, &ip, 4);
    pConn.proto.udp->local_port = espconn_port();   //номер порта клиента
    os_printf("espconn_port: %d \r\n", pConn.proto.udp->local_port);
    pConn.proto.udp->remote_port = UDPSERVERPORT;   //номер порта сервера
    
    espconn_create(&pConn);
    os_printf("UDP espconn create...\r\n");

    espconn_regist_sentcb(&pConn, udp_client_udp_send_cb);  //указывает на функцию которая должна быть вызвана
                                                            //когда данные были переданы на удаленный хост
    espconn_regist_recvcb(&pConn, udp_client_udp_recv_cb);
                                                        

   // os_timer_setfn(&ptimer, (os_timer_func_t *)wifi_check_ip, NULL);
   // os_timer_arm(&ptimer, 1000, 0);

    espconn_send(&pConn, (uint8_t*)&message_send, 1);

}



static void ICACHE_FLASH_ATTR 
wifi_check_ip(void *arg){
    uint8_t wifiStatus = wifi_station_get_connect_status();
   // os_printf(" wifiStatus %d\n", wifiStatus);
    struct ip_info ipConfig;
    //os_printf("IP GOT  \n", IP2STR (&ipConfig.ip.addr));   
    if (wifiStatus == STATION_GOT_IP) 
    {
        wifi_get_ip_info(STATION_IF, &ipConfig);
        if(ipConfig.ip.addr !=0){
            
            if(connState != WIFI_CONNECTED){
               connState = WIFI_CONNECTED;
               os_printf("-------------------------------WIFI connected \r\n");
               udp_connect();
            return;
            }

            else {//os_printf("connState = WIFI_CONNECTED");
            }
        }
    }
    else
    {
    connState = WIFI_CONNECTING;
    espconn_delete(&pConn);
    if(wifi_station_get_connect_status() == STATION_WRONG_PASSWORD)
    {
      connState = WIFI_CONNECTING_ERROR;
      os_printf("STATION_WRONG_PASSWORD\r\n");
    }
    else if(wifi_station_get_connect_status() == STATION_NO_AP_FOUND)
    {
      connState = WIFI_CONNECTING_ERROR;
      os_printf("STATION_NO_AP_FOUND\r\n");
    }
    else if(wifi_station_get_connect_status() == STATION_CONNECT_FAIL)
    {
      connState = WIFI_CONNECTING_ERROR;
      os_printf("STATION_CONNECT_FAIL\r\n");
    }
    else
    {
      connState = WIFI_CONNECTING;
      os_printf("WiFi connecting...\r\n");
    }
  }
  os_timer_setfn(&ptimer, (os_timer_func_t *)wifi_check_ip, NULL);
  os_timer_arm(&ptimer, 2000, 0);
}

void ICACHE_FLASH_ATTR 
user_init(void)
{
    uart_init(BIT_RATE_115200, BIT_RATE_115200);
    gpio_init();
    //PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO2_U, FUNC_GPIO2);
    
    
    struct station_config stationConf;
    wifi_set_opmode_current(STATION_MODE);
    wifi_station_disconnect();
    wifi_station_dhcpc_stop();
  
    message_send.time = 0; 

  
  
  
   /*
    struct ip_info s_IP;
    uint32_t staticIP = 0;
    char static_IP[15] = {};
    os_sprintf(static_IP, "%s", STATICIP);
    staticIP = ipaddr_addr(static_IP);
    s_IP.ip.addr = staticIP; 
    //wifi_set_ip_info(STATION_IF, &s_IP);
*/
    if (wifi_station_get_config(&stationConf)){
        os_memset(stationConf.ssid, 0, sizeof(stationConf.ssid));
        os_memset(stationConf.password, 0, sizeof(stationConf.password));
        sprintf(stationConf.ssid, "%s", WIFI_CLIENTSSID);
        sprintf(stationConf.password, "%s", WIFI_CLIENTPASSWORD);
    }
    
    if(wifi_station_set_config_current(&stationConf) == TRUE){
        os_printf("\r\n");
        os_printf("WiFi configure done!\r\n");
    }

    if(wifi_station_get_config(&stationConf)!= TRUE){
        os_printf("Not station config set!\r\n");
    }
    
    wifi_set_sleep_type(NONE_SLEEP_T);
    wifi_station_connect();
    wifi_station_dhcpc_start();
    //wifi_set_ip_info(STATION_IF, &s_IP);

    if(wifi_get_phy_mode() != PHY_MODE_11N){
        wifi_set_phy_mode(PHY_MODE_11N);
    }

    if(wifi_station_get_auto_connect() == FALSE){
        wifi_station_set_auto_connect(TRUE);
    }

    wifi_status_led_install(LED, WIFI_LED_IO_MUX, FUNC_GPIO2);

    
    os_timer_disarm(&ptimer);      
    os_timer_setfn(&ptimer, (os_timer_func_t*)wifi_check_ip, NULL); //Set timer callback function.
    os_timer_arm(&ptimer, 1000, 0);
    
    
}
