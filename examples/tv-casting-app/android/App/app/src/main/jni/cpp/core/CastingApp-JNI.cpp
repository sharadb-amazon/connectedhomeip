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

#include "CastingApp-JNI.h"

#include "core/CastingApp.h" // from tv-casting-common

#include <jni.h>
#include <lib/support/JniReferences.h>
#include <lib/support/JniTypeWrappers.h>

using namespace chip;

#define JNI_METHOD(RETURN, METHOD_NAME) extern "C" JNIEXPORT RETURN JNICALL Java_com_matter_casting_core_CastingApp_##METHOD_NAME

namespace matter {
namespace casting {
namespace core {
namespace jni {

CastingAppJNI CastingAppJNI::sInstance;

JNI_METHOD(jobject, _initialize)(JNIEnv *, jobject, jobject jAppParameters)
{
    chip::DeviceLayer::StackLock lock;
    ChipLogProgress(AppServer, "JNI_METHOD initialize called");
    matter::casting::support::AppParameters appParameters;
    matter::casting::core::CastingApp::GetInstance()->Initialize(appParameters);
    return nullptr;
}
}; // namespace jni
}; // namespace core
}; // namespace casting
}; // namespace matter
