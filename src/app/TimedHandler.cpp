/*
 *
 *    Copyright (c) 2020 Project CHIP Authors
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

#include "TimedHandler.h"
#include <app/InteractionModelEngine.h>
#include <app/MessageDef/TimedRequestMessage.h>
#include <app/StatusResponse.h>
#include <lib/core/CHIPTLV.h>
#include <lib/support/logging/CHIPLogging.h>
#include <system/SystemClock.h>
#include <system/TLVPacketBufferBackingStore.h>

namespace chip {
namespace app {

CHIP_ERROR TimedHandler::OnMessageReceived(Messaging::ExchangeContext * aExchangeContext, const PayloadHeader & aPayloadHeader,
                                           System::PacketBufferHandle && aPayload)
{
    using namespace Protocols::InteractionModel;

    if (aExchangeContext->IsGroupExchangeContext())
    {
        // Timed interactions are always supposed to be unicast.  Nothing else
        // to do here; exchange will close and we'll free ourselves.
        ChipLogError(DataManagement, "Dropping Timed Request on group exchange " ChipLogFormatExchange,
                     ChipLogValueExchange(aExchangeContext));
        return CHIP_NO_ERROR;
    }

    if (mState == State::kExpectingTimedAction)
    {
        // We were just created; our caller should have done this only if it's
        // dealing with a Timed Request message.
        VerifyOrDie(aPayloadHeader.HasMessageType(MsgType::TimedRequest));
        mState         = State::kReceivedTimedAction;
        CHIP_ERROR err = HandleTimedRequestAction(aExchangeContext, aPayloadHeader, std::move(aPayload));
        if (err != CHIP_NO_ERROR)
        {
            ChipLogError(DataManagement, "Failed to parse Timed Request action: handler %p exchange " ChipLogFormatExchange, this,
                         ChipLogValueExchange(aExchangeContext));
            StatusResponse::Send(Status::InvalidAction, aExchangeContext, /* aExpectResponse = */ false);
        }
        return err;
    }

    if (mState == State::kExpectingFollowingAction)
    {
        if (System::SystemClock().GetMonotonicTimestamp() > mTimeLimit)
        {
            // Time is up.  Spec says to send UNSUPPORTED_ACCESS.
            ChipLogError(DataManagement, "Timeout expired: handler %p exchange " ChipLogFormatExchange, this,
                         ChipLogValueExchange(aExchangeContext));
            return StatusResponse::Send(Status::UnsupportedAccess, aExchangeContext, /* aExpectResponse = */ false);
        }

        if (aPayloadHeader.HasMessageType(MsgType::InvokeCommandRequest))
        {
            auto * imEngine = InteractionModelEngine::GetInstance();
            aExchangeContext->SetDelegate(imEngine);
            ChipLogDetail(DataManagement, "Handing timed invoke to IM engine: handler %p exchange " ChipLogFormatExchange, this,
                          ChipLogValueExchange(aExchangeContext));
            imEngine->OnTimedInvoke(this, aExchangeContext, aPayloadHeader, std::move(aPayload));
            return CHIP_NO_ERROR;
        }

        // TODO: Add handling of MsgType::WriteRequest here.
    }

    // Not an expected message.  Send an error response.  The exchange will
    // close when we return.
    ChipLogError(DataManagement, "Unexpected unknown message in tiemd interaction: handler %p exchange " ChipLogFormatExchange,
                 this, ChipLogValueExchange(aExchangeContext));

    return StatusResponse::Send(Status::InvalidAction, aExchangeContext, /* aExpectResponse = */ false);
}

void TimedHandler::OnExchangeClosing(Messaging::ExchangeContext *)
{
    InteractionModelEngine::GetInstance()->OnTimedInteractionFailed(this);
}

CHIP_ERROR TimedHandler::HandleTimedRequestAction(Messaging::ExchangeContext * aExchangeContext,
                                                  const PayloadHeader & aPayloadHeader, System::PacketBufferHandle && aPayload)
{
    using namespace Protocols::InteractionModel;

    System::PacketBufferTLVReader reader;
    reader.Init(std::move(aPayload));

    reader.Next();

    TimedRequestMessage::Parser parser;
    ReturnErrorOnFailure(parser.Init(reader));

#if CHIP_CONFIG_IM_ENABLE_SCHEMA_CHECK
    ReturnErrorOnFailure(parser.CheckSchemaValidity());
#endif

    uint16_t timeoutMs;
    ReturnErrorOnFailure(parser.GetTimeoutMs(&timeoutMs));

    ChipLogDetail(DataManagement, "Got Timed Request with timeout %" PRIu16 ": handler %p exchange " ChipLogFormatExchange,
                  timeoutMs, this, ChipLogValueExchange(aExchangeContext));
    // Tell the exchange to close after the timeout passes, so we don't get
    // stuck waiting forever if the client never sends another message.
    auto delay = System::Clock::Milliseconds32(timeoutMs);
    aExchangeContext->SetResponseTimeout(delay);
    ReturnErrorOnFailure(StatusResponse::Send(Status::Success, aExchangeContext, /* aExpectResponse = */ true));

    // Now just wait for the client.
    mState     = State::kExpectingFollowingAction;
    mTimeLimit = System::SystemClock().GetMonotonicTimestamp() + delay;
    return CHIP_NO_ERROR;
}

} // namespace app
} // namespace chip
