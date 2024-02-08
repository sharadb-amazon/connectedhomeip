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

#include "ClusterConverter-JNI.h"
#include <lib/support/JniReferences.h>

namespace matter {
namespace casting {
namespace support {

using namespace chip;

jobject createJCluster(matter::casting::memory::Strong<core::BaseCluster> cluster, const char * className)
{
    ChipLogProgress(AppServer, "ClusterConverter-JNI.createJCluster() called");
    JNIEnv * env = JniReferences::GetInstance().GetEnvForCurrentThread();

    // Get a reference to the cluster's Java class
    jclass clusterJavaClass = env->FindClass(className);
    if (clusterJavaClass == nullptr)
    {
        ChipLogError(AppServer,
                     "ClusterConverter-JNI.createJCluster() could not locate cluster's Java class");
        return nullptr;
    }

    // Get the constructor for the cluster's Java class
    jmethodID constructor =
        env->GetMethodID(clusterJavaClass, "<init>", "()V");
    if (constructor == nullptr)
    {
        ChipLogError(AppServer, "ClusterConverter-JNI.createJCluster() could not locate cluster's Java class constructor");
        return nullptr;
    }

    // Create a new instance of the cluster's Java class
    jobject jMatterCluster = nullptr;
    jMatterCluster = env->NewObject(clusterJavaClass, constructor);
    if (jMatterCluster == nullptr)
    {
        ChipLogError(AppServer, "ClusterConverter-JNI.createJCluster(): Could not create cluster's Java object");
        return jMatterCluster;
    }
    // Set the value of the _cppEndpoint field in the Java object to the C++ Endpoint pointer.
    jfieldID longFieldId = env->GetFieldID(clusterJavaClass, "_cppCluster", "J");
    env->SetLongField(jMatterCluster, longFieldId, reinterpret_cast<jlong>(cluster.get()));
    return jMatterCluster;
}

}; // namespace support
}; // namespace casting
}; // namespace matter
