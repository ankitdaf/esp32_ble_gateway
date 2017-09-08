/*
 * wifi_server.h
 *
 *  Created on: 06-May-2017
 *      Author: ankitdaf
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
 *
 */

#ifndef MAIN_WIFI_SERVER_H_
#define MAIN_WIFI_SERVER_H_

esp_err_t esp_loop_init_event_handler(void *ctx, system_event_t *event);
void server_init(void);

#endif /* MAIN_WIFI_SERVER_H_ */
