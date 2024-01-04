/**
 *
 *    Copyright (c) 2020-2023 Project CHIP Authors
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

#import "MTREndpoint.h"
#import "MTREndpointFilter.h"

#import <Foundation/Foundation.h>

#ifndef MTRCastingPlayer_h
#define MTRCastingPlayer_h

@class MTREndpoint;

@interface MTRCastingPlayer : NSObject

/**
 
minimum matter::casting::core::kCommissioningWindowTimeoutSec
 */
- (void)verifyOrEstablishConnectionWithCompletionBlock:(void (^_Nonnull)(NSError * _Nullable))completion timeout:(long long)timeout desiredEndpointFilter:(MTREndpointFilter * _Nullable)desiredEndpointFilter;

- (void)verifyOrEstablishConnectionWithCompletionBlock:(void (^_Nonnull)(NSError * _Nullable))completion desiredEndpointFilter:(MTREndpointFilter * _Nullable)desiredEndpointFilter;

-(void)disconnect;

- (NSString * _Nonnull)identifier;
- (NSString * _Nonnull)deviceName;
- (uint16_t)vendorId;
- (uint16_t)productId;
- (uint32_t)deviceType;
- (NSArray * _Nonnull)ipAddresses;
- (NSArray<MTREndpoint *> * _Nonnull)endpoints;

- (nonnull instancetype)init UNAVAILABLE_ATTRIBUTE;
+ (nonnull instancetype)new UNAVAILABLE_ATTRIBUTE;

@end

#endif /* MTRCastingPlayer_h */
