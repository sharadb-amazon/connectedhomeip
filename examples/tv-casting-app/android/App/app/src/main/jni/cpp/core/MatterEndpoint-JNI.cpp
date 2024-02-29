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

#include "MatterEndpoint-JNI.h"

#include "../JNIDACProvider.h"
#include "../support/MatterCallback-JNI.h"
#include "../support/Converters-JNI.h"
#include "../support/RotatingDeviceIdUniqueIdProvider-JNI.h"
#include "clusters/Clusters.h"           // from tv-casting-common
#include "core/CastingApp.h"             // from tv-casting-common
#include "core/CastingPlayer.h"          // from tv-casting-common
#include "core/CastingPlayerDiscovery.h" // from tv-casting-common
#include "core/Endpoint.h"               // from tv-casting-common

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

MatterEndpointJNI MatterEndpointJNI::sInstance;

JNI_METHOD(jint, getId)
(JNIEnv * env, jobject thiz)
{
    chip::DeviceLayer::StackLock lock;
    ChipLogProgress(AppServer, "MatterEndpoint-JNI::getId() called");
    Endpoint * endpoint = support::convertEndpointFromJavaToCpp(thiz);
    VerifyOrReturnValue(endpoint != nullptr, 0, ChipLogError(AppServer, "MatterEndpoint-JNI::getId() endpoint == nullptr"));
    return endpoint->GetId();
}

JNI_METHOD(jint, getProductId)
(JNIEnv * env, jobject thiz)
{
    chip::DeviceLayer::StackLock lock;
    ChipLogProgress(AppServer, "MatterEndpoint-JNI::getProductId() called");
    Endpoint * endpoint = support::convertEndpointFromJavaToCpp(thiz);
    VerifyOrReturnValue(endpoint != nullptr, 0, ChipLogError(AppServer, "MatterEndpoint-JNI::getProductId() endpoint == nullptr"));
    return endpoint->GetProductId();
}

JNI_METHOD(jint, getVendorId)
(JNIEnv * env, jobject thiz)
{
    chip::DeviceLayer::StackLock lock;
    ChipLogProgress(AppServer, "MatterEndpoint-JNI::getVendorId() called");
    Endpoint * endpoint = support::convertEndpointFromJavaToCpp(thiz);
    VerifyOrReturnValue(endpoint != nullptr, 0, ChipLogError(AppServer, "MatterEndpoint-JNI::getVendorId() endpoint == nullptr"));
    return endpoint->GetVendorId();
}

JNI_METHOD(jobject, getCastingPlayer)
(JNIEnv * env, jobject thiz)
{
    chip::DeviceLayer::StackLock lock;
    ChipLogProgress(AppServer, "MatterEndpoint-JNI::getCastingPlayer() called");
    Endpoint * endpoint = support::convertEndpointFromJavaToCpp(thiz);
    VerifyOrReturnValue(endpoint != nullptr, 0,
                        ChipLogError(AppServer, "MatterEndpoint-JNI::getCastingPlayer() endpoint == nullptr"));
    return support::convertCastingPlayerFromCppToJava(std::shared_ptr<CastingPlayer>(endpoint->GetCastingPlayer()));
}

static void FindOrEstablishConnectionTask(intptr_t context)
{
    ChipLogProgress(AppServer, "FindOrEstablishConnectionTask called");
    if (context != 0)
    {
        SessionContextJNI * __context = reinterpret_cast<SessionContextJNI *>(context);   
        __context->endpoint->GetCastingPlayer()->FindOrEstablishSession(__context,
                    [](void * context, chip::Messaging::ExchangeManager & exchangeMgr, const chip::SessionHandle & sessionHandle) {
                        ChipLogProgress(AppServer, "MatterEndpointJNI::FindOrEstablishSessionTask() success");
                        //chip::DeviceLayer::StackLock lock;
                        JNIEnv * env = chip::JniReferences::GetInstance().GetEnvForCurrentThread();
                        OperationalDeviceProxy * device = new OperationalDeviceProxy(&exchangeMgr, sessionHandle);
                        SessionContextJNI * _context = static_cast<SessionContextJNI *>(context);                

                        DeviceProxyMatterSuccessHandlerJNI successHandler;
                        successHandler.SetUp(env, _context->successHandler);
                        successHandler.Handle(reinterpret_cast<jlong>(device));
                    },
                    [](void * context, const chip::ScopedNodeId & peerId, CHIP_ERROR error) {
                        ChipLogError(AppServer, "MatterEndpointJNI::FindOrEstablishSessionTask() failure");
                        JNIEnv * env = chip::JniReferences::GetInstance().GetEnvForCurrentThread();
                        jstring errorMessage = env->NewStringUTF(error.Format());
                        VerifyOrReturn(errorMessage != nullptr,
                               ChipLogError(AppServer, "MatterEndpointJNI::FindOrEstablishSessionTask() Could not create errorMessage"));
                        jobject throwableObject = env->NewObject(MatterEndpointJNIMgr().mThrowableClass, MatterEndpointJNIMgr().mThrowableConstructor, errorMessage);
                        VerifyOrReturn(throwableObject != nullptr,
                               ChipLogError(AppServer, "MatterEndpointJNI::FindOrEstablishSessionTask() Could not create throwableObject"));

                        SessionContextJNI * __context = static_cast<SessionContextJNI *>(context);
                    });
                    
    }
}

JNI_METHOD(void, getDeviceProxy)
(JNIEnv * env, jobject thiz, jobject jSuccessHandler, jobject jFailureHandler)
{
    chip::DeviceLayer::StackLock lock;
    ChipLogProgress(AppServer, "MatterEndpoint-JNI::getDeviceProxy() called");
    Endpoint * endpoint = support::convertEndpointFromJavaToCpp(thiz);
    VerifyOrReturn(endpoint != nullptr, ChipLogError(AppServer, "MatterEndpoint-JNI::getDeviceProxy() endpoint == nullptr"));

    SessionContextJNI * context         = new SessionContextJNI();
    context->endpoint = endpoint;
    context->successHandler = env->NewGlobalRef(jSuccessHandler);
    context->failureHandler = env->NewGlobalRef(jFailureHandler);

    DeviceProxyMatterSuccessHandlerJNI successHandler;
    successHandler.SetUp(env, jSuccessHandler);

    /*chip::Controller::GetConnectedDeviceCallback * connectedDeviceCallback = reinterpret_cast<chip::Controller::GetConnectedDeviceCallback *>(callbackHandle);

    chip::NodeId nodeId = endpoint->GetCastingPlayer()->GetNodeId();

    chip::Server::GetInstance().GetCASESessionManager()->FindOrEstablishSession(
        chip::ScopedNodeId(endpoint->GetCastingPlayer()->GetNodeId(), endpoint->GetCastingPlayer()->GetFabricIndex()), &connectedDeviceCallback->mOnSuccess, &connectedDeviceCallback->mOnFailure);*/

    //endpoint->GetCastingPlayer()->FindOrEstablishSession(&nodeId, &connectedDeviceCallback->mOnSuccess, &connectedDeviceCallback->mOnFailure);
    chip::DeviceLayer::PlatformMgr().ScheduleWork(FindOrEstablishConnectionTask, reinterpret_cast<intptr_t>(context));

    

    /*MatterEndpointJNIMgr().Init();
    // Setup completableFuture
    jobject completableFuture = env->NewGlobalRef(env->NewObject(MatterEndpointJNIMgr().mCompletableFutureClass, MatterEndpointJNIMgr().mCompletableFutureConstructor));
        VerifyOrReturnValue(
            completableFuture != nullptr, nullptr,
            ChipLogError(AppServer, "MatterEndpoint-JNI::getDeviceProxy() Could not instantiate completableFuture"));

    SessionContextJNI * context         = new SessionContextJNI();
    context->endpoint = endpoint;
    context->completableFuture = env->NewGlobalRef(completableFuture);
    
    chip::DeviceLayer::SystemLayer().StartTimer(chip::System::Clock::Milliseconds32(1000), MatterEndpointJNI::FindOrEstablishSessionTask, context);

    return completableFuture;*/
}

/*void MatterEndpointJNI::FindOrEstablishSessionTask(chip::System::Layer * aSystemLayer, void * context)
{
    ChipLogProgress(AppServer, "MatterEndpointJNI::FindOrEstablishSessionTask() called");
    if (context != nullptr)
    {
        SessionContextJNI * _context = static_cast<SessionContextJNI *>(context);
        _context->
    }
}*/

JNI_METHOD(jobject, getCluster)
(JNIEnv * env, jobject thiz, jclass clusterClass)
{
    chip::DeviceLayer::StackLock lock;
    ChipLogProgress(AppServer, "MatterEndpoint-JNI::getCluster() called");
    Endpoint * endpoint = support::convertEndpointFromJavaToCpp(thiz);
    VerifyOrReturnValue(endpoint != nullptr, 0, ChipLogError(AppServer, "MatterEndpoint-JNI::getCluster() endpoint == nullptr"));

    jclass clsClass        = env->GetObjectClass(clusterClass); // Class<Class>
    jmethodID mid          = env->GetMethodID(clsClass, "getName", "()Ljava/lang/String;");
    jstring jClassName     = (jstring) env->CallObjectMethod(clusterClass, mid);
    const char * className = env->GetStringUTFChars(jClassName, nullptr);
    ChipLogProgress(AppServer, "MatterEndpoint-JNI::getCluster() className: %s", className);
    if (strcmp(className, "com.matter.casting.clusters.MatterClusters$ContentLauncherCluster") == 0)
    {
        matter::casting::memory::Strong<matter::casting::clusters::content_launcher::ContentLauncherCluster>
            contentLauncherCluster = endpoint->GetCluster<matter::casting::clusters::content_launcher::ContentLauncherCluster>();

        jobject clusterJavaObject = support::convertClusterFromCppToJava(
            contentLauncherCluster, "com/matter/casting/clusters/MatterClusters$ContentLauncherCluster");
        env->ReleaseStringUTFChars(jClassName, className);
        return clusterJavaObject;
    }

    env->ReleaseStringUTFChars(jClassName, className);
    return nullptr;
}

}; // namespace core
}; // namespace casting
}; // namespace matter
