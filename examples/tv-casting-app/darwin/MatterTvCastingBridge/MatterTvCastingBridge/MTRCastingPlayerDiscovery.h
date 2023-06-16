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

#ifndef MTRCastingPlayerDiscovery_h
#define MTRCastingPlayerDiscovery_h

/**
 NotificationCenter.default.addObserver(self, selector: #selector(self.methodOfReceivedNotification(notification:)), name: Notification.Name("NotificationIdentifier"), object: nil)
 ^ This is how classes will listen to the event in swift
 @objc func methodOfReceivedNotification(notification: Notification) {}
 */
NSString * _Nonnull const MTRCastingPlayerDiscoveryPlayersDidUpdateNotification = @"MTRCastingPlayerDiscoveryPlayersDidUpdateNotification";

@interface MTRCastingPlayerDiscovery : NSObject
+ (MTRCastingPlayerDiscovery * _Nonnull)sharedInstance;

- (nonnull instancetype)init UNAVAILABLE_ATTRIBUTE;
+ (nonnull instancetype)new UNAVAILABLE_ATTRIBUTE;

@property (nonatomic, strong) NSArray<MTRCastingPlayer * > * _Nonnull castingPlayers;

- (void)startDiscovery;

- (void)stopDiscovery;

@end

#endif /* MTRCastingPlayerDiscovery_h */
