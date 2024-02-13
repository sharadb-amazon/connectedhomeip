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

#include "Command-JNI.h"

#include "../support/CommandConverter-JNI.h"

#include <jni.h>
#include <lib/support/JniReferences.h>
#include <lib/support/JniTypeWrappers.h>

using namespace chip;

#define JNI_METHOD(RETURN, METHOD_NAME) extern "C" JNIEXPORT RETURN JNICALL Java_com_matter_casting_core_MatterCommand_##METHOD_NAME

namespace matter {
namespace casting {
namespace core {

CHIP_ERROR convertRequestFromJavaToCpp(jobject inRequest,
                                       chip::app::Clusters::ContentLauncher::Commands::LaunchURL::Type & outRequest)
{
    VerifyOrReturnError(inRequest != nullptr, CHIP_ERROR_INVALID_ARGUMENT);

    JNIEnv * env = chip::JniReferences::GetInstance().GetEnvForCurrentThread();

    jclass jRequestClass;
    ReturnErrorOnFailure(JniReferences::GetInstance().GetClassRef(
        env, "com/matter/casting/clusters/MatterCommands$ContentLauncherClusterLaunchURLRequest", jRequestClass));

    jfieldID jContentURLField = env->GetFieldID(jRequestClass, "contentURL", "Ljava/lang/String;");
    jstring jContentURLObj    = (jstring) env->GetObjectField(inRequest, jContentURLField);
    VerifyOrReturnError(jContentURLObj != nullptr, CHIP_ERROR_INVALID_ARGUMENT);
    outRequest.contentURL = CharSpan::fromCharString(env->GetStringUTFChars(jContentURLObj, 0));

    jfieldID jDisplayStringField = env->GetFieldID(jRequestClass, "displayString", "Ljava/lang/String;");
    jstring jDisplayStringObj    = (jstring) env->GetObjectField(inRequest, jDisplayStringField);
    if (jDisplayStringObj != nullptr)
    {
        const char * nativeValue = env->GetStringUTFChars(jDisplayStringObj, 0);
        outRequest.displayString = chip::Optional<CharSpan>(CharSpan::fromCharString(nativeValue));
    }
    else
    {
        outRequest.displayString = chip::NullOptional;
    }

    // TODO: translate brandingInformation
    outRequest.brandingInformation =
        chip::MakeOptional(chip::app::Clusters::ContentLauncher::Structs::BrandingInformationStruct::Type());
    return CHIP_NO_ERROR;
}

jobject
convertResponseFromCppToJava(const chip::app::Clusters::ContentLauncher::Commands::LaunchURL::Type::ResponseType * responseData)
{
    VerifyOrReturnValue(responseData != nullptr, nullptr);
    JNIEnv * env = JniReferences::GetInstance().GetEnvForCurrentThread();

    jclass responseTypeClass = nullptr;
    VerifyOrReturnValue(
        JniReferences::GetInstance().GetClassRef(env, "com/matter/casting/clusters/MatterCommands$ContentLauncherClusterResponse",
                                                 responseTypeClass) != CHIP_NO_ERROR,
        nullptr);

    jmethodID constructor = env->GetMethodID(responseTypeClass, "<init>", "()V");
    jobject jResponseObj  = env->NewObject(responseTypeClass, constructor);

    if (responseData->data.HasValue())
    {
        jfieldID dataField = env->GetFieldID(responseTypeClass, "data", "Ljava/lang/String;");
        env->SetObjectField(jResponseObj, dataField, env->NewStringUTF(responseData->data.Value().data()));
    }

    jfieldID statusField = env->GetFieldID(responseTypeClass, "status", "Ljava/lang/Integer;");
    env->SetIntField(jResponseObj, statusField, static_cast<jint>(responseData->status));

    return jResponseObj;
}

JNI_METHOD(jobject, invoke)
(JNIEnv * env, jobject thiz, jobject jRequest, jobject jContext, jobject jTimedInvokeTimeoutMs)
{
    chip::DeviceLayer::StackLock lock;
    ChipLogProgress(AppServer, "Command-JNI::invoke() called");
    void * command = CommandJNIMgr().GetCommand(thiz);
    VerifyOrReturnValue(command != nullptr, 0, ChipLogError(AppServer, "Command-JNI::invoke() command == nullptr"));

    // Create a new Java CompletableFuture
    jclass completableFutureClass          = env->FindClass("java/util/concurrent/CompletableFuture");
    jmethodID completableFutureConstructor = env->GetMethodID(completableFutureClass, "<init>", "()Ljava/lang/Object;");
    jobject completableFutureObj           = env->NewObject(completableFutureClass, completableFutureConstructor);
    jobject completableFutureObjGlobalRef  = env->NewGlobalRef(completableFutureObj);
    VerifyOrReturnValue(completableFutureObjGlobalRef != nullptr, nullptr,
                        ChipLogError(AppServer, "Command-JNI::invoke() completableFutureObjGlobalRef == nullptr"));

    jmethodID completeMethod = env->GetMethodID(completableFutureClass, "complete", "(Ljava/lang/Object;)Z");
    VerifyOrReturnValue(completeMethod != nullptr, nullptr, ChipLogError(AppServer, "ConnectCallback, completeMethod == nullptr"));

    jmethodID completeExceptionallyMethod =
        env->GetMethodID(completableFutureClass, "completeExceptionally", "(Ljava/lang/Throwable;)Z");
    VerifyOrReturnValue(completeExceptionallyMethod != nullptr, nullptr,
                        ChipLogError(AppServer, "ConnectCallback, completeExceptionallyMethod == nullptr"));

    jclass requestClass    = env->GetObjectClass(jRequest);     // Request.class
    jclass clsClass        = env->GetObjectClass(requestClass); // Class<Class>
    jmethodID mid          = env->GetMethodID(clsClass, "getName", "()Ljava/lang/String;");
    jstring jClassName     = (jstring) env->CallObjectMethod(requestClass, mid);
    const char * className = env->GetStringUTFChars(jClassName, nullptr);
    ChipLogProgress(AppServer, "Command-JNI::invoke() request className: %s", className);
    if (strcmp(className, "com.matter.casting.clusters.MatterCommands$ContentLauncherClusterLaunchURLRequest") == 0)
    {
        matter::casting::core::Command<chip::app::Clusters::ContentLauncher::Commands::LaunchURL::Type> * launchURLCommand =
            static_cast<matter::casting::core::Command<chip::app::Clusters::ContentLauncher::Commands::LaunchURL::Type> *>(command);

        // create the LaunchURL request
        chip::app::Clusters::ContentLauncher::Commands::LaunchURL::Type cppRequest;
        convertRequestFromJavaToCpp(jRequest, cppRequest);

        // call Invoke on launchURLCommand while passing in success/failure callbacks
        launchURLCommand->Invoke(
            cppRequest, jContext,
            [completableFutureObjGlobalRef, completeMethod](
                void * context, const chip::app::Clusters::ContentLauncher::Commands::LaunchURL::Type::ResponseType & response) {
                ChipLogProgress(AppServer, "LaunchURL Success with response.data: %.*s",
                                static_cast<int>(response.data.Value().size()), response.data.Value().data());

                jobject jResponse = convertResponseFromCppToJava(&response);

                chip::DeviceLayer::StackUnlock unlock;
                ChipLogProgress(AppServer, "LaunchURL Success, before env->CallBooleanMethod");
                JNIEnv * env = chip::JniReferences::GetInstance().GetEnvForCurrentThread();
                env->CallBooleanMethod(completableFutureObjGlobalRef, completeMethod, jResponse);
            },
            [completableFutureObjGlobalRef, completeExceptionallyMethod](void * context, CHIP_ERROR error) {
                ChipLogError(AppServer, "LaunchURL Failure with err %" CHIP_ERROR_FORMAT, error.Format());
                JNIEnv * env = chip::JniReferences::GetInstance().GetEnvForCurrentThread();

                jclass throwableClass = env->FindClass("java/lang/RuntimeException");
                VerifyOrReturn(throwableClass != nullptr, ChipLogError(AppServer, "LaunchURL failure, throwableClass == nullptr"));
                jmethodID throwableConstructor = env->GetMethodID(throwableClass, "<init>", "(Ljava/lang/String;)V");
                VerifyOrReturn(throwableConstructor != nullptr,
                               ChipLogError(AppServer, "LaunchURL failure, throwableConstructor == nullptr"));
                jstring errorMessage = env->NewStringUTF(error.Format());
                VerifyOrReturn(errorMessage != nullptr, ChipLogError(AppServer, "LaunchURL failure, errorMessage == nullptr"));
                jobject throwableObject = env->NewObject(throwableClass, throwableConstructor, errorMessage);
                VerifyOrReturn(throwableObject != nullptr,
                               ChipLogError(AppServer, "LaunchURL failure, throwableObject == nullptr"));

                chip::DeviceLayer::StackUnlock unlock;
                env->CallBooleanMethod(completableFutureObjGlobalRef, completeExceptionallyMethod, throwableObject);
            },
            chip::MakeOptional(static_cast<unsigned short>(5000))); // time out after kTimedInvokeCommandTimeoutMs
    }

    return completableFutureObjGlobalRef;
}

/**
 * @brief Get the matter::casting::core::Command object from the jobject jCommandObject
 */
void * CommandJNI::GetCommand(jobject jCommandObject)
{
    JNIEnv * env                = chip::JniReferences::GetInstance().GetEnvForCurrentThread();
    jclass commandClass         = env->GetObjectClass(jCommandObject);
    jfieldID _cppCommandFieldId = env->GetFieldID(commandClass, "_cppCommand", "J");
    VerifyOrReturnValue(_cppCommandFieldId != nullptr, nullptr,
                        ChipLogError(AppServer, "Command-JNI::GetCommand() _cppCommand == nullptr"));
    jlong _cppCommandValue = env->GetLongField(jCommandObject, _cppCommandFieldId);
    return reinterpret_cast<void *>(_cppCommandValue);
}

}; // namespace core
}; // namespace casting
}; // namespace matter
