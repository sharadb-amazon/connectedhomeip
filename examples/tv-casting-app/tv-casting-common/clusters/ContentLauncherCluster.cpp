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

#include "ContentLauncherCluster.h"

namespace matter {
namespace casting {
namespace clusters {
namespace content_launcher {

void ContentLauncherCluster::LaunchURL(
    chip::app::Clusters::ContentLauncher::Commands::LaunchURL::Type request, void * context,
    SuccessCallbackType<chip::app::Clusters::ContentLauncher::Commands::LaunchURL::Type::ResponseType> successCb,
    FailureCallbackType failureCb, const chip::Optional<uint16_t> & timedInvokeTimeoutMs)
{
    LaunchURLCommand command(this->GetEndpoint());
    command.Invoke(request, context, successCb, failureCb, timedInvokeTimeoutMs);
}

}; // namespace content_launcher
}; // namespace clusters
}; // namespace casting
}; // namespace matter
