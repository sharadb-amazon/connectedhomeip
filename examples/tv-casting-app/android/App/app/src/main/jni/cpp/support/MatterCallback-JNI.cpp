/*
 *   Copyright (c) 2023 Project CHIP Authors
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

#include "MatterCallback-JNI.h"

CHIP_ERROR MatterCallbackBaseJNI::SetUp(JNIEnv * env, jobject inHandler)
{
    ChipLogProgress(AppServer, "MatterCallbackBaseJNI::SetUp called");
    CHIP_ERROR err = CHIP_NO_ERROR;

    mObject = env->NewGlobalRef(inHandler);
    VerifyOrExit(mObject != nullptr, ChipLogError(AppServer, "Failed to NewGlobalRef for handler object"));

    mClazz = env->GetObjectClass(inHandler);
    VerifyOrExit(mClazz != nullptr, ChipLogError(AppServer, "Failed to get handler Java class"));

    mSuperClazz = env->GetSuperclass(mClazz);
    VerifyOrExit(mSuperClazz != nullptr, ChipLogError(AppServer, "Failed to get handler's parent's Java class"));

    mMethod = env->GetMethodID(mClazz, "handleInternal", mMethodSignature);
    if (mMethod == nullptr)
    {
        ChipLogError(AppServer, "Failed to access 'handleInternal' method with signature %s", mMethodSignature);
        env->ExceptionClear();
    }

exit:
    if (err != CHIP_NO_ERROR)
    {
        ChipLogError(AppServer, "MatterCallbackBaseJNI::SetUp error: %s", err.AsString());
        return err;
    }

    return err;
}

void MatterFailureHandlerJNI::Handle(CHIP_ERROR callbackErr)
{
    ChipLogProgress(AppServer, "MatterFailureHandlerJNI called with %" CHIP_ERROR_FORMAT, callbackErr.Format());

    JNIEnv * env = chip::JniReferences::GetInstance().GetEnvForCurrentThread();
    chip::UtfString jniCallbackErrString(env, callbackErr.AsString());

    chip::DeviceLayer::StackUnlock unlock;
    CHIP_ERROR err = CHIP_NO_ERROR;
    VerifyOrExit(mObject != nullptr, err = CHIP_ERROR_INCORRECT_STATE);
    VerifyOrExit(mMethod != nullptr, err = CHIP_ERROR_INCORRECT_STATE);
    env->CallVoidMethod(mObject, mMethod, static_cast<jint>(callbackErr.AsInteger()), jniCallbackErrString.jniValue());
exit:
    if (err != CHIP_NO_ERROR)
    {
        ChipLogError(AppServer, "MatterFailureHandlerJNI status error: %s", err.AsString());
    }
}

jobject ConvertToLongJObject(jlong responseData)
{
    JNIEnv * env = chip::JniReferences::GetInstance().GetEnvForCurrentThread();

    jclass responseTypeClass = env->FindClass("java/lang/Long");
    if (responseTypeClass == nullptr)
    {
        ChipLogError(AppServer, "ConvertToJObject: Class for Response Type not found!");
        return nullptr;
    }

    jmethodID constructor = env->GetMethodID(responseTypeClass, "<init>", "(J)V");
    return env->NewObject(responseTypeClass, constructor, responseData);
}

jobject DeviceProxyMatterSuccessHandlerJNI::ConvertToJObject(jlong responseData)
{
    ChipLogProgress(AppServer, "DeviceProxyMatterSuccessHandlerJNI::ConvertToJObject called");
    return ConvertToLongJObject(responseData);
}
