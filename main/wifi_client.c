/*
 * wifi_client.c
 *
 *  Created on: 07-May-2017
 *      Author: ankitdaf
 *
 *  Modified and Licensed under the Apache License, Version 2.0 (the "License");
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *      Unless required by applicable law or agreed to in writing, software
 *      distributed under the License is distributed on an "AS IS" BASIS,
 *      WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *      See the License for the specific language governing permissions and
 *      limitations under the License.
 *
 */

#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "esp_err.h"

// For the esp-request component part of the library
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"

#include "esp_request.h"

#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "string.h"

#include "platform.h"
#include "httpd.h"
#include "cgiwifi.h"
#include "wifi_client.h"
#include "module_settings.h"

#define TAG "WIFICLIENT"

/* FreeRTOS event group to signal when we are connected & ready to make a request */
EventGroupHandle_t wifi_event_group;

/* The event group allows multiple bits for each event,
   but we only care about one event - are we connected
   to the AP with an IP? */
const int CONNECTED_BIT = BIT0;

esp_err_t esp_client_loop_init_event_handler(void *ctx, system_event_t *event)
{
	 switch(event->event_id) {
	    case SYSTEM_EVENT_STA_START:
	    	ESP_LOGI(TAG, "Station started, Trying to connect now");
	    	wifi_config_t wfc;
	    	ESP_ERROR_CHECK(esp_wifi_get_config(ESP_IF_WIFI_STA,&wfc));
	    	// Don't connect if the configuration is null
	    	if(*wfc.sta.ssid != 0) {
	    	ESP_ERROR_CHECK(esp_wifi_connect());
	    	}
	        break;
	    case SYSTEM_EVENT_STA_STOP:
	    	ESP_LOGI(TAG, "Station stopped");
	    	break;
	    case SYSTEM_EVENT_STA_GOT_IP:
	    	ESP_LOGI(TAG, "Got IP");
	        xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
	        httpdDeInit();
	        break;
	    case SYSTEM_EVENT_STA_DISCONNECTED:
	        /* This is a workaround as ESP32 WiFi libs don't currently
	           auto-reassociate. */
	    	ESP_LOGI(TAG, "Station  disconnected");
	        ESP_ERROR_CHECK(esp_wifi_connect());
	        xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
	        break;
	    case SYSTEM_EVENT_STA_CONNECTED:
	    	ESP_LOGI(TAG, "Connected");
	    	break;
	    case SYSTEM_EVENT_SCAN_DONE:
	    	ESP_LOGI(TAG, "Scan is done");
	    	wifiScanDoneCb();
	    	break;
	    case SYSTEM_EVENT_AP_START:                 /**< ESP32 soft-AP start */
	    	ESP_LOGI(TAG, "AP started");
	    	break;
	    case SYSTEM_EVENT_AP_STOP:                  /**< ESP32 soft-AP stop */
	    	ESP_LOGI(TAG, "AP stopped");
	    	break;
	    case SYSTEM_EVENT_AP_STACONNECTED:          /**< a station connected to ESP32 soft-AP */
	    	ESP_LOGI(TAG, "Station connected to AP");
	    	break;
	    case SYSTEM_EVENT_AP_STADISCONNECTED:       /**< a station disconnected from ESP32 soft-AP */
	    	ESP_LOGI(TAG, "Station disconnected from AP");
	    	break;
	    default:
	    	ESP_LOGI(TAG,"Unable to connect, error code %d", event->event_id);
	        break;
	    }
    return ESP_OK;
}


void request_task(void *pvParameters)
{
	ESP_LOGI(TAG, "Starting request");
    request_t *req;
    char request_data[100];
    memcpy(request_data,(char *) pvParameters,strlen((char *) pvParameters));
    char url[256];
    memset(url,0,256);
    memcpy(url,module_param.url_endpoint,strlen(module_param.url_endpoint));
    strcat(url,(char *) pvParameters); // TODO: Make it compatible with custom port numbers
    int status;
    EventBits_t uxBits = xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT, false, true, 3000/portTICK_PERIOD_MS);
    if((uxBits & CONNECTED_BIT) == CONNECTED_BIT) {
        req = req_new(url);
        status = req_perform(req);
        ESP_LOGI(TAG, "Request status=%d", status);
        req_clean(req);
    }
    else {
    	// TODO: We are no longer connected, just reboot into AP mode for reconfiguration
    }
    ESP_LOGI(TAG, "heap size: %d\n", esp_get_free_heap_size());
    vTaskDelete(NULL);
}

void initialise_wifi(void)
{
    tcpip_adapter_init();
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK( esp_event_loop_init(esp_client_loop_init_event_handler, NULL) );
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_FLASH));
    wifi_config_t wfc;
    ESP_ERROR_CHECK( esp_wifi_get_config(ESP_IF_WIFI_STA,&wfc));
    if(strlen((char *)wfc.sta.ssid) == 0) {
    	ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_AP) );
    	    wifi_config_t wifi_config = {
    	    		.ap = {
    	    				.ssid = "ESP32_BLE_GATEWAY",
    						.authmode = WIFI_AUTH_OPEN,
    						.max_connection = 1,
    	    		},
    	    };
    	    ESP_ERROR_CHECK( esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config) );
    }
    else {
    	ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    	ESP_ERROR_CHECK( esp_wifi_set_auto_connect(true) );
    }
    ESP_ERROR_CHECK( esp_wifi_start());
    ESP_LOGI(TAG, "Initialised wifi");
}
