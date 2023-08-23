/*
 *
 *    Copyright (c) 2023 Project CHIP Authors
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

#include "CommissionableDataProviderImpl-JNI.h"

#include <cstdint>
#include <string.h>

#include <crypto/CHIPCryptoPAL.h>
#include <lib/support/Base64.h>
#include <lib/support/CodeUtils.h>
#include <lib/support/JniTypeWrappers.h>
#include <lib/support/Span.h>
#include <lib/support/logging/CHIPLogging.h>
#include <platform/CHIPDeviceConfig.h>

using namespace chip;
using namespace chip::Crypto;

#ifndef CHIP_DEVICE_CONFIG_USE_TEST_SPAKE2P_ITERATION_COUNT
#define CHIP_DEVICE_CONFIG_USE_TEST_SPAKE2P_ITERATION_COUNT 1000
#endif

namespace matter {
namespace casting {
namespace support {

CHIP_ERROR GeneratePaseSalt(std::vector<uint8_t> & spake2pSaltVector)
{
    constexpr size_t kSaltLen = kSpake2p_Max_PBKDF_Salt_Length;
    spake2pSaltVector.resize(kSaltLen);
    return DRBG_get_bytes(spake2pSaltVector.data(), spake2pSaltVector.size());
}

CHIP_ERROR CommissionableDataProviderImplJNI::Initialize(jobject provider)
{
    JNIEnv * env = JniReferences::GetInstance().GetEnvForCurrentThread();
    VerifyOrReturnValue(env != nullptr, CHIP_ERROR_INCORRECT_STATE, ChipLogError(AppServer, "Failed to GetEnvForCurrentThread for RotatingDeviceIdUniqueIdProviderJNI"));

    jobject JNIProviderObject = env->NewGlobalRef(provider);
    VerifyOrReturnValue(JNIProviderObject != nullptr, CHIP_ERROR_INCORRECT_STATE, ChipLogError(AppServer, "Failed to NewGlobalRef JNIProvider"));

    jclass JNIProviderClass = env->GetObjectClass(provider);
    VerifyOrReturnValue(JNIProviderClass != nullptr, CHIP_ERROR_INCORRECT_STATE, ChipLogError(AppServer, "Failed to get JNIProvider Java class"));

    jmethodID getMethod = env->GetMethodID(JNIProviderClass, "get", "()java/lang/Object;");
    if (getMethod == nullptr)
    {
        ChipLogError(AppServer, "Failed to access JNIProvider 'get' method");
        env->ExceptionClear();
        return CHIP_ERROR_INCORRECT_STATE;
    }

    jobject commissionableData = env->CallObjectMethod(JNIProviderObject, getMethod);
    if (env->ExceptionCheck())
    {
        ChipLogError(AppServer, "Java exception in Get Method");
        env->ExceptionDescribe();
        env->ExceptionClear();
        return CHIP_ERROR_INCORRECT_STATE;
    }
    VerifyOrReturnLogError(commissionableData != nullptr, CHIP_ERROR_INVALID_ARGUMENT);

    jclass JNICommissionableDataClass = env->GetObjectClass(commissionableData);
    VerifyOrReturnValue(JNICommissionableDataClass != nullptr, CHIP_ERROR_INCORRECT_STATE, ChipLogError(AppServer, "Failed to get CommissionableData Java class"));

    // get the setup passcode
    jmethodID getSetupPasscodeMethod = env->GetMethodID(JNICommissionableDataClass, "getSetupPasscode", "()J");
    if (getSetupPasscodeMethod == nullptr)
    {
        ChipLogError(AppServer, "Failed to access CommissionableData 'getSetupPasscode' method");
        env->ExceptionClear();
        return CHIP_ERROR_INCORRECT_STATE;
    }
    jlong setupPasscode = env->CallLongMethod(commissionableData, getSetupPasscodeMethod);
    if (env->ExceptionCheck())
    {
        ChipLogError(DeviceLayer, "Java exception in calling CommissionableData.getSetupPasscode method");
        env->ExceptionDescribe();
        return CHIP_ERROR_INCORRECT_STATE;
    }

    // get the discriminator
    jmethodID getDiscriminatorMethod = env->GetMethodID(JNICommissionableDataClass, "getDiscriminator", "()I");
    if (getDiscriminatorMethod == nullptr)
    {
        ChipLogError(AppServer, "Failed to access CommissionableData 'getDiscriminator' method");
        env->ExceptionClear();
        return CHIP_ERROR_INCORRECT_STATE;
    }
    jint discriminator = env->CallIntMethod(commissionableData, getDiscriminatorMethod);
    if (env->ExceptionCheck())
    {
        ChipLogError(DeviceLayer, "Java exception in calling CommissionableData.getDiscriminator method");
        env->ExceptionDescribe();
        return CHIP_ERROR_INCORRECT_STATE;
    }

    jint spake2pIterationCount = 0;
    jstring spake2pVerifierBase64 = nullptr, Spake2pSaltBase64 = nullptr;

    VerifyOrReturnLogError(discriminator <= chip::kMaxDiscriminatorValue, CHIP_ERROR_INVALID_ARGUMENT);

    if (spake2pIterationCount == 0)
    {
        spake2pIterationCount = CHIP_DEVICE_CONFIG_USE_TEST_SPAKE2P_ITERATION_COUNT;
    }
    VerifyOrReturnLogError(static_cast<uint32_t>(spake2pIterationCount) >= kSpake2p_Min_PBKDF_Iterations,
                           CHIP_ERROR_INVALID_ARGUMENT);
    VerifyOrReturnLogError(static_cast<uint32_t>(spake2pIterationCount) <= kSpake2p_Max_PBKDF_Iterations,
                           CHIP_ERROR_INVALID_ARGUMENT);

    const bool havePaseVerifier = (spake2pVerifierBase64 != nullptr);
    const bool havePaseSalt     = (Spake2pSaltBase64 != nullptr);
    VerifyOrReturnLogError(!havePaseVerifier || (havePaseVerifier && havePaseSalt), CHIP_ERROR_INVALID_ARGUMENT);

    CHIP_ERROR err;
    // read verifier from paramter is have
    Spake2pVerifier providedVerifier;
    std::vector<uint8_t> serializedSpake2pVerifier(kSpake2p_VerifierSerialized_Length);
    if (havePaseVerifier)
    {
        chip::JniUtfString utfSpake2pVerifierBase64(env, spake2pVerifierBase64);

        size_t maxBase64Size = BASE64_ENCODED_LEN(chip::Crypto::kSpake2p_VerifierSerialized_Length);
        VerifyOrReturnLogError(static_cast<unsigned>(utfSpake2pVerifierBase64.size()) <= maxBase64Size,
                               CHIP_ERROR_INVALID_ARGUMENT);

        size_t decodedLen = chip::Base64Decode32(utfSpake2pVerifierBase64.c_str(), static_cast<uint32_t>(utfSpake2pVerifierBase64.size()),
                                                 reinterpret_cast<uint8_t *>(serializedSpake2pVerifier.data()));
        VerifyOrReturnLogError(decodedLen == chip::Crypto::kSpake2p_VerifierSerialized_Length, CHIP_ERROR_INVALID_ARGUMENT);

        chip::MutableByteSpan verifierSpan{ serializedSpake2pVerifier.data(), decodedLen };
        err = providedVerifier.Deserialize(verifierSpan);
        VerifyOrReturnLogError(err == CHIP_NO_ERROR, err);

        ChipLogProgress(Support, "Got externally provided verifier, using it.");
    }

    // read slat from paramter is have or generate one
    std::vector<uint8_t> spake2pSalt(chip::Crypto::kSpake2p_Max_PBKDF_Salt_Length);
    if (!havePaseSalt)
    {
        ChipLogProgress(Support, "LinuxCommissionableDataProvider didn't get a PASE salt, generating one.");
        err = GeneratePaseSalt(spake2pSalt);
        VerifyOrReturnLogError(err == CHIP_NO_ERROR, err);
    }
    else
    {
        chip::JniUtfString utfSpake2pSaltBase64(env, Spake2pSaltBase64);

        size_t maxBase64Size = BASE64_ENCODED_LEN(chip::Crypto::kSpake2p_Max_PBKDF_Salt_Length);
        VerifyOrReturnLogError(static_cast<unsigned>(utfSpake2pSaltBase64.size()) <= maxBase64Size, CHIP_ERROR_INVALID_ARGUMENT);

        size_t decodedLen = chip::Base64Decode32(utfSpake2pSaltBase64.c_str(), static_cast<uint32_t>(utfSpake2pSaltBase64.size()),
                                                 reinterpret_cast<uint8_t *>(spake2pSalt.data()));
        VerifyOrReturnLogError(decodedLen >= chip::Crypto::kSpake2p_Min_PBKDF_Salt_Length &&
                                   decodedLen <= chip::Crypto::kSpake2p_Max_PBKDF_Salt_Length,
                               CHIP_ERROR_INVALID_ARGUMENT);
        spake2pSalt.resize(decodedLen);
    }

    // generate verifier from passcode is have
    const bool havePasscode = (setupPasscode > kMinSetupPasscode && setupPasscode < kMaxSetupPasscode);
    Spake2pVerifier passcodeVerifier;
    std::vector<uint8_t> serializedPasscodeVerifier(kSpake2p_VerifierSerialized_Length);
    chip::MutableByteSpan saltSpan{ spake2pSalt.data(), spake2pSalt.size() };
    if (havePasscode)
    {
        uint32_t u32SetupPasscode = static_cast<uint32_t>(setupPasscode);
        err                       = passcodeVerifier.Generate(static_cast<uint32_t>(spake2pIterationCount), saltSpan, u32SetupPasscode);
        VerifyOrReturnLogError(err == CHIP_NO_ERROR, err);

        chip::MutableByteSpan verifierSpan{ serializedPasscodeVerifier.data(), serializedPasscodeVerifier.size() };
        err = passcodeVerifier.Serialize(verifierSpan);
        VerifyOrReturnLogError(err == CHIP_NO_ERROR, err);
    }

    // Make sure we actually have a verifier
    VerifyOrReturnLogError(havePasscode || havePaseVerifier, CHIP_ERROR_INVALID_ARGUMENT);

    // If both passcode and external verifier were provided, validate they match, otherwise
    // it's ambiguous.
    if (havePasscode && havePaseVerifier)
    {
        VerifyOrReturnLogError(serializedPasscodeVerifier == serializedSpake2pVerifier, CHIP_ERROR_INVALID_ARGUMENT);
        ChipLogProgress(Support, "Validated externally provided passcode matches the one generated from provided passcode.");
    }

    // External PASE verifier takes precedence when present (even though it is identical to passcode-based
    // one when the latter is present).
    if (havePaseVerifier)
    {
        mSerializedPaseVerifier = std::move(serializedSpake2pVerifier);
    }
    else
    {
        mSerializedPaseVerifier = std::move(serializedPasscodeVerifier);
    }
    mDiscriminator      = static_cast<uint16_t>(discriminator);
    mPaseSalt           = std::move(spake2pSalt);
    mPaseIterationCount = static_cast<uint32_t>(spake2pIterationCount);
    if (havePasscode)
    {
        mSetupPasscode.SetValue(static_cast<uint32_t>(setupPasscode));
    }

    // Set to global CommissionableDataProvider once success first time
    if (!mFirstUpdated)
    {
        DeviceLayer::SetCommissionableDataProvider(this);
    }
    mFirstUpdated = true;

    return CHIP_NO_ERROR;
}

CHIP_ERROR CommissionableDataProviderImplJNI::GetSetupDiscriminator(uint16_t & setupDiscriminator)
{
    VerifyOrReturnError(mFirstUpdated, CHIP_ERROR_INCORRECT_STATE);
    setupDiscriminator = mDiscriminator;
    return CHIP_NO_ERROR;
}

CHIP_ERROR CommissionableDataProviderImplJNI::GetSpake2pIterationCount(uint32_t & iterationCount)
{
    VerifyOrReturnLogError(mFirstUpdated, CHIP_ERROR_INCORRECT_STATE);
    iterationCount = mPaseIterationCount;
    return CHIP_NO_ERROR;
}

CHIP_ERROR CommissionableDataProviderImplJNI::GetSpake2pSalt(chip::MutableByteSpan & saltBuf)
{
    VerifyOrReturnError(mFirstUpdated, CHIP_ERROR_INCORRECT_STATE);

    VerifyOrReturnError(saltBuf.size() >= kSpake2p_Max_PBKDF_Salt_Length, CHIP_ERROR_BUFFER_TOO_SMALL);
    memcpy(saltBuf.data(), mPaseSalt.data(), mPaseSalt.size());
    saltBuf.reduce_size(mPaseSalt.size());

    return CHIP_NO_ERROR;
}

CHIP_ERROR CommissionableDataProviderImplJNI::GetSpake2pVerifier(chip::MutableByteSpan & verifierBuf, size_t & outVerifierLen)
{
    VerifyOrReturnError(mFirstUpdated, CHIP_ERROR_INCORRECT_STATE);

    // By now, serialized verifier from Init should be correct size
    VerifyOrReturnError(mSerializedPaseVerifier.size() == kSpake2p_VerifierSerialized_Length, CHIP_ERROR_INTERNAL);

    outVerifierLen = mSerializedPaseVerifier.size();
    VerifyOrReturnError(verifierBuf.size() >= outVerifierLen, CHIP_ERROR_BUFFER_TOO_SMALL);
    memcpy(verifierBuf.data(), mSerializedPaseVerifier.data(), mSerializedPaseVerifier.size());
    verifierBuf.reduce_size(mSerializedPaseVerifier.size());

    return CHIP_NO_ERROR;
}

CHIP_ERROR CommissionableDataProviderImplJNI::GetSetupPasscode(uint32_t & setupPasscode)
{
    VerifyOrReturnError(mFirstUpdated, CHIP_ERROR_INCORRECT_STATE);

    // Pretend not implemented if we don't have a passcode value externally set
    if (!mSetupPasscode.HasValue())
    {
        return CHIP_ERROR_NOT_IMPLEMENTED;
    }

    setupPasscode = mSetupPasscode.Value();
    return CHIP_NO_ERROR;
}

}; // namespace support
}; // namespace casting
}; // namespace matter