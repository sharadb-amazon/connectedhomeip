/*
 *
 *    Copyright (c) 2023-2024 Project CHIP Authors
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */
#pragma once

#include "core/Command.h"

#include <lib/core/CHIPError.h>

#include <jni.h>

namespace matter {
namespace casting {
namespace support {

/**
 * @brief Converts a native Command into a MatterCommand jobject
 *
 * @return pointer to the Command jobject if created successfully, nullptr otherwise.
 */
jobject createJCommand(void * command, const char * className);

}; // namespace support
}; // namespace casting
}; // namespace matter
