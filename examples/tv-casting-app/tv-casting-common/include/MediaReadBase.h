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

template <typename TypeInfo>
class MediaReadBase : public MediaBase
{
public:
    MediaReadBase(chip::ClusterId clusterId) : MediaBase(clusterId) {}

    CHIP_ERROR ReadAttribute(void * context,
                             chip::Controller::ReadResponseSuccessCallback<typename TypeInfo::DecodableArgType> successFn,
                             chip::Controller::ReadResponseFailureCallback failureFn)
    {
        VerifyOrDieWithMsg(mTargetVideoPlayerInfo != nullptr, AppServer, "Target unknown");

        mReadContext       = context;
        mSuccessFn                 = successFn;
        mFailureFn                 = failureFn;
        return mTargetVideoPlayerInfo->FindOrEstablishCASESession(this, OnConnectionSuccess, OnConnectionFailure);
        
        VerifyOrDieWithMsg(mTargetVideoPlayerInfo != nullptr, AppServer, "Target unknown");
    }

    static void OnConnectionSuccess(chip::Messaging::ExchangeManager & exchangeMgr, chip::SessionHandle & sessionHandle, void * context)
    {
        MediaReadBase * _this = static_cast<MediaReadBase *>(context);
        if (!sessionHandle->IsSecureSession())
        {
            _this->mFailureFn(_this->mReadContext, CHIP_ERROR_MISSING_SECURE_SESSION);
            return;
        }

        if (sessionHandle->AsSecureSession()->IsDefunct())
        {
            _this->mFailureFn(_this->mReadContext, CHIP_ERROR_CONNECTION_CLOSED_UNEXPECTEDLY);
            return;
        }

        MediaClusterBase cluster(exchangeMgr, sessionHandle, _this->mClusterId,
                                 _this->mTvEndpoint);
        CHIP_ERROR err = cluster.template ReadAttribute<TypeInfo>(_this->mReadContext, _this->mSuccessFn, _this->mFailureFn);
        if (err != CHIP_NO_ERROR)
        {
            _this->mFailureFn(_this->mReadContext, err);
        }
    }

    static void OnConnectionFailure(CHIP_ERROR err)
    {
        // TODO: _this->mFailureFn(_this->mReadContext, err);
        ChipLogError(AppServer, "MediaReadBase:OnConnectionFailure - FindOrEstablishSession failed %" CHIP_ERROR_FORMAT,
                     err.Format());
    }

private:
    void * mReadContext;
    chip::Controller::ReadResponseSuccessCallback<typename TypeInfo::DecodableArgType> mSuccessFn;
    chip::Controller::ReadResponseFailureCallback mFailureFn;
};
