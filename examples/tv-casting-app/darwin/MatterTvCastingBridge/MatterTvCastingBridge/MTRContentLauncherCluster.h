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

#import "MTRCluster.h"
#import "MTREndpoint.h"
#import "MTRLaunchResponse.h"

#ifndef MTRContentLauncherCluster_h
#define MTRContentLauncherCluster_h

@interface MTRContentLauncherCluster : NSObject <MTRCluster>
@property (nonatomic, weak, readonly) MTREndpoint * _Nullable endpoint;

- (BOOL)canLaunchURL;

- (BOOL)canLaunchCommand;

- (void)launchURL:(NSURL * _Nonnull)url
            options:(NSDictionary * _Nullable)options
    completionBlock:(void (^_Nonnull)(MTRLaunchResponse * _Nullable response, NSError * _Nullable error))completionBlock;

// launchCommand

@end

#endif /* MTRContentLauncherCluster_h */
