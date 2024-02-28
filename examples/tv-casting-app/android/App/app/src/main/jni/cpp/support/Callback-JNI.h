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

class CallbackBaseJNI
{
public:
    CallbackBaseJNI(const char * methodSignature) { mMethodSignature = methodSignature; }
    CHIP_ERROR SetUp(JNIEnv * env, jobject inHandler);

protected:
    jobject mObject               = nullptr;
    jclass mClazz                 = nullptr;
    jclass mSuperClazz            = nullptr;
    jmethodID mMethod             = nullptr;
    const char * mMethodSignature = nullptr;
};

class FailureHandlerJNI : public CallbackBaseJNI
{
public:
    FailureHandlerJNI() : CallbackBaseJNI("(ILjava/lang/String;)V") {}
    void Handle(CHIP_ERROR err);
};

template <typename T>
class SuccessHandlerJNI : public CallbackBaseJNI
{
public:
    SuccessHandlerJNI(const char * methodSignature) : CallbackBaseJNI(methodSignature) {}

    virtual ~SuccessHandlerJNI() = 0;

    virtual jobject ConvertToJObject(T responseData) = 0;

    void Handle(T responseData)
    {
        ChipLogProgress(AppServer, "SuccessHandlerJNI::Handle called");

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
            ChipLogError(AppServer, "SuccessHandlerJNI::Handle status error: %s", err.AsString());
        }
    }
};

template <typename T>
SuccessHandlerJNI<T>::~SuccessHandlerJNI(){};

class DeviceProxySuccessHandlerJNI
    : public SuccessHandlerJNI<chip::app::Clusters::MediaPlayback::Attributes::Duration::TypeInfo::DecodableArgType>
{
public:
    DeviceProxySuccessHandlerJNI() : SuccessHandlerJNI("(Ljava/lang/Long;)V") {}
    jobject ConvertToJObject(chip::app::Clusters::MediaPlayback::Attributes::Duration::TypeInfo::DecodableArgType responseData);
};