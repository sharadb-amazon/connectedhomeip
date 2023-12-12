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

#include "clusters/ClusterTemplates.h"
#include "core/Endpoint.h"
#include "core/Types.h"

#include "lib/support/logging/CHIPLogging.h"

namespace matter {
namespace casting {
namespace clusters {
namespace content_launcher {

class ContentLauncherCluster : public core::BaseCluster
{
public:
    ContentLauncherCluster(memory::Weak<core::Endpoint> endpoint) : core::BaseCluster(endpoint) {}

    void LaunchURL(chip::app::Clusters::ContentLauncher::Commands::LaunchURL::Type request, void * context,
                   SuccessCallbackType<chip::app::Clusters::ContentLauncher::Commands::LaunchURL::Type::ResponseType> successCb,
                   FailureCallbackType failureCb, const chip::Optional<uint16_t> & timedInvokeTimeoutMs);
};

struct LaunchURLContext : public CommandContext<chip::app::Clusters::ContentLauncher::Commands::LaunchURL::Type>
{
    LaunchURLContext(memory::Strong<core::Endpoint> endpoint,
                     chip::app::Clusters::ContentLauncher::Commands::LaunchURL::Type request, void * context,
                     SuccessCallbackType<chip::app::Clusters::ContentLauncher::Commands::LaunchURL::Type::ResponseType> successCb,
                     FailureCallbackType failureCb, const chip::Optional<uint16_t> & timedInvokeTimeoutMs) :
        CommandContext(endpoint, request, context, successCb, failureCb, timedInvokeTimeoutMs)
    {}
};

class LaunchURLCommand : public Command<chip::app::Clusters::ContentLauncher::Commands::LaunchURL::Type, LaunchURLContext>
{
public:
    LaunchURLCommand(memory::Weak<core::Endpoint> endpoint) : Command(endpoint) {}
};

}; // namespace content_launcher
}; // namespace clusters
}; // namespace casting
}; // namespace matter
