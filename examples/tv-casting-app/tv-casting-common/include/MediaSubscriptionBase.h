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
class MediaSubscriptionBase : public MediaBase
{
public:
    MediaSubscriptionBase(chip::ClusterId clusterId) : MediaBase(clusterId) {}

    CHIP_ERROR SubscribeAttribute(void * context,
                                  chip::Controller::ReadResponseSuccessCallback<typename TypeInfo::DecodableArgType> successFn,
                                  chip::Controller::ReadResponseFailureCallback failureFn, uint16_t minInterval,
                                  uint16_t maxInterval, chip::Controller::SubscriptionEstablishedCallback onSubscriptionEstablished)
    {
        VerifyOrDieWithMsg(mTargetVideoPlayerInfo != nullptr, AppServer, "Target unknown");

        mSubscriptionContext       = context;
        mSuccessFn                 = successFn;
        mFailureFn                 = failureFn;
        mMinInterval               = minInterval;
        mMaxInterval               = maxInterval;
        mOnSubscriptionEstablished = onSubscriptionEstablished;
        return mTargetVideoPlayerInfo->FindOrEstablishCASESession(this, OnConnectionSendSubscriptionRequest, OnConnectionFailure);
    }

    static void OnConnectionSendSubscriptionRequest(void * context, chip::Messaging::ExchangeManager & exchangeMgr, chip::SessionHandle & sessionHandle)
    {
        ChipLogProgress(AppServer, "MediaSubscriptionBase:OnConnectionSendSubscriptionRequest called");
        MediaSubscriptionBase * _this = static_cast<MediaSubscriptionBase *>(context);
        if (!sessionHandle->IsSecureSession())
        {
            _this->mFailureFn(_this->mSubscriptionContext, CHIP_ERROR_MISSING_SECURE_SESSION);
            return;
        }

        if (sessionHandle->AsSecureSession()->IsDefunct())
        {
            _this->mFailureFn(_this->mSubscriptionContext, CHIP_ERROR_CONNECTION_CLOSED_UNEXPECTEDLY);
            return;
        }

        MediaClusterBase cluster(exchangeMgr, sessionHandle, _this->mClusterId,
                                 _this->mTvEndpoint);
        CHIP_ERROR err = cluster.template SubscribeAttribute<TypeInfo>(_this->mSubscriptionContext, _this->mSuccessFn,
                                                                       _this->mFailureFn, _this->mMinInterval, _this->mMaxInterval,
                                                                       _this->mOnSubscriptionEstablished);
        if (err != CHIP_NO_ERROR)
        {
            _this->mFailureFn(_this->mSubscriptionContext, err);
        }
    }

    static void OnConnectionFailure(void *context, CHIP_ERROR err)
    {
        MediaSubscriptionBase * _this = static_cast<MediaSubscriptionBase *>(context);
        ChipLogError(AppServer, "MediaSubscriptionBase:OnConnectionFailure error %" CHIP_ERROR_FORMAT, err.Format());
        _this->mFailureFn(_this->mSubscriptionContext, err);
    }

private:
    void * mSubscriptionContext;
    chip::Controller::ReadResponseSuccessCallback<typename TypeInfo::DecodableArgType> mSuccessFn;
    chip::Controller::ReadResponseFailureCallback mFailureFn;
    uint16_t mMinInterval;
    uint16_t mMaxInterval;
    chip::Controller::SubscriptionEstablishedCallback mOnSubscriptionEstablished;
};
