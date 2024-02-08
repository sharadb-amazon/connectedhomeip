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

#define JNI_METHOD(RETURN, METHOD_NAME)                                                                                            \
    extern "C" JNIEXPORT RETURN JNICALL Java_com_matter_casting_core_MatterCommand_##METHOD_NAME

namespace matter {
namespace casting {
namespace core {

JNI_METHOD(jobject, invoke)
(JNIEnv * env, jobject thiz, jobject jRequest, jobject jContext, jobject jTimedInvokeTimeoutMs)
{
    chip::DeviceLayer::StackLock lock;
    ChipLogProgress(AppServer, "Command-JNI::invoke() called");
    void * command = CommandJNIMgr().GetCommand(thiz);
    VerifyOrReturnValue(command != nullptr, 0, ChipLogError(AppServer, "Command-JNI::invoke() command == nullptr"));

    jclass requestClass = env->GetObjectClass(jRequest); // Request.class
    jclass clsClass = env->GetObjectClass(requestClass); // Class<Class>
    jmethodID mid = env->GetMethodID(clsClass, "getName", "()Ljava/lang/String;");
    jstring jClassName = (jstring)env->CallObjectMethod(requestClass, mid);
    const char* className = env->GetStringUTFChars(jClassName, nullptr);
    ChipLogProgress(AppServer, "Command-JNI::invoke() request className: %s", className);
    if (strcmp(className, "com.matter.casting.clusters.MatterCommands$ContentLauncherClusterLaunchURLRequest") == 0)
    {
        matter::casting::core::Command<chip::app::Clusters::ContentLauncher::Commands::LaunchURL::Type> * launchURLCommand =
        static_cast<matter::casting::core::Command<chip::app::Clusters::ContentLauncher::Commands::LaunchURL::Type> *>(command);
    
    // create the LaunchURL request
    chip::app::Clusters::ContentLauncher::Commands::LaunchURL::Type request;
    request.contentURL    = chip::CharSpan::fromCharString("test test url");
    request.displayString = chip::Optional<chip::CharSpan>(chip::CharSpan::fromCharString("test display string"));
    request.brandingInformation =
        chip::MakeOptional(chip::app::Clusters::ContentLauncher::Structs::BrandingInformationStruct::Type());

    // call Invoke on launchURLCommand while passing in success/failure callbacks
    launchURLCommand->Invoke(
        request, jContext,
        [](void * context, const chip::app::Clusters::ContentLauncher::Commands::LaunchURL::Type::ResponseType & response) {
            ChipLogProgress(AppServer, "LaunchURL Success with response.data: %.*s", static_cast<int>(response.data.Value().size()),
                            response.data.Value().data());
        },
        [](void * context, CHIP_ERROR error) {
            ChipLogError(AppServer, "LaunchURL Failure with err %" CHIP_ERROR_FORMAT, error.Format());
        },
        chip::MakeOptional(static_cast<unsigned short>(5000))); // time out after kTimedInvokeCommandTimeoutMs
    }
    
    return nullptr;
}

/**
 * @brief Get the matter::casting::core::Command object from the jobject jCommandObject
 */
void * CommandJNI::GetCommand(jobject jCommandObject)
{
    JNIEnv * env = chip::JniReferences::GetInstance().GetEnvForCurrentThread();
    jclass commandClass         = env->GetObjectClass(jCommandObject);
    jfieldID _cppCommandFieldId = env->GetFieldID(commandClass, "_cppCommand", "J");
    VerifyOrReturnValue(_cppCommandFieldId != nullptr, nullptr, ChipLogError(AppServer, "Command-JNI::GetCommand() _cppCommand == nullptr"));
    jlong _cppCommandValue  = env->GetLongField(jCommandObject, _cppCommandFieldId);
    return reinterpret_cast<void *>(_cppCommandValue);
}

}; // namespace core
}; // namespace casting
}; // namespace matter
