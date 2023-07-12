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

#import "MatterError.h"
#import <Foundation/Foundation.h>

#ifndef CommissioningCallbacksHandlers_h
#define CommissioningCallbacksHandlers_h

@interface CommissioningCallbackHandlers : NSObject

- (CommissioningCallbackHandlers * _Nonnull)
    initWithCommissioningWindowRequestedHandler:(void (^_Nonnull)(MatterError * _Nonnull))commissioningWindowRequestedHandler
              commissioningWindowOpenedCallback:(void (^_Nonnull)(MatterError * _Nonnull))commissioningWindowOpenedCallback
                  commissioningCompleteCallback:(void (^_Nonnull)(MatterError * _Nonnull))commissioningCompleteCallback
            sessionEstablishmentStartedCallback:(void (^_Nullable)(void))sessionEstablishmentStartedCallback
                     sessionEstablishedCallback:(void (^_Nullable)(void))sessionEstablishedCallback
              sessionEstablishmentErrorCallback:(void (^_Nullable)(MatterError * _Nonnull))sessionEstablishmentErrorCallback
            sessionEstablishmentStoppedCallback:(void (^_Nullable)(void))sessionEstablishmentStoppedCallback;

@property void (^_Nullable commissioningWindowRequestedHandler)(MatterError * _Nonnull);

@property void (^_Nullable commissioningWindowOpenedCallback)(MatterError * _Nonnull);

@property void (^_Nullable commissioningCompleteCallback)(MatterError * _Nonnull);

@property void (^_Nullable sessionEstablishmentStartedCallback)(void);

@property void (^_Nullable sessionEstablishedCallback)(void);

@property void (^_Nullable sessionEstablishmentErrorCallback)(MatterError * _Nonnull);

@property void (^_Nullable sessionEstablishmentStoppedCallback)(void);

@end

#endif /* CommissioningCallbacksHandlers_h */
