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

#include "core/Endpoint.h" // from tv-casting-common

#include <jni.h>
#include <lib/support/JniReferences.h>
#include <lib/support/JniTypeWrappers.h>

namespace matter {
namespace casting {
namespace core {

struct SessionContextJNI
{
public:
    Endpoint *endpoint;
    jobject successHandler;
    jobject failureHandler;
};

class MatterEndpointJNI
{
public:
    void Init()
    {
        ChipLogProgress(AppServer, "MatterEndpointJNI() called");
        JNIEnv * env = chip::JniReferences::GetInstance().GetEnvForCurrentThread();

        mCompletableFutureClass       = env->FindClass("java/util/concurrent/CompletableFuture");
        mCompletableFutureConstructor = env->GetMethodID(mCompletableFutureClass, "<init>", "()V");

        mCompleteMethod = env->GetMethodID(mCompletableFutureClass, "complete", "(Ljava/lang/Object;)Z");
        VerifyOrReturn(mCompleteMethod != nullptr,
                       ChipLogError(AppServer, "<MatterEndpointJNI()> Could not get completeMethod"));

        mCompleteExceptionallyMethod =
            env->GetMethodID(mCompletableFutureClass, "completeExceptionally", "(Ljava/lang/Throwable;)Z");
        VerifyOrReturn(mCompleteExceptionallyMethod != nullptr,
                       ChipLogError(AppServer, "<MatterEndpointJNI()> Could not get completeExceptionallyMethod"));

        mThrowableClass = env->FindClass("java/lang/RuntimeException");
        VerifyOrReturn(mThrowableClass != nullptr,
                       ChipLogError(AppServer, "<MatterEndpointJNI()> Could not find throwableClass"));
        mThrowableConstructor = env->GetMethodID(mThrowableClass, "<init>", "(Ljava/lang/String;)V");
        VerifyOrReturn(mThrowableConstructor != nullptr,
                       ChipLogError(AppServer, "<MatterEndpointJNI()> Could not get throwableConstructor"));
    }

    jclass mCompletableFutureClass, mThrowableClass;
    jmethodID mCompletableFutureConstructor, mCompleteMethod, mCompleteExceptionallyMethod, mThrowableConstructor;

    //static void FindOrEstablishSessionTask(chip::System::Layer * aSystemLayer, void * context);

private:
    friend MatterEndpointJNI & MatterEndpointJNIMgr();
    static MatterEndpointJNI sInstance;
};

inline class MatterEndpointJNI & MatterEndpointJNIMgr()
{
    return MatterEndpointJNI::sInstance;
}
}; // namespace core
}; // namespace casting
}; // namespace matter
