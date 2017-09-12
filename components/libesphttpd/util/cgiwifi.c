/*
 * wifi_server.c
 *
 *  Created on: 06-May-2017
 *      Author: ankitdaf
 *
 *  Based on Jeroen's libesphttpd code but separated out for
 *  this project
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
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * Jeroen Domburg <jeroen@spritesmods.com> wrote this file. As long as you retain
 * this notice you can do whatever you want with this stuff. If we meet some day,
 * and you think this stuff is worth it, you can buy me a beer in return.
 * ----------------------------------------------------------------------------
 *
 */

/*
Cgi/template routines for the /wifi url.
*/

#include <string.h>
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_err.h"
#include "esp_log.h"
#include "cgiwifi.h"

#define TAG "CGIWIFI"

static int number_of_aps_found = 0;
static uint8_t is_scan_in_progress = 0;
wifi_ap_record_t *scan_result;

//This CGI is called from the bit of AJAX-code in wifi.tpl. It will initiate a
//scan for access points and if available will return the result of an earlier scan.
//The result is embedded in a bit of JSON parsed by the javascript in wifi.tpl.
int  cgiWiFiScan(HttpdConnData *connData) {
	int len;
	char buff[1024];
	if (is_scan_in_progress) {
		httpdStartResponse(connData, 200);
		httpdHeader(connData, "Content-Type", "text/json");
		httpdEndHeaders(connData);
		//We're still scanning. Tell Javascript code that.
		ESP_LOGI(TAG,"Scan in progress");
		len=sprintf(buff, "{\n \"result\": { \n\"inProgress\": \"1\"\n }\n}\n");
		ESP_LOGI(TAG,"%s",buff);
		httpdSend(connData, buff, len);
		return HTTPD_CGI_DONE;
	} else if(number_of_aps_found > 0) {
		httpdStartResponse(connData, 200);
		httpdHeader(connData, "Content-Type", "text/json");
		httpdEndHeaders(connData);

		//We have a scan result. Pass it on.
		ESP_LOGI(TAG,"We have a result");
		len=sprintf(buff, "{\n \"result\": { \n\"inProgress\": \"0\", \n\"APs\": [\n");
		ESP_LOGI(TAG,"%s",buff);
		httpdSend(connData, buff, len);
		for(int i=0;i<number_of_aps_found;++i) {
			len=sprintf(buff, "{\"essid\": \"%s\", \"bssid\": \"" MACSTR "\", \"rssi\": \"%d\", \"enc\": \"%d\", \"channel\": \"%d\"}%s",
									scan_result[i].ssid, MAC2STR(scan_result[i].bssid), scan_result[i].rssi,
									scan_result[i].authmode, scan_result[i].primary, (i==(number_of_aps_found-1))?"":",");
			ESP_LOGI(TAG,"%s",buff);
			httpdSend(connData, buff, len);
		}
		len=sprintf(buff, "]\n}\n}\n");
		ESP_LOGI(TAG,"%s",buff);
		httpdSend(connData, buff, len);
		number_of_aps_found = 0;
		wifi_mode_t mode;
		ESP_ERROR_CHECK(esp_wifi_get_mode(&mode));
		if(mode != WIFI_MODE_AP) {
			//Also start a new scan.
			ESP_LOGI(TAG,"About to start a scan\n");
			wifi_scan_config_t config = {
					.scan_type = WIFI_SCAN_TYPE_ACTIVE,
			};
			is_scan_in_progress = 1;
			ESP_ERROR_CHECK(esp_wifi_scan_start(&config, 0));
		}
		// if (cgiWifiAps.apData==NULL) number_of_aps_found=0;
		return HTTPD_CGI_DONE;
	}
	else {
		ESP_LOGI(TAG,"Scan not started yet, let's start one");
		wifi_mode_t mode;
				ESP_ERROR_CHECK(esp_wifi_get_mode(&mode));
				if(mode != WIFI_MODE_AP) {
					//Also start a new scan.
					ESP_LOGI(TAG,"About to start a scan\n");
					wifi_scan_config_t config = {
							.scan_type = WIFI_SCAN_TYPE_ACTIVE,
					};
					is_scan_in_progress = 1;
					ESP_ERROR_CHECK(esp_wifi_scan_start(&config, 0));
				}
		return HTTPD_CGI_DONE;
	}
}


//This cgi uses the routines above to connect to a specific access point with the
//given ESSID using the given password.
int  cgiWiFiConnect(HttpdConnData *connData) {
	char essid[128];
	char passwd[128];

	if (connData->conn==NULL) {
		//Connection aborted. Clean up.
		return HTTPD_CGI_DONE;
	}
	httpdFindArg(connData->post->buff, "essid", essid, sizeof(essid));
	httpdFindArg(connData->post->buff, "passwd", passwd, sizeof(passwd));
	wifi_config_t wifi_config;
	uint8_t essid_len = strlen(essid);
	uint8_t password_len = strlen(passwd);
	// Attempt to change connection parameters only if the SSID is not null
	if(essid_len > 0 ) {
	    memcpy(wifi_config.sta.ssid,essid, essid_len);
	    memcpy(wifi_config.sta.password,passwd,password_len);
	    wifi_config.sta.bssid_set = false;
	    wifi_config.sta.ssid[essid_len] = '\0';
	    wifi_config.sta.password[password_len] = '\0';
	    ESP_LOGI(TAG, "SSID is : %s",essid);
	    ESP_LOGI(TAG, "Password is : %s",passwd);
	    ESP_ERROR_CHECK( esp_wifi_stop() );
	    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
	    ESP_ERROR_CHECK( esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
	    ESP_ERROR_CHECK( esp_wifi_start());
	}
	return HTTPD_CGI_DONE;
}

//This cgi uses the routines above to connect to a specific access point with the
//given ESSID using the given password.
int  cgiWiFiSetMode(HttpdConnData *connData) {
	int len;
		char buff[1024];

		if (connData->conn==NULL) {
			//Connection aborted. Clean up.
			return HTTPD_CGI_DONE;
		}

		len=httpdFindArg(connData->getArgs, "mode", buff, sizeof(buff));
		if (len!=0) {
			httpd_printf("cgiWifiSetMode: %s\n", buff);
			ESP_ERROR_CHECK(esp_wifi_set_mode((wifi_mode_t)(atoi(buff))));
			// If it is station mode, don't connect to saved AP, wait for a new configuration
			if((wifi_mode_t)atoi(buff) == WIFI_MODE_STA || (wifi_mode_t)atoi(buff) == WIFI_MODE_APSTA ) {
			    wifi_config_t wifi_config;
			    *wifi_config.sta.ssid = 0;
			    *wifi_config.sta.password = 0;
			    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
			}
		}
		httpdRedirect(connData, "/wifi");
	return HTTPD_CGI_DONE;
}

int  cgiWiFiConnStatus(HttpdConnData *connData) {
	return HTTPD_CGI_DONE;
}

void  wifiScanDoneCb(void) {
	is_scan_in_progress = 0;
	ESP_LOGI(TAG,"wifiScanDoneCb\n");
	// Count number of APs found
	ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&number_of_aps_found));
	ESP_LOGI(TAG,"Scan done: found %d APs\n", number_of_aps_found);
	wifi_ap_record_t result[number_of_aps_found];
	ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&number_of_aps_found,result));
	scan_result = result;
	for (uint16_t i=0;i<number_of_aps_found;++i) {
		ESP_LOGI(TAG,"%02x:%02x:%02x:%02x:%02x:%02x %s RSSI %d",scan_result[i].bssid[0],scan_result[i].bssid[1],scan_result[i].bssid[2],scan_result[i].bssid[3],scan_result[i].bssid[4],scan_result[i].bssid[5], scan_result[i].ssid,scan_result[i].rssi);
	}
}
//Template code for the WLAN page.
int tplWlan(HttpdConnData *connData, char *token, void **arg) {
	char buff[1024];
	wifi_mode_t x;
	wifi_config_t stconf;
	if (token==NULL) return 1;
	ESP_ERROR_CHECK(esp_wifi_get_config(ESP_IF_WIFI_STA,&stconf));

	strcpy(buff, "Unknown");
	if (strcmp(token, "WiFiMode")==0) {
		ESP_ERROR_CHECK(esp_wifi_get_mode(&x));
		if (x==WIFI_MODE_STA) strcpy(buff, "Client");
		if (x==WIFI_MODE_AP) strcpy(buff, "SoftAP");
		if (x==WIFI_MODE_APSTA) strcpy(buff, "STA+AP");
	} else if (strcmp(token, "currSsid")==0) {
		strcpy(buff, (char*)stconf.sta.ssid);
	} else if (strcmp(token, "WiFiEssid")==0) {
		strcpy(buff, (char*)stconf.sta.ssid);
	} else if (strcmp(token, "WiFiPasswd")==0) {
		strcpy(buff, (char*)stconf.sta.password);
	} else if (strcmp(token, "WiFiapwarn")==0) {
		ESP_ERROR_CHECK(esp_wifi_get_mode(&x));
		if (x==WIFI_MODE_AP) {
			strcpy(buff, "<b>Can't scan in this mode.</b> Click <a href=\"setmode.cgi?mode=3\">here</a> to go to STA+AP mode.");
		} else if( x== WIFI_MODE_STA) {
			strcpy(buff, "Click <a href=\"setmode.cgi?mode=3\">here</a> to go to AP + STA mode.");
		} else {
			strcpy(buff, "Click <a href=\"setmode.cgi?mode=2\">here</a> to go to AP mode.");
		}
	}
	httpdSend(connData, buff, -1);
	return 0;
}
