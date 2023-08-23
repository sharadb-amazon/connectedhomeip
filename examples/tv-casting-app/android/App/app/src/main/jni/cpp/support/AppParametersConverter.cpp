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
#include "AppParametersConverter.h"

#include "RotatingDeviceIdUniqueIdProvider-JNI.h"
#include "CommissionableDataProviderImpl-JNI.h"
#include "DeviceAttestationCredentialsProvider-JNI.h"

#include <lib/support/JniReferences.h>
#include <lib/support/JniTypeWrappers.h>
#include <credentials/attestation_verifier/DefaultDeviceAttestationVerifier.h>
#include <credentials/attestation_verifier/DeviceAttestationVerifier.h>

namespace matter {
namespace casting {
namespace support {

class CommonCaseDeviceServerInitParamsProvider : public ServerInitParamsProvider
{
private:
    // For this example, we'll use CommonCaseDeviceServerInitParams
    chip::CommonCaseDeviceServerInitParams serverInitParams;

public:
    chip::ServerInitParams * Get()
    {
        CHIP_ERROR err = serverInitParams.InitializeStaticResourcesBeforeServerInit();
        VerifyOrReturnValue(err == CHIP_NO_ERROR, nullptr,
                            ChipLogError(AppServer, "Initialization of ServerInitParams failed %" CHIP_ERROR_FORMAT, err.Format()));
        return &serverInitParams;
    }
};

CHIP_ERROR AppParametersConverter::toCppObject(jobject inAppParameters, matter::casting::support::AppParameters &outAppParameters)
{
    ChipLogProgress(AppServer, "AppParametersConverter::toCppObject called");
    VerifyOrReturnError(inAppParameters != nullptr, CHIP_ERROR_INVALID_ARGUMENT);
    JNIEnv * env = chip::JniReferences::GetInstance().GetEnvForCurrentThread();

    jclass jAppParametersClass;
    ReturnErrorOnFailure(chip::JniReferences::GetInstance().GetClassRef(env, "com/matter/casting/support/AppParameters", jAppParametersClass));

    // get the RotatingDeviceIdUniqueIdProvider
    jmethodID getRotatingDeviceIdUniqueIdProviderMethod = env->GetMethodID(jAppParametersClass, "getRotatingDeviceIdUniqueIdProvider", "()java/lang/Object;");
    if(env->ExceptionCheck() || getRotatingDeviceIdUniqueIdProviderMethod == nullptr)
    {
        env->ExceptionDescribe();
        env->ExceptionClear();
        return CHIP_ERROR_INCORRECT_STATE;
    }

    jobject jRotatingDeviceIdUniqueIdProvider = (jobject) env->CallObjectMethod(inAppParameters, getRotatingDeviceIdUniqueIdProviderMethod);
    if (env->ExceptionCheck() || jRotatingDeviceIdUniqueIdProvider == nullptr)
    {
        env->ExceptionDescribe();
        env->ExceptionClear();
        return CHIP_ERROR_INCORRECT_STATE;
    }

    ReturnErrorOnFailure(RotatingDeviceIdUniqueIdProviderJNIMgr().Initialize(jRotatingDeviceIdUniqueIdProvider));
    RotatingDeviceIdUniqueIdProviderJNI *uniqueIdProvider = &RotatingDeviceIdUniqueIdProviderJNIMgr();
    
    // get the CommissionableDataProvider
    jmethodID getCommissionableDataProviderMethod = env->GetMethodID(jAppParametersClass, "getCommissionableDataProvider", "()java/lang/Object;");
    if(env->ExceptionCheck() || getCommissionableDataProviderMethod == nullptr)
    {
        env->ExceptionDescribe();
        env->ExceptionClear();
        return CHIP_ERROR_INCORRECT_STATE;
    }

    jobject jCommissionableDataProvider = (jobject) env->CallObjectMethod(inAppParameters, getCommissionableDataProviderMethod);
    if (env->ExceptionCheck() || jCommissionableDataProvider == nullptr)
    {
        env->ExceptionDescribe();
        env->ExceptionClear();
        return CHIP_ERROR_INCORRECT_STATE;
    }
    ReturnErrorOnFailure(CommissionableDataProviderImplJNIMgr().Initialize(jCommissionableDataProvider));
    CommissionableDataProviderImplJNI *commissionableDataProvider = &CommissionableDataProviderImplJNIMgr();

    // get the DeviceAttestationCredentialsProvider
    jmethodID getDeviceAttestationCredentialsProviderMethod = env->GetMethodID(jAppParametersClass, "getDacProvider", "()java/lang/Object;");
    if(env->ExceptionCheck() || getDeviceAttestationCredentialsProviderMethod == nullptr)
    {
        env->ExceptionDescribe();
        env->ExceptionClear();
        return CHIP_ERROR_INCORRECT_STATE;
    }

    jobject jDeviceAttestationCredentialsProvider = (jobject) env->CallObjectMethod(inAppParameters, getDeviceAttestationCredentialsProviderMethod);
    if (env->ExceptionCheck() || jDeviceAttestationCredentialsProvider == nullptr)
    {
        env->ExceptionDescribe();
        env->ExceptionClear();
        return CHIP_ERROR_INCORRECT_STATE;
    }
    ReturnErrorOnFailure(DeviceAttestationCredentialsProviderJNIMgr().Initialize(jDeviceAttestationCredentialsProvider));
    DeviceAttestationCredentialsProviderJNI *deviceAttestationCredentialsProvider = &DeviceAttestationCredentialsProviderJNIMgr();

    ReturnErrorOnFailure(outAppParameters.Create(uniqueIdProvider, commissionableDataProvider,
                               deviceAttestationCredentialsProvider,
                               GetDefaultDACVerifier(chip::Credentials::GetTestAttestationTrustStore()), &serverInitParamsProvider));
    VerifyOrReturnValue(err == CHIP_NO_ERROR, 0,
                        ChipLogError(AppServer, "Creation of AppParameters failed %" CHIP_ERROR_FORMAT, err.Format()));

    chip::DeviceLayer::PersistedStorage::KeyValueStoreMgrImpl().InitializeWithObject(nullptr);

    /*CHIP_ERROR Create(MutableByteSpanDataProvider * rotatingDeviceIdUniqueIdProvider,
                      chip::DeviceLayer::CommissionableDataProvider * commissionableDataProvider,
                      chip::Credentials::DeviceAttestationCredentialsProvider * deviceAttestationCredentialsProvider,
                      chip::Credentials::DeviceAttestationVerifier * deviceAttestationVerifier,
                      ServerInitParamsProvider * serverInitParamsProvider)

    out.Create(uniqueIdProvider, )
    return CHIP_NO_ERROR;*/

    return CHIP_ERROR_NOT_IMPLEMENTED;
}

CHIP_ERROR AppParametersConverter::toJObject(matter::casting::support::AppParameters *in, jobject &out)
{
    return CHIP_ERROR_NOT_IMPLEMENTED;
}

}; // namespace support
}; // namespace casting
}; // namespace matter