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

#pragma once

#include <lib/core/CHIPError.h>

#include <map>
#include <string>

class FeatureOverrides
{
public:
    FeatureOverrides(FeatureOverrides & other) = delete;
    void operator=(const FeatureOverrides &) = delete;
    static FeatureOverrides * GetInstance();
    void Init(std::map<std::string, int> overrides);
    bool IsDefined(std::string key);
    CHIP_ERROR Value(std::string key, int &outValue);

private:
    static FeatureOverrides * featureOverrides_;
    FeatureOverrides();
    std::map<std::string, int> mOverrides;
};
