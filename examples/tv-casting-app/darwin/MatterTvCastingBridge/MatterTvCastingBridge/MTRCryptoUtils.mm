/**
 *
 *    Copyright (c) 2020-2022 Project CHIP Authors
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

#import "MTRCryptoUtils.h"

#include <crypto/CHIPCryptoPAL.h>
#include <lib/support/logging/CHIPLogging.h>
#include <lib/support/Span.h>
#include <lib/core/CHIPError.h>

#include <Security/Security.h>

@implementation MTRCryptoUtils

+ (NSError *)ecdsaAsn1SignatureToRawWithFeLengthBytes:(NSUInteger)feLengthBytes asn1Signature:(CFDataRef)asn1Signature outRawSignature:(NSData **)outRawSignature
{
    // convert asn1Signature from CFDataRef to MutableByteSpan (asn1SignatureByteSpan)
    uint8_t mAsn1SignatureBytes[256];
    chip::MutableByteSpan asn1SignatureByteSpan = chip::MutableByteSpan(mAsn1SignatureBytes, sizeof(mAsn1SignatureBytes));
    size_t signatureLen = CFDataGetLength(asn1Signature);
    CFDataGetBytes(asn1Signature, CFRangeMake(0, signatureLen), asn1SignatureByteSpan.data());
    asn1SignatureByteSpan.reduce_size(signatureLen);
    
    uint8_t *outRawSignatureBytes = new uint8_t[(*outRawSignature).length];
    chip::MutableByteSpan outRawSignatureMutableByteSpan = chip::MutableByteSpan(outRawSignatureBytes, (*outRawSignature).length);
    
    CHIP_ERROR conversionError = chip::Crypto::EcdsaAsn1SignatureToRaw(
                                                                       feLengthBytes, chip::ByteSpan(asn1SignatureByteSpan.data(), asn1SignatureByteSpan.size()), outRawSignatureMutableByteSpan);
    if (CHIP_NO_ERROR != conversionError) {
        ChipLogError(AppServer,
            "DeviceAttestationCredentialsProviderImpl::SignWithDeviceAttestationKey failed to convert to raw signature.");
        
        //break;
    }
    *outRawSignature = [NSData dataWithBytes:outRawSignatureMutableByteSpan.data() length:outRawSignatureMutableByteSpan.size()];
    return nil;
}

@end
