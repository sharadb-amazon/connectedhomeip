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

#import "MTRCastingPlayer.h"
#import "MTRCluster.h"
#import "MTREndpointClusterType.h"

#import <Foundation/Foundation.h>

#ifndef MTREndpoint_h
#define MTREndpoint_h

@protocol MTRCluster;
@class MTRCastingPlayer;

@interface MTREndpoint : NSObject
@property (nonatomic, weak, readonly) MTRCastingPlayer * _Nullable castingPlayer;
@property (nonatomic, strong, readonly) NSString * _Nonnull identifier; // endpointid
@property (nonatomic, strong, readonly) NSString * _Nonnull vendorId;
@property (nonatomic, strong, readonly) NSString * _Nonnull productId;
@property (nonatomic, strong, readonly) NSString * _Nonnull type;

- (nonnull instancetype)init UNAVAILABLE_ATTRIBUTE;
+ (nonnull instancetype)new UNAVAILABLE_ATTRIBUTE;

- (BOOL)hasCluster:(MTREndpointClusterType)type;

/**
  @param type The type of cluster. Please refer to MTREndpointClusterType.
 */
- (id<MTRCluster> _Nullable)clusterForType:(MTREndpointClusterType)type;

@end

#endif /* MTREndpoint_h */
