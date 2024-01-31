/*
 *   Copyright (c) 2020-24 Project CHIP Authors
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

#include "EndpointConverter-JNI.h"
#include <lib/support/JniReferences.h>

namespace matter {
namespace casting {
namespace support {

using namespace chip;

jobject createJEndpoint(matter::casting::memory::Strong<core::Endpoint> endpoint)
{
    ChipLogProgress(AppServer, "EndpointConverter-JNI.createJEndpoint() called");
    JNIEnv * env = JniReferences::GetInstance().GetEnvForCurrentThread();

    // Get a reference to the MatterEndpoint Java class
    jclass matterEndpointJavaClass = env->FindClass("com/matter/casting/core/MatterEndpoint");
    if (matterEndpointJavaClass == nullptr)
    {
        ChipLogError(AppServer,
                     "EndpointConverter-JNI.createJEndpoint() could not locate MatterEndpoint Java class");
        return nullptr;
    }

    // Get the constructor for the com/matter/casting/core/MatterEndpoint Java class
    jmethodID constructor =
        env->GetMethodID(matterEndpointJavaClass, "<init>", "()V");
    if (constructor == nullptr)
    {
        ChipLogError(AppServer, "EndpointConverter-JNI.createJEndpoint() could not locate MatterEndpoint Java class constructor");
        return nullptr;
    }

    // Create a new instance of the MatterEndpoint Java class
    jobject jMatterEndpoint = nullptr;
    jMatterEndpoint = env->NewObject(matterEndpointJavaClass, constructor);
    if (jMatterEndpoint == nullptr)
    {
        ChipLogError(AppServer, "EndpointConverter-JNI.createJEndpoint(): Could not create MatterEndpoint Java object");
        return jMatterEndpoint;
    }
    // Set the value of the _cppEndpoint field in the Java object to the C++ Endpoint pointer.
    jfieldID longFieldId = env->GetFieldID(matterEndpointJavaClass, "_cppEndpoint", "J");
    env->SetLongField(jMatterEndpoint, longFieldId, reinterpret_cast<jlong>(endpoint.get()));
    return jMatterEndpoint;
}

}; // namespace support
}; // namespace casting
}; // namespace matter
