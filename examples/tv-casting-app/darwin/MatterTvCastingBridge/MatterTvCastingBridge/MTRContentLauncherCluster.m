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

#import "MTRContentLauncherCluster.h"
#import <Foundation/Foundation.h>

@implementation MTRContentLauncherCluster

- (BOOL)canLaunchCommand
{
    // TODO: replace stub implementation
    return true;
}

- (BOOL)canLaunchURL
{
    // TODO: replace stub implementation
    return true;
}

- (void)launchURL:(NSURL * _Nonnull)url
            options:(NSDictionary * _Nullable)options
    completionBlock:(void (^_Nonnull __strong)(MTRLaunchResponse * _Nullable __strong, NSError * _Nullable __strong))completionBlock
{
    // TODO: replace stub implementation
}

@end
