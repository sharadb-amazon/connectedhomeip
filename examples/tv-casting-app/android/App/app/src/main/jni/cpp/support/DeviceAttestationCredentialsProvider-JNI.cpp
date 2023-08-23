/*
 *
 *    Copyright (c) 2021 Project CHIP Authors
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

#include "DeviceAttestationCredentialsProvider-JNI.h"
#include "lib/support/logging/CHIPLogging.h"
#include <credentials/CHIPCert.h>
#include <crypto/CHIPCryptoPAL.h>
#include <cstdlib>
#include <jni.h>
#include <lib/core/CHIPError.h>
#include <lib/support/CHIPJNIError.h>
#include <lib/support/JniReferences.h>
#include <lib/support/JniTypeWrappers.h>
#include <lib/support/Span.h>

using namespace chip;

namespace matter {
namespace casting {
namespace support {

CHIP_ERROR DeviceAttestationCredentialsProviderJNI::Initialize(jobject provider)
{
    JNIEnv * env = JniReferences::GetInstance().GetEnvForCurrentThread();
    VerifyOrReturnValue(env != nullptr, CHIP_ERROR_INCORRECT_STATE, ChipLogError(AppServer, "Failed to GetEnvForCurrentThread for DeviceAttestationCredentialsProvider"));

    mJNIDACProviderObject = env->NewGlobalRef(provider);
    VerifyOrReturnValue(mJNIDACProviderObject != nullptr, CHIP_ERROR_INCORRECT_STATE, ChipLogError(AppServer, "Failed to NewGlobalRef DeviceAttestationCredentialsProvider"));

    jclass JNIDACProviderClass = env->GetObjectClass(provider);
    VerifyOrReturnValue(JNIDACProviderClass != nullptr, CHIP_ERROR_INCORRECT_STATE, ChipLogError(AppServer, "Failed to get DeviceAttestationCredentialsProvider Java class"));

    mGetCertificationDeclarationMethod = env->GetMethodID(JNIDACProviderClass, "GetCertificationDeclaration", "()[B");
    if (mGetCertificationDeclarationMethod == nullptr)
    {
        ChipLogError(AppServer, "Failed to access DeviceAttestationCredentialsProvider 'GetCertificationDeclaration' method");
        env->ExceptionClear();
        return CHIP_ERROR_INCORRECT_STATE;
    }

    mGetFirmwareInformationMethod = env->GetMethodID(JNIDACProviderClass, "GetFirmwareInformation", "()[B");
    if (mGetFirmwareInformationMethod == nullptr)
    {
        ChipLogError(AppServer, "Failed to access DeviceAttestationCredentialsProvider 'GetFirmwareInformation' method");
        env->ExceptionClear();
        return CHIP_ERROR_INCORRECT_STATE;
    }

    mGetDeviceAttestationCertMethod = env->GetMethodID(JNIDACProviderClass, "GetDeviceAttestationCert", "()[B");
    if (mGetDeviceAttestationCertMethod == nullptr)
    {
        ChipLogError(AppServer, "Failed to access DeviceAttestationCredentialsProvider 'GetDeviceAttestationCert' method");
        env->ExceptionClear();
        return CHIP_ERROR_INCORRECT_STATE;
    }

    mGetProductAttestationIntermediateCertMethod =
        env->GetMethodID(JNIDACProviderClass, "GetProductAttestationIntermediateCert", "()[B");
    if (mGetProductAttestationIntermediateCertMethod == nullptr)
    {
        ChipLogError(AppServer, "Failed to access DeviceAttestationCredentialsProvider 'GetProductAttestationIntermediateCert' method");
        env->ExceptionClear();
        return CHIP_ERROR_INCORRECT_STATE;
    }

    mSignWithDeviceAttestationKeyMethod = env->GetMethodID(JNIDACProviderClass, "SignWithDeviceAttestationKey", "([B)[B");
    if (mSignWithDeviceAttestationKeyMethod == nullptr)
    {
        ChipLogError(AppServer, "Failed to access DeviceAttestationCredentialsProvider 'SignWithDeviceAttestationKey' method");
        env->ExceptionClear();
        return CHIP_ERROR_INCORRECT_STATE;
    }
    return CHIP_NO_ERROR;
}

CHIP_ERROR DeviceAttestationCredentialsProviderJNI::GetJavaByteByMethod(jmethodID method, MutableByteSpan & out_buffer)
{
    JNIEnv * env = JniReferences::GetInstance().GetEnvForCurrentThread();
    VerifyOrReturnLogError(mJNIDACProviderObject != nullptr, CHIP_ERROR_INCORRECT_STATE);
    VerifyOrReturnLogError(method != nullptr, CHIP_ERROR_INCORRECT_STATE);
    VerifyOrReturnLogError(env != nullptr, CHIP_JNI_ERROR_NO_ENV);

    jbyteArray outArray = (jbyteArray) env->CallObjectMethod(mJNIDACProviderObject, method);
    if (env->ExceptionCheck())
    {
        ChipLogError(AppServer, "Java exception in get Method");
        env->ExceptionDescribe();
        env->ExceptionClear();
        return CHIP_ERROR_INCORRECT_STATE;
    }

    if (outArray == nullptr || env->GetArrayLength(outArray) <= 0)
    {
        out_buffer.reduce_size(0);
        return CHIP_NO_ERROR;
    }

    JniByteArray JniOutArray(env, outArray);
    return CopySpanToMutableSpan(JniOutArray.byteSpan(), out_buffer);
}

CHIP_ERROR DeviceAttestationCredentialsProviderJNI::GetJavaByteByMethod(jmethodID method, const ByteSpan & in_buffer, MutableByteSpan & out_buffer)
{
    JNIEnv * env = JniReferences::GetInstance().GetEnvForCurrentThread();
    VerifyOrReturnLogError(mJNIDACProviderObject != nullptr, CHIP_ERROR_INCORRECT_STATE);
    VerifyOrReturnLogError(method != nullptr, CHIP_ERROR_INCORRECT_STATE);
    VerifyOrReturnLogError(env != nullptr, CHIP_JNI_ERROR_NO_ENV);

    jbyteArray in_buffer_jbyteArray = env->NewByteArray((jsize)(in_buffer.size()));
    env->SetByteArrayRegion(in_buffer_jbyteArray, 0, (int) in_buffer.size(), reinterpret_cast<const jbyte *>(in_buffer.data()));

    jbyteArray outArray = (jbyteArray) env->CallObjectMethod(mJNIDACProviderObject, method, in_buffer_jbyteArray);
    if (env->ExceptionCheck())
    {
        ChipLogError(AppServer, "Java exception in get Method");
        env->ExceptionDescribe();
        env->ExceptionClear();
        return CHIP_ERROR_INCORRECT_STATE;
    }

    env->DeleteLocalRef(in_buffer_jbyteArray);

    if (outArray == nullptr || env->GetArrayLength(outArray) <= 0)
    {
        out_buffer.reduce_size(0);
        return CHIP_NO_ERROR;
    }

    JniByteArray JniOutArray(env, outArray);
    return CopySpanToMutableSpan(JniOutArray.byteSpan(), out_buffer);
}

CHIP_ERROR DeviceAttestationCredentialsProviderJNI::GetCertificationDeclaration(MutableByteSpan & out_cd_buffer)
{
    ChipLogProgress(AppServer, "Received GetCertificationDeclaration");
    return GetJavaByteByMethod(mGetCertificationDeclarationMethod, out_cd_buffer);
}

CHIP_ERROR DeviceAttestationCredentialsProviderJNI::GetFirmwareInformation(MutableByteSpan & out_firmware_info_buffer)
{
    ChipLogProgress(AppServer, "Received GetFirmwareInformation");
    return GetJavaByteByMethod(mGetFirmwareInformationMethod, out_firmware_info_buffer);
}

CHIP_ERROR DeviceAttestationCredentialsProviderJNI::GetDeviceAttestationCert(MutableByteSpan & out_dac_buffer)
{
    ChipLogProgress(AppServer, "Received GetDeviceAttestationCert");
    return GetJavaByteByMethod(mGetDeviceAttestationCertMethod, out_dac_buffer);
}

CHIP_ERROR DeviceAttestationCredentialsProviderJNI::GetProductAttestationIntermediateCert(MutableByteSpan & out_pai_buffer)
{
    ChipLogProgress(AppServer, "Received GetProductAttestationIntermediateCert");
    return GetJavaByteByMethod(mGetProductAttestationIntermediateCertMethod, out_pai_buffer);
}

CHIP_ERROR DeviceAttestationCredentialsProviderJNI::SignWithDeviceAttestationKey(const ByteSpan & message_to_sign, MutableByteSpan & out_signature_buffer)
{
    ChipLogProgress(AppServer, "Received SignWithDeviceAttestationKey");
    uint8_t mAsn1SignatureBytes[73];

    MutableByteSpan asn1_signature_buffer(mAsn1SignatureBytes, sizeof(mAsn1SignatureBytes));

    CHIP_ERROR error = GetJavaByteByMethod(mSignWithDeviceAttestationKeyMethod, message_to_sign, asn1_signature_buffer);
    if (error != CHIP_NO_ERROR)
    {
        ChipLogProgress(AppServer, "SignWithDeviceAttestationKey failed");
        return error;
    }

    return chip::Crypto::EcdsaAsn1SignatureToRaw(32, ByteSpan(asn1_signature_buffer.data(), asn1_signature_buffer.size()),
                                                 out_signature_buffer);
}

}; // namespace support
}; // namespace casting
}; // namespace matter