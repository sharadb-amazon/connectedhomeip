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

#import "MTRAttribute.h"
#import "MTRCluster.h"
#import "MTRCurrentState.h"
#import "MTREndpoint.h"

#ifndef MTRMediaPlaybackCluster_h
#define MTRMediaPlaybackCluster_h

@interface MTRMediaPlaybackCluster : NSObject <MTRCluster>
@property (nonatomic, weak, readonly) MTREndpoint * _Nullable endpoint;

@property (nonatomic, strong, readonly) MTRAttribute<MTRCurrentState *> * _Nonnull currentState; // Nonnull for required attr
@property (nonatomic, strong, readonly) MTRAttribute<MTRCurrentState *> * _Nullable optionalAttribute; // Nullable for optional attr

// TODO: can support commands

// TODO: commands

@end

#endif /* MTRMediaPlaybackCluster_h */
