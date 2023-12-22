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

#import "MTRObserver.h"

#import <Foundation/Foundation.h>

#ifndef MTRAttribute_h
#define MTRAttribute_h

@interface MTRAttribute<__covariant ObjectType> : NSObject
@property (nonatomic, strong, readonly) ObjectType _Nullable value;

- (void)read:(void (^_Nonnull)(ObjectType _Nullable value, NSError * _Nullable error))completionBlock;
- (void)write:(ObjectType _Nonnull)value withCompletionBlock:(void (^_Nonnull)(NSError * _Nullable))completionBlock;

- (void)addObserver:(id<MTRObserver> _Nonnull)observer
    withMinInterval:(int)minInterval
        maxInterval:(int)maxInterval
    completionBlock:(void (^_Nonnull)(NSError * _Nullable))completionBlock;

- (void)removeObserver:(id<MTRObserver> _Nonnull)observer;

- (bool)isAvailable;

@end

#endif /* MTRAttribute_h */
