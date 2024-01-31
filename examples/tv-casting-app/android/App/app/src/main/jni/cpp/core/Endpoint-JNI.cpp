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

#include "Endpoint-JNI.h"

#include "../JNIDACProvider.h"
#include "../support/CastingPlayerConverter-JNI.h"
#include "../support/ErrorConverter-JNI.h"
#include "../support/RotatingDeviceIdUniqueIdProvider-JNI.h"
#include "core/CastingApp.h"             // from tv-casting-common
#include "core/CastingPlayer.h"          // from tv-casting-common
#include "core/CastingPlayerDiscovery.h" // from tv-casting-common

#include <app/clusters/bindings/BindingManager.h>
#include <app/server/Server.h>
#include <jni.h>
#include <lib/support/JniReferences.h>
#include <lib/support/JniTypeWrappers.h>

using namespace chip;

#define JNI_METHOD(RETURN, METHOD_NAME)                                                                                            \
    extern "C" JNIEXPORT RETURN JNICALL Java_com_matter_casting_core_MatterEndpoint_##METHOD_NAME

namespace matter {
namespace casting {
namespace core {

JNI_METHOD(jint, getId)
(JNIEnv * env, jobject thiz)
{
    chip::DeviceLayer::StackLock lock;
    ChipLogProgress(AppServer, "Endpoint-JNI::getId() called");
    Endpoint * endpoint = EndpointJNIMgr().GetEndpoint(thiz);
    VerifyOrReturnValue(endpoint != nullptr, 0, ChipLogError(AppServer, "Endpoint-JNI::getId() endpoint == nullptr"));
    return endpoint->GetId();
}

JNI_METHOD(jint, getProductId)
(JNIEnv * env, jobject thiz)
{
    chip::DeviceLayer::StackLock lock;
    ChipLogProgress(AppServer, "Endpoint-JNI::getProductId() called");
    Endpoint * endpoint = EndpointJNIMgr().GetEndpoint(thiz);
    VerifyOrReturnValue(endpoint != nullptr, 0, ChipLogError(AppServer, "Endpoint-JNI::getProductId() endpoint == nullptr"));
    return endpoint->GetProductId();
}

JNI_METHOD(jint, getVendorId)
(JNIEnv * env, jobject thiz)
{
    chip::DeviceLayer::StackLock lock;
    ChipLogProgress(AppServer, "Endpoint-JNI::getVendorId() called");
    Endpoint * endpoint = EndpointJNIMgr().GetEndpoint(thiz);
    VerifyOrReturnValue(endpoint != nullptr, 0, ChipLogError(AppServer, "Endpoint-JNI::getVendorId() endpoint == nullptr"));
    return endpoint->GetVendorId();
}

JNI_METHOD(jobject, getCastingPlayer)
(JNIEnv * env, jobject thiz)
{
    chip::DeviceLayer::StackLock lock;
    ChipLogProgress(AppServer, "Endpoint-JNI::getCastingPlayer() called");
    Endpoint * endpoint = EndpointJNIMgr().GetEndpoint(thiz);
    VerifyOrReturnValue(endpoint != nullptr, 0, ChipLogError(AppServer, "Endpoint-JNI::getCastingPlayer() endpoint == nullptr"));
    return support::createJCastingPlayer(std::shared_ptr<CastingPlayer>(endpoint->GetCastingPlayer()));
}

/**
 * @brief Get the matter::casting::core::Endpoint object from the jobject jEndpointObject
 */
Endpoint * EndpointJNI::GetEndpoint(jobject jEndpointObject)
{
    JNIEnv * env = chip::JniReferences::GetInstance().GetEnvForCurrentThread();
    jclass endpointClass         = env->GetObjectClass(jEndpointObject);
    jfieldID _cppEndpointFieldId = env->GetFieldID(endpointClass, "_cppEndpoint", "J");
    VerifyOrReturnValue(_cppEndpointFieldId != nullptr, nullptr, ChipLogError(AppServer, "Endpoint-JNI::GetEndpoint() _cppEndpointFieldId == nullptr"));
    jlong _cppEndpointValue  = env->GetLongField(jEndpointObject, _cppEndpointFieldId);
    return reinterpret_cast<Endpoint *>(_cppEndpointValue);
}

}; // namespace core
}; // namespace casting
}; // namespace matter
