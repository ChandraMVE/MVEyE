//-----------------------------------------------------------------
///
///     \file app_main.c
///
///     \brief main application framework driver
///
///
///     \author       Chandrashekhar Venkatesh
///
///     Location:     India
///
///     Project Name: MVEyE_1V0
///
///     \date Created 20AUG2024
///
///      Tools:  EspressifIDE
///      Device:   ESP32WROOM
///		 Operating System: windows 10
/// 	 Java Runtime Version: 17.0.11+9
///      Eclipse Version: 4.30.0.v20231201-0110
///      Eclipse CDT Version: 11.4.0.202309142347
///      IDF Eclipse Plugin Version: 3.0.0.202406051940
///      IDF Version:   5.3
///
/// Copyright © 2024 MicriVision Embedded Pvt Ltd
///
/// Confidential Property of MicroVision Embedded Pvt Ltd
///
//-----------------------------------------------------------------

//==============================================================================
//          __             __   ___  __
//  | |\ | /  ` |    |  | |  \ |__  /__`
//  | | \| \__, |___ \__/ |__/ |___ .__/
//
//==============================================================================

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "soc/gpio_periph.h"
#include "sdkconfig.h"
#include "esp_flash.h"

#include "lora_llc68.h"
#include "lora_app.h"
#include "leds.h"

#include "accelero_driver.h"
#include "accelero_app.h"

#include "MQTT_app.h"
#include "mqtt_client.h"

#include "MVEyE_1V0.h"
//==============================================================================
//   __   ___  ___         ___  __
//  |  \ |__  |__  | |\ | |__  /__`
//  |__/ |___ |    | | \| |___ .__/
//
//==============================================================================
#define TAG "MVEyE_1V0"

//==============================================================================
//   __        __   __                          __   __
//  / _` |    /  \ |__)  /\  |       \  /  /\  |__) /__`
//  \__> |___ \__/ |__) /~~\ |___     \/  /~~\ |  \ .__/
//
//==============================================================================
bool is_wifi_connected = false;

bool get_wifi_connected()
{
	return is_wifi_connected;
}

void set_wifi_connected(bool is_connected)
{
	is_wifi_connected = is_connected;
}
/*******************************************************************************
 * Function name  : get_esp32_version
 *
 * Description    : function to print the esp32 chip version parameters
 * Parameters     : None
 * Returns        : None
 *
 * Known Issues   :
 * Note           :
 * author         : Chandrashekhar Venkatesh
 * date           : 12AUG2024
 ******************************************************************************/
void get_esp32_version(void)
{
    printf("Welcome MVEyE 1V0 Development\n");
    /* Print chip information */
    esp_chip_info_t chip_info;
    uint32_t flash_size;
    esp_chip_info(&chip_info);
    printf("This is %s chip with %d CPU core(s), %s%s%s%s, ",
           CONFIG_IDF_TARGET,
           chip_info.cores,
           (chip_info.features & CHIP_FEATURE_WIFI_BGN) ? "WiFi/" : "",
           (chip_info.features & CHIP_FEATURE_BT) ? "BT" : "",
           (chip_info.features & CHIP_FEATURE_BLE) ? "BLE" : "",
           (chip_info.features & CHIP_FEATURE_IEEE802154) ? ", 802.15.4 (Zigbee/Thread)" : "");

    unsigned major_rev = chip_info.revision / 100;
    unsigned minor_rev = chip_info.revision % 100;
    printf("silicon revision v%d.%d, ", major_rev, minor_rev);
    if(esp_flash_get_size(NULL, &flash_size) != ESP_OK) {
        printf("Get flash size failed");
        return;
    }

    printf("%" PRIu32 "MB %s flash\n", flash_size / (uint32_t)(1024 * 1024),
           (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    printf("Minimum free heap size: %" PRIu32 " bytes\n", esp_get_minimum_free_heap_size());
}

/*******************************************************************************
 * Function name  : app_main
 *
 * Description    : function for main app
 * Parameters     : None
 * Returns        : None
 *
 * Known Issues   :
 * Note           :
 * author         : Chandrashekhar Venkatesh
 * date           : 12AUG2024
 ******************************************************************************/
 
void app_main(void)
{
	
	char console_input[100];
    int len = 0;
        	
	vTaskDelay(5000 / portTICK_PERIOD_MS); //Wait for proper debug messages to see.
	
	// Set up USB-UART CLI
    get_esp32_version();
    example_configure_stdin_stdout();
    
    // Initialize and set up LEDs	    
    init_leds();
    create_leds_task();
    
    // Initialize and handle accelerometer data via Interrupt routine and Task Queue
    create_Accelerometer_task();
    
    // Initialize and configure Lora and tasks to handle ping pong data    
    //LoRaAppInit();
    //create_lora_task();
    
    // Initialize MQTT, WIFI settings and events to handle publish and subscribe topics
    mqtt_init();
	create_mqtt_task();
    
    // Use main Task to handle CLI
	while(1)
	{
		// ignore empty or LF only string
	    do {
	        fgets(console_input, 100, stdin);
	        len = strlen(console_input);
	    } while (len<=1 && console_input[0] == '\n');
	    console_input[len - 1] = '\0';
	    
	    printf("Received string  = %s\r\n", console_input);
		vTaskDelay(1000 / portTICK_PERIOD_MS);		
	}
}