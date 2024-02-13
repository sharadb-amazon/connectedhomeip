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

#include "Cluster-JNI.h"

#include "../JNIDACProvider.h"
#include "../support/CastingPlayerConverter-JNI.h"
#include "../support/CommandConverter-JNI.h"
#include "../support/EndpointConverter-JNI.h"
#include "../support/ErrorConverter-JNI.h"
#include "../support/RotatingDeviceIdUniqueIdProvider-JNI.h"
#include "core/CastingApp.h" // from tv-casting-common

#include <app/clusters/bindings/BindingManager.h>
#include <app/server/Server.h>
#include <jni.h>
#include <lib/support/JniReferences.h>
#include <lib/support/JniTypeWrappers.h>

using namespace chip;

#define JNI_METHOD(RETURN, METHOD_NAME) extern "C" JNIEXPORT RETURN JNICALL Java_com_matter_casting_core_MatterCluster_##METHOD_NAME

namespace matter {
namespace casting {
namespace core {

JNI_METHOD(jobject, getEndpoint)
(JNIEnv * env, jobject thiz)
{
    chip::DeviceLayer::StackLock lock;
    ChipLogProgress(AppServer, "Cluster-JNI::getEndpoint() called");
    BaseCluster * cluster = ClusterJNIMgr().GetCluster(thiz);
    VerifyOrReturnValue(cluster != nullptr, 0, ChipLogError(AppServer, "Cluster-JNI::getEndpoint() cluster == nullptr"));
    return support::createJEndpoint(std::shared_ptr<Endpoint>(cluster->GetEndpoint().lock()));
}

JNI_METHOD(jobject, getCommand)
(JNIEnv * env, jobject thiz, jclass commandClass)
{
    chip::DeviceLayer::StackLock lock;
    ChipLogProgress(AppServer, "Cluster-JNI::getCommand() called");

    jclass clsClass        = env->GetObjectClass(commandClass); // Class<Class>
    jmethodID mid          = env->GetMethodID(clsClass, "getName", "()Ljava/lang/String;");
    jstring jClassName     = (jstring) env->CallObjectMethod(commandClass, mid);
    const char * className = env->GetStringUTFChars(jClassName, nullptr);
    ChipLogProgress(AppServer, "Cluster-JNI::getCommand() className: %s", className);
    if (strcmp(className, "com.matter.casting.clusters.MatterCommands$ContentLauncherClusterLaunchURLCommand") == 0)
    {
        BaseCluster * cluster = ClusterJNIMgr().GetCluster(thiz);
        VerifyOrReturnValue(cluster != nullptr, 0, ChipLogError(AppServer, "Cluster-JNI::getCommand() cluster == nullptr"));
        void * command = cluster->GetCommand(chip::app::Clusters::ContentLauncher::Commands::LaunchURL::Id);

        jobject commandJavaObject =
            support::createJCommand(command, "com/matter/casting/clusters/MatterCommands$ContentLauncherClusterLaunchURLCommand");
        env->ReleaseStringUTFChars(jClassName, className);
        return commandJavaObject;
    }

    env->ReleaseStringUTFChars(jClassName, className);
    return nullptr;
}

/**
 * @brief Get the matter::casting::core::Cluster object from the jobject jClusterObject
 */
BaseCluster * ClusterJNI::GetCluster(jobject jClusterObject)
{
    JNIEnv * env                = chip::JniReferences::GetInstance().GetEnvForCurrentThread();
    jclass clusterClass         = env->GetObjectClass(jClusterObject);
    jfieldID _cppClusterFieldId = env->GetFieldID(clusterClass, "_cppCluster", "J");
    VerifyOrReturnValue(_cppClusterFieldId != nullptr, nullptr,
                        ChipLogError(AppServer, "Cluster-JNI::GetCluster() _cppCluster == nullptr"));
    jlong _cppClusterValue = env->GetLongField(jClusterObject, _cppClusterFieldId);
    return reinterpret_cast<BaseCluster *>(_cppClusterValue);
}

}; // namespace core
}; // namespace casting
}; // namespace matter
