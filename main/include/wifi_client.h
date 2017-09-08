/*
 * wifi_client.h
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
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 */

#ifndef MAIN_INCLUDE_WIFI_CLIENT_H_
#define MAIN_INCLUDE_WIFI_CLIENT_H_

extern EventGroupHandle_t wifi_event_group;
extern const int CONNECTED_BIT;

void initialise_wifi();
void request_task(void *pvParameters);


#endif /* MAIN_INCLUDE_WIFI_CLIENT_H_ */
