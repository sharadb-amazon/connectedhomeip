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

#ifndef MTRCastingApp_h
#define MTRCastingApp_h

@interface MTRCastingApp : NSObject

+(void)initializeWithAppParameters:(MTRAppParameters* _Nonnull)appParameters
                        dataSource:(id<MTRDataSource> _Nonnull)datasource;

+(void)start;

+(void)stop;

@end

#endif /* MTRCastingApp_h */
