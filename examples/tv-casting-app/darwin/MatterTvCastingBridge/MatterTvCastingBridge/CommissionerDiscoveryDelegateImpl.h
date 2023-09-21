/*
 *
 *    Copyright (c) 2022 Project CHIP Authors
 *    All rights reserved.
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

#ifndef CommissionerDiscoveryDelegateImpl_h
#define CommissionerDiscoveryDelegateImpl_h

#import "ConversionUtils.hpp"
#include <controller/DeviceDiscoveryDelegate.h>
#include <system/SystemClock.h>

class CommissionerDiscoveryDelegateImpl : public chip::Controller::DeviceDiscoveryDelegate {
public:
    void SetUp(dispatch_queue_t _Nonnull clientQueue,
        void (^_Nonnull objCDiscoveredCommissionerHandler)(DiscoveredNodeData * _Nonnull),
        TargetVideoPlayerInfo * _Nullable cachedTargetVideoPlayerInfos)
    {
        mClientQueue = clientQueue;
        mObjCDiscoveredCommissionerHandler = objCDiscoveredCommissionerHandler;
        mCachedTargetVideoPlayerInfos = cachedTargetVideoPlayerInfos;
        mDiscoveredCommissioners.clear();

        // cancel preexisting timer for ReportSleepingCommissioners, if any and schedule a new one if there are any cached
        // commissioners
        chip::DeviceLayer::SystemLayer().CancelTimer(ReportSleepingCommissioners, this);
        if (mCachedTargetVideoPlayerInfos != nullptr && mCachedTargetVideoPlayerInfos[0].IsInitialized()) {
            chip::DeviceLayer::SystemLayer().StartTimer(
                chip::System::Clock::Milliseconds32(CHIP_DEVICE_CONFIG_STR_DISCOVERY_DELAY_SEC * 1000), ReportSleepingCommissioners,
                this);
        }
    }

    void OnDiscoveredDevice(const chip::Dnssd::DiscoveredNodeData & nodeData)
    {
        ChipLogProgress(AppServer, "CommissionerDiscoveryDelegateImpl().OnDiscoveredDevice() called");
        __block const chip::Dnssd::DiscoveredNodeData cppNodeData = nodeData;
        dispatch_async(mClientQueue, ^{
            DiscoveredNodeData * objCDiscoveredNodeData = [ConversionUtils convertToObjCDiscoveredNodeDataFrom:&cppNodeData];
            mDiscoveredCommissioners.push_back(objCDiscoveredNodeData); // add to the list of discovered commissioners

            // set associated connectable video player from cache, if any
            if (mCachedTargetVideoPlayerInfos != nullptr) {
                for (size_t i = 0; i < kMaxCachedVideoPlayers && mCachedTargetVideoPlayerInfos[i].IsInitialized(); i++) {
                    if (mCachedTargetVideoPlayerInfos[i].IsSameAs(&cppNodeData)) {
                        mCachedTargetVideoPlayerInfos[i].SetLastDiscovered(
                            chip::System::SystemClock().GetMonotonicMilliseconds64()); // add discovery timestamp
                        CastingServer::GetInstance()->AddVideoPlayer(
                            &mCachedTargetVideoPlayerInfos[i]); // write updated video player to cache

                        VideoPlayer * connectableVideoPlayer =
                            [ConversionUtils convertToObjCVideoPlayerFrom:&mCachedTargetVideoPlayerInfos[i]];
                        [objCDiscoveredNodeData setConnectableVideoPlayer:connectableVideoPlayer];
                    }
                }
            }

            // make the callback
            mObjCDiscoveredCommissionerHandler(objCDiscoveredNodeData);
        });
    }

    static void ReportSleepingCommissioners(chip::System::Layer * _Nonnull aSystemLayer, void * _Nullable context)
    {
        ChipLogProgress(AppServer, "CommissionerDiscoveryDelegateImpl().ReportSleepingCommissioners() called");
        CommissionerDiscoveryDelegateImpl * thiz = (CommissionerDiscoveryDelegateImpl *) context;
        if (thiz != nullptr && thiz->mCachedTargetVideoPlayerInfos != nullptr) {
            for (size_t i = 0; i < kMaxCachedVideoPlayers && thiz->mCachedTargetVideoPlayerInfos[i].IsInitialized(); i++) {
                // check if there is a MACAddress to wake this Video Player up with
                if (thiz->mCachedTargetVideoPlayerInfos[i].GetMACAddress() != nullptr
                    && thiz->mCachedTargetVideoPlayerInfos[i].GetMACAddress()->size() > 0) {
                    bool discovered = false;
                    // check if it was already discovered
                    for (DiscoveredNodeData * discoveredCommissioner : thiz->mDiscoveredCommissioners) {
                        if (strcmp((char *) [discoveredCommissioner.hostName UTF8String],
                                thiz->mCachedTargetVideoPlayerInfos[i].GetHostName())
                            == 0) {
                            discovered = true;
                            break;
                        }
                    }

                    // surface the *sleeping* video player as a DiscoveredNodeData
                    if (!discovered) {
                        DiscoveredNodeData * objCDiscoveredNodeData =
                            [ConversionUtils convertToDiscoveredNodeDataFrom:&thiz->mCachedTargetVideoPlayerInfos[i]];
                        objCDiscoveredNodeData.getConnectableVideoPlayer.isAsleep = true;

                        // make the callback
                        ChipLogProgress(AppServer,
                            "CommissionerDiscoveryDelegateImpl().ReportSleepingCommissioners() reporting sleeping video player");
                        thiz->mObjCDiscoveredCommissionerHandler(objCDiscoveredNodeData);
                    }
                }
            }
        }
    }

private:
    void (^_Nonnull mObjCDiscoveredCommissionerHandler)(DiscoveredNodeData * _Nonnull);
    dispatch_queue_t _Nonnull mClientQueue;
    TargetVideoPlayerInfo * _Nullable mCachedTargetVideoPlayerInfos;
    std::vector<DiscoveredNodeData *> mDiscoveredCommissioners;
};

#endif /* CommissionerDiscoveryDelegateImpl_h */
