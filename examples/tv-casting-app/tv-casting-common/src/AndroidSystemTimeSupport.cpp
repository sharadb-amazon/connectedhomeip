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

#include "AndroidSystemTimeSupport.h"

CHIP_ERROR AndroidClockImpl::GetClock_RealTime(chip::System::Clock::Microseconds64 & aCurTime)
{
    ChipLogProgress(AppServer, "AndroidClockImpl::GetClock_RealTime called");
    struct timeval tv;
    if (gettimeofday(&tv, nullptr) != 0)
    {
        return CHIP_ERROR_POSIX(errno);
    }
    if (tv.tv_sec < CHIP_SYSTEM_CONFIG_VALID_REAL_TIME_THRESHOLD)
    {
        return CHIP_ERROR_REAL_TIME_NOT_SYNCED;
    }
    if (tv.tv_usec < 0)
    {
        return CHIP_ERROR_REAL_TIME_NOT_SYNCED;
    }
    static_assert(CHIP_SYSTEM_CONFIG_VALID_REAL_TIME_THRESHOLD >= 0, "We might be letting through negative tv_sec values!");
    aCurTime = chip::System::Clock::Microseconds64((static_cast<uint64_t>(tv.tv_sec) * UINT64_C(1000000)) +
                                                   static_cast<uint64_t>(tv.tv_usec));
    return CHIP_NO_ERROR;
}

CHIP_ERROR AndroidClockImpl::GetClock_RealTimeMS(chip::System::Clock::Milliseconds64 & aCurTime)
{
    ChipLogProgress(AppServer, "AndroidClockImpl::GetClock_RealTimeMS called");
    chip::System::Clock::Microseconds64 curTimeUs;
    auto err = GetClock_RealTime(curTimeUs);
    aCurTime = std::chrono::duration_cast<chip::System::Clock::Milliseconds64>(curTimeUs);
    return err;
}
