/*
 *
 *    Copyright (c) 2020-24 Project CHIP Authors
 *    All rights reserved.
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

#include <jni.h>

namespace matter {
namespace casting {
namespace core {

/*class CommandContext
{
public:
    jobject completableFuture;
    ConnectCallback callback;
    unsigned long long int commissioningWindowTimeoutSec;
    EndpointFilter desiredEndpointFilter;
};*/

class CommandJNI
{
public:
    void * GetCommand(jobject jCommandObject);

private:
    friend CommandJNI & CommandJNIMgr();
    static CommandJNI sInstance;
};

inline class CommandJNI & CommandJNIMgr()
{
    return CommandJNI::sInstance;
}
}; // namespace core
}; // namespace casting
}; // namespace matter
