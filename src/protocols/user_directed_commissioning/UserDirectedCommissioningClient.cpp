/*
 *
 *    Copyright (c) 2021 Project CHIP Authors
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

/**
 *    @file
 *      This file implements an object for a Matter User Directed Commissioning unsolicited
 *      initiator (client).
 *
 */

#include "UserDirectedCommissioning.h"

#include <platform/CHIPDeviceLayer.h>

#include <unistd.h>

namespace chip {
namespace Protocols {
namespace UserDirectedCommissioning {

CHIP_ERROR UserDirectedCommissioningClient::SendUDCMessage(TransportMgrBase * transportMgr, System::PacketBufferHandle && payload,
                                                           chip::Transport::PeerAddress peerAddress)
{
    CHIP_ERROR err = EncodeUDCMessage(payload);
    if (err != CHIP_NO_ERROR)
    {
        return err;
    }
    ChipLogProgress(Inet, "Sending UDC msg");

    mUdcAttemptsRemaining = 5; // send UDC message 5 times per spec (no ACK on this message)
    mTransportMgr         = transportMgr;
    mTargetPayload        = payload.CloneData();
    mTargetPeerAddress    = peerAddress;
    DeviceLayer::SystemLayer().StartTimer(System::Clock::Milliseconds32(0), SendUDCTask, this);

    return err;
}

void UserDirectedCommissioningClient::SendUDCTask(System::Layer * aSystemLayer, void * aAppState)
{
    UserDirectedCommissioningClient * thiz = reinterpret_cast<UserDirectedCommissioningClient *>(aAppState);

    // Schedule next UDC msg, if any, 100ms later
    thiz->mUdcAttemptsRemaining--;
    if (thiz->mUdcAttemptsRemaining > 0)
    {
        DeviceLayer::SystemLayer().StartTimer(System::Clock::Milliseconds32(100), SendUDCTask, thiz);
    }

    // send UDC message
    CHIP_ERROR err = thiz->mTransportMgr->SendMessage(thiz->mTargetPeerAddress, thiz->mTargetPayload.CloneData());
    if (err != CHIP_NO_ERROR)
    {
        ChipLogError(Inet, "UDC SendMessage failed: %" CHIP_ERROR_FORMAT, err.Format());
    }
}

CHIP_ERROR UserDirectedCommissioningClient::EncodeUDCMessage(const System::PacketBufferHandle & payload)
{
    PayloadHeader payloadHeader;
    PacketHeader packetHeader;

    payloadHeader.SetMessageType(MsgType::IdentificationDeclaration).SetInitiator(true).SetNeedsAck(false);

    VerifyOrReturnError(!payload.IsNull(), CHIP_ERROR_INVALID_ARGUMENT);
    VerifyOrReturnError(!payload->HasChainedBuffer(), CHIP_ERROR_INVALID_MESSAGE_LENGTH);
    VerifyOrReturnError(payload->TotalLength() <= kMaxAppMessageLen, CHIP_ERROR_MESSAGE_TOO_LONG);

    ReturnErrorOnFailure(payloadHeader.EncodeBeforeData(payload));

    ReturnErrorOnFailure(packetHeader.EncodeBeforeData(payload));

    return CHIP_NO_ERROR;
}

} // namespace UserDirectedCommissioning
} // namespace Protocols
} // namespace chip
