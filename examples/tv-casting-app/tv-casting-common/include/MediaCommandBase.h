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
#pragma once

#include "MediaBase.h"

#include <functional>

template <typename RequestType, typename ResponseType>
class MediaCommandBase : public MediaBase
{
public:
    MediaCommandBase(chip::ClusterId clusterId) : MediaBase(clusterId) {}

    CHIP_ERROR Invoke(RequestType request, std::function<void(CHIP_ERROR)> responseCallback)
    {
        ReturnErrorCodeIf(mTargetVideoPlayerInfo == nullptr, CHIP_ERROR_PEER_NODE_NOT_FOUND);
        mRequest          = request;
        sResponseCallback = responseCallback;
        return mTargetVideoPlayerInfo->FindOrEstablishCASESession(this, OnConnectionSuccess, OnConnectionFailure);
    }

    static void OnConnectionSuccess(TargetVideoPlayerInfo * connectedVideoPlayer, void * context)
    {
        auto deviceProxy = connectedVideoPlayer->GetOperationalDeviceProxy();

        if (deviceProxy == nullptr || !deviceProxy->ConnectionReady())
        {
            sResponseCallback(CHIP_ERROR_PEER_NODE_NOT_FOUND);
            return;
        }

        if (!deviceProxy->GetSecureSession().HasValue())
        {
            sResponseCallback(CHIP_ERROR_MISSING_SECURE_SESSION);
            return;
        }

        const chip::SessionHandle & sessionHandle = deviceProxy->GetSecureSession().Value();
        if (!sessionHandle->IsSecureSession())
        {
            sResponseCallback(CHIP_ERROR_MISSING_SECURE_SESSION);
            return;
        }

        if (sessionHandle->AsSecureSession()->IsDefunct())
        {
            sResponseCallback(CHIP_ERROR_CONNECTION_CLOSED_UNEXPECTEDLY);
            return;
        }

        MediaCommandBase * _this = static_cast<MediaCommandBase *>(context);
        MediaClusterBase cluster(*deviceProxy->GetExchangeManager(), deviceProxy->GetSecureSession().Value(), _this->mClusterId,
                                 _this->mTvEndpoint);
        sResponseCallback(cluster.InvokeCommand(_this->mRequest, nullptr, OnSuccess, OnFailure));
    }

    static void OnConnectionFailure(CHIP_ERROR err) { sResponseCallback(err); }

    static void OnSuccess(void * context, const ResponseType & response) { sResponseCallback(CHIP_NO_ERROR); }

    static void OnFailure(void * context, CHIP_ERROR error) { sResponseCallback(error); }

protected:
    static std::function<void(CHIP_ERROR)> sResponseCallback;
    RequestType mRequest;
};

template <typename RequestType, typename ResponseType>
std::function<void(CHIP_ERROR)> MediaCommandBase<RequestType, ResponseType>::sResponseCallback = {};
