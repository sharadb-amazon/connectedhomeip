/*
 *
 *    Copyright (c) 2023 Project CHIP Authors
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

#include "Converters-JNI.h"

#include <jni.h>
#include <lib/support/JniReferences.h>
#include <lib/support/JniTypeWrappers.h>

class MatterCallbackBaseJNI
{
public:
    MatterCallbackBaseJNI(const char * methodSignature) { mMethodSignature = methodSignature; }
    CHIP_ERROR SetUp(JNIEnv * env, jobject inHandler);

protected:
    jobject mObject               = nullptr;
    jclass mClazz                 = nullptr;
    jclass mSuperClazz            = nullptr;
    jmethodID mMethod             = nullptr;
    const char * mMethodSignature = nullptr;
};

class MatterFailureHandlerJNI : public MatterCallbackBaseJNI
{
public:
    MatterFailureHandlerJNI() : MatterCallbackBaseJNI("(ILjava/lang/String;)V") {}
    void Handle(CHIP_ERROR err);
};

template <typename T>
class MatterSuccessHandlerJNI : public MatterCallbackBaseJNI
{
public:
    MatterSuccessHandlerJNI(const char * methodSignature) : MatterCallbackBaseJNI(methodSignature) {}

    virtual ~MatterSuccessHandlerJNI() = 0;

    virtual jobject ConvertToJObject(T responseData) = 0;

    void Handle(T responseData)
    {
        ChipLogProgress(AppServer, "MatterSuccessHandlerJNI::Handle called");

        JNIEnv * env          = chip::JniReferences::GetInstance().GetEnvForCurrentThread();
        jobject jResponseData = ConvertToJObject(responseData);

        chip::DeviceLayer::StackUnlock unlock;
        CHIP_ERROR err = CHIP_NO_ERROR;
        VerifyOrExit(mObject != nullptr, err = CHIP_ERROR_INCORRECT_STATE);
        VerifyOrExit(mMethod != nullptr, err = CHIP_ERROR_INCORRECT_STATE);
        env->CallVoidMethod(mObject, mMethod, jResponseData);
    exit:
        if (err != CHIP_NO_ERROR)
        {
            ChipLogError(AppServer, "MatterSuccessHandlerJNI::Handle status error: %s", err.AsString());
        }
    }
};

template <typename T>
MatterSuccessHandlerJNI<T>::~MatterSuccessHandlerJNI(){};

class DeviceProxyMatterSuccessHandlerJNI
    : public MatterSuccessHandlerJNI<jlong>
{
public:
    DeviceProxyMatterSuccessHandlerJNI() : MatterSuccessHandlerJNI("(Ljava/lang/Long;)V") {}
    jobject ConvertToJObject(jlong responseData);
};