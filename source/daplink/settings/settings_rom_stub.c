/* CMSIS-DAP Interface Firmware
 * Copyright (c) 2015-2015 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdbool.h>
#include "settings.h"

void config_rom_init()
{
    // Do nothing
}

void config_set_auto_rst(bool on)
{
    // Do nothing
}

void config_set_automation_allowed(bool on)
{
    // Do nothing
}

bool config_get_auto_rst()
{
    return false;
}

bool config_get_automation_allowed()
{
    return true;
}