/*
 *   Copyright (c) 2024 Project CHIP Authors
 *   All rights reserved.
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 *
 */

#include "Commands-JNI.h"

#include "../JNIDACProvider.h"
#include "../support/CastingPlayerConverter-JNI.h"
#include "../support/ErrorConverter-JNI.h"
#include "../support/EndpointConverter-JNI.h"
#include "../support/RotatingDeviceIdUniqueIdProvider-JNI.h"
#include "core/CastingApp.h"             // from tv-casting-common

#include <app/clusters/bindings/BindingManager.h>
#include <app/server/Server.h>
#include <jni.h>
#include <lib/support/JniReferences.h>
#include <lib/support/JniTypeWrappers.h>

using namespace chip;

#define JNI_METHOD(RETURN, METHOD_NAME)                                                                                            \
    extern "C" JNIEXPORT RETURN JNICALL Java_com_matter_casting_clusters_MatterCommands_ContentLauncherClusterLaunchURLCommand##METHOD_NAME

namespace matter {
namespace casting {
namespace core {

JNI_METHOD(jobject, invoke)
(JNIEnv * env, jobject thiz, jobject request, jobject context, jobject timedInvokeTimeoutMs)
{
    chip::DeviceLayer::StackLock lock;
    ChipLogProgress(AppServer, "Commands-JNI::invoke() called");
    void * command = CommandsJNI().GetCommand(thiz);
    VerifyOrReturnValue(command != nullptr, 0, ChipLogError(AppServer, "Commands-JNI::invoke() command == nullptr"));
    return nullptr;
}

/**
 * @brief Get the matter::casting::core::Command object from the jobject jCommandObject
 */
void * CommandsJNI::GetCommand(jobject jCommandObject)
{
    JNIEnv * env = chip::JniReferences::GetInstance().GetEnvForCurrentThread();
    jclass commandClass         = env->GetObjectClass(jCommandObject);
    jfieldID _cppCommandFieldId = env->GetFieldID(commandClass, "_cppCommand", "J");
    VerifyOrReturnValue(_cppCommandFieldId != nullptr, nullptr, ChipLogError(AppServer, "Commands-JNI::GetCommand() _cppCommand == nullptr"));
    jlong _cppCommandValue  = env->GetLongField(jCommandObject, _cppCommandFieldId);
    return reinterpret_cast<void *>(_cppCommandValue);
}

}; // namespace core
}; // namespace casting
}; // namespace matter
