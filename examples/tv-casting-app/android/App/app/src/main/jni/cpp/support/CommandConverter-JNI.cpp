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

#include "CommandConverter-JNI.h"
#include <lib/support/JniReferences.h>

namespace matter {
namespace casting {
namespace support {

using namespace chip;

jobject createJCommand(void * command, const char * className)
{
    ChipLogProgress(AppServer, "CommandConverter-JNI.createJCommand() called");
    VerifyOrReturnValue(command != nullptr, nullptr, ChipLogError(AppServer, "CommandConverter-JNI::createJCommand() command == nullptr"));
    JNIEnv * env = JniReferences::GetInstance().GetEnvForCurrentThread();

    // Get a reference to the command's Java class
    jclass commandJavaClass = env->FindClass(className);
    if (commandJavaClass == nullptr)
    {
        ChipLogError(AppServer,
                     "CommandConverter-JNI.createJCommand() could not locate command's Java class");
        return nullptr;
    }

    // Get the constructor for the command's Java class
    jmethodID constructor = env->GetMethodID(commandJavaClass, "<init>", "()V");
    if (constructor == nullptr)
    {
        ChipLogError(AppServer, "CommandConverter-JNI.createJCommand() could not locate command's Java class constructor");
        return nullptr;
    }

    // Create a new instance of the command's Java class
    jobject jMatterCommand = env->NewObject(commandJavaClass, constructor);
    if (jMatterCommand == nullptr)
    {
        ChipLogError(AppServer, "CommandConverter-JNI.createJCommand(): Could not create command's Java object");
        return jMatterCommand;
    }
    // Set the value of the _cppEndpoint field in the Java object to the C++ Endpoint pointer.
    jfieldID longFieldId = env->GetFieldID(commandJavaClass, "_cppCommand", "J");
    env->SetLongField(jMatterCommand, longFieldId, reinterpret_cast<jlong>(command));
    return jMatterCommand;
}

}; // namespace support
}; // namespace casting
}; // namespace matter
