/*
 * wifi_server.c
 *
 *  Created on: 15-Sep-2017
 *  Author: ankitdaf
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
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_partition.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "driver/gpio.h"
#include "module_settings.h"
#include "esp_log.h"

#define STORAGE_NAMESPACE "settings"

#define TAG "MODULE_SETTINGS"

extern module_saved_param module_param;

/*
 *
 * Save the module parameters
 * We save the custom url endpoint and port number here
 *
 */
void save_parameters(void){

	nvs_handle module_settings_handle;
    // Open
    ESP_ERROR_CHECK(nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &module_settings_handle));
    ESP_ERROR_CHECK(nvs_set_str(module_settings_handle,URL_PARAM,module_param.url_endpoint));
    ESP_ERROR_CHECK(nvs_set_str(module_settings_handle,PORT_PARAM,module_param.port_number));
    ESP_ERROR_CHECK(nvs_commit(module_settings_handle));
    nvs_close(module_settings_handle);
}

void load_parameters(void) {
	nvs_handle module_settings_handle;
	esp_err_t err;

	// Open
	ESP_ERROR_CHECK(nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &module_settings_handle));

    // Read
    size_t required_size = 0;  // value will default to 0, if not set yet in NVS
    err = nvs_get_str(module_settings_handle,URL_PARAM,NULL,&required_size);
    if(err == ESP_ERR_NVS_NOT_FOUND) {
    	ESP_ERROR_CHECK(nvs_set_str(module_settings_handle,URL_PARAM,"http://httpbin.org/get?data="));
    	ESP_ERROR_CHECK(nvs_set_str(module_settings_handle,PORT_PARAM,"80"));
    }
    ESP_LOGI(TAG, "Size of value is %u",required_size);
    if(required_size > 0) {
    	ESP_ERROR_CHECK(nvs_get_str(module_settings_handle,URL_PARAM,module_param.url_endpoint,&required_size));
        ESP_LOGI(TAG, "Value is %s",module_param.url_endpoint);
    }

    ESP_ERROR_CHECK(nvs_get_str(module_settings_handle,PORT_PARAM,NULL,&required_size));
    ESP_LOGI(TAG, "Size of value is %u",required_size);
    if(required_size > 0) {
    	ESP_ERROR_CHECK(nvs_get_str(module_settings_handle,PORT_PARAM,module_param.port_number,&required_size));
    	ESP_LOGI(TAG, "Value is %s",module_param.port_number);
    }
    nvs_close(module_settings_handle);
}


void initialize_memory()
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES) {
        // NVS partition was truncated and needs to be erased
        const esp_partition_t* nvs_partition = esp_partition_find_first(
                ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NVS, NULL);
        assert(nvs_partition && "partition table must have an NVS partition");
        ESP_ERROR_CHECK( esp_partition_erase_range(nvs_partition, 0, nvs_partition->size) );
        // Retry nvs_flash_init
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK( err );
}
