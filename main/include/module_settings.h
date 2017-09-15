/*
 * wifi_client.h
 *
 *  Created on: 11-Sep-2017
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

#ifndef MODULE_SETTINGS_H
#define MODULE_SETTINGS_H

#include "nvs.h"

void initialize_memory(void);
void load_parameters(void);
void save_parameters(void);

static const char *URL_PARAM = "url";
static const char *PORT_PARAM = "port";

typedef struct {
	char url_endpoint[256];
	char port_number[6];
} module_saved_param;

extern module_saved_param module_param;

#endif /* MODULE_SETTINGS_H */
 
