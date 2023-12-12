/*
 *
 *    Copyright (c) 2023 Project CHIP Authors
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

#include "core/Endpoint.h"
#include "core/Types.h"

#include "lib/support/logging/CHIPLogging.h"

namespace matter {
namespace casting {
namespace clusters {

template <typename ResponseType>
using SuccessCallbackType = std::function<void(void * context, const ResponseType & responseObject)>;
using FailureCallbackType = std::function<void(void * context, CHIP_ERROR err)>;

template <typename T>
struct CommandContext
{
    CommandContext(memory::Strong<core::Endpoint> endpoint, T request, void * context,
                   SuccessCallbackType<typename T::ResponseType> successCb, FailureCallbackType failureCb,
                   const chip::Optional<uint16_t> & timedInvokeTimeoutMs) :
        mSuccessCb(successCb),
        mFailureCb(failureCb)
    {
        mEndpoint             = endpoint;
        mRequest              = request;
        mClientContext        = context;
        mTimedInvokeTimeoutMs = timedInvokeTimeoutMs;
    }

    memory::Strong<core::Endpoint> mEndpoint;
    T mRequest;
    void * mClientContext;
    SuccessCallbackType<typename T::ResponseType> mSuccessCb;
    FailureCallbackType mFailureCb;
    chip::Optional<uint16_t> mTimedInvokeTimeoutMs;
};

template <typename T, typename C>
class Command
{
public:
    Command(memory::Weak<core::Endpoint> endpoint) { this->mEndpoint = endpoint; }

    void Invoke(T request, void * context, SuccessCallbackType<typename T::ResponseType> successCb, FailureCallbackType failureCb,
                const chip::Optional<uint16_t> & timedInvokeTimeoutMs)
    {
        memory::Strong<core::Endpoint> endpoint = this->GetEndpoint().lock();
        if (endpoint)
        {
            C * commandContext = new C(endpoint, request, context, successCb, failureCb, timedInvokeTimeoutMs);

            endpoint->GetCastingPlayer()->FindOrEstablishSession(
                commandContext,
                // FindOrEstablishSession success handler
                [](void * context, chip::Messaging::ExchangeManager & exchangeMgr, const chip::SessionHandle & sessionHandle) {
                    C * _commandContext = static_cast<C *>(context);
                    ChipLogProgress(AppServer, "<Command>::Invoke() Found or established session");

                    // Invoke command
                    support::MediaClusterBase cluster(exchangeMgr, sessionHandle, _commandContext->mEndpoint->GetId());
                    CHIP_ERROR err = cluster.template InvokeCommand(
                        _commandContext->mRequest, _commandContext,
                        // Command success handler
                        [](void * context,
                           const chip::app::Clusters::ContentLauncher::Commands::LauncherResponse::DecodableType & response) {
                            C * _commandContext = static_cast<C *>(context);
                            ChipLogProgress(AppServer, "<Command>::Invoke() response success");
                            _commandContext->mSuccessCb(_commandContext->mClientContext, response);
                            delete _commandContext;
                        },
                        // Command failure handler
                        [](void * context, CHIP_ERROR error) {
                            C * _commandContext = static_cast<C *>(context);
                            ChipLogError(AppServer,
                                         "<Command>::Invoke() failure response on EndpointId: %d with error: "
                                         "%" CHIP_ERROR_FORMAT,
                                         _commandContext->mEndpoint->GetId(), error.Format());
                            _commandContext->mFailureCb(_commandContext->mClientContext, error);
                            delete _commandContext;
                        },
                        _commandContext->mTimedInvokeTimeoutMs);

                    // error in invoking the command
                    if (err != CHIP_NO_ERROR)
                    {
                        ChipLogError(AppServer,
                                     "<Command>::Invoke() failure in invoking command on EndpointId: %d with error: "
                                     "%" CHIP_ERROR_FORMAT,
                                     _commandContext->mEndpoint->GetId(), err.Format());
                        _commandContext->mFailureCb(_commandContext->mClientContext, err);
                        delete _commandContext;
                    }
                },
                // FindOrEstablishSession failure handler
                [](void * context, const chip::ScopedNodeId & peerId, CHIP_ERROR error) {
                    C * _commandContext = static_cast<C *>(context);
                    ChipLogError(AppServer,
                                 "<Command>::Invoke() failure in retrieving session info for peerId.nodeId: "
                                 "0x" ChipLogFormatX64 ", peer.fabricIndex: %d with error: %" CHIP_ERROR_FORMAT,
                                 ChipLogValueX64(peerId.GetNodeId()), peerId.GetFabricIndex(), error.Format());
                    _commandContext->mFailureCb(_commandContext->mClientContext, error);
                    delete _commandContext;
                });
        }
        else
        {
            ChipLogError(AppServer, "<Command>::Invoke() failure in retrieving Endpoint");
            failureCb(context, CHIP_ERROR_INCORRECT_STATE);
        }
    }

protected:
    memory::Weak<core::Endpoint> GetEndpoint() const { return mEndpoint.lock(); }
    memory::Weak<core::Endpoint> mEndpoint;
};

}; // namespace clusters
}; // namespace casting
}; // namespace matter
