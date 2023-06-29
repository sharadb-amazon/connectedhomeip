/*
 *
 *    Copyright (c) 2020-23 Project CHIP Authors
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

#include "FeatureOverrides.h"

#include <lib/support/CodeUtils.h>

FeatureOverrides * FeatureOverrides::featureOverrides_ = nullptr;

FeatureOverrides::FeatureOverrides() {}

FeatureOverrides * FeatureOverrides::GetInstance()
{
    if (featureOverrides_ == nullptr)
    {
        featureOverrides_ = new FeatureOverrides();
    }
    return featureOverrides_;
}

void FeatureOverrides::Init(std::map<std::string, int> overrides)
{
    mOverrides = overrides;
}

bool FeatureOverrides::IsDefined(std::string key)
{
    return mOverrides.find(key) != mOverrides.end();
}

CHIP_ERROR FeatureOverrides::Value(std::string key, int &outValue)
{
    VerifyOrReturnError(mOverrides.find(key) != mOverrides.end(), CHIP_ERROR_NOT_FOUND);
    outValue = mOverrides[key];
    return CHIP_NO_ERROR;
}
