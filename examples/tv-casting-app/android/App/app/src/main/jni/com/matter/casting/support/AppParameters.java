/*
 *   Copyright (c) 2023 Project CHIP Authors
 *   All rights reserved.
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 */

package com.matter.casting.support;

import android.content.Context;

import androidx.annotation.NonNull;

import chip.platform.ConfigurationManager;

public class AppParameters {
    @NonNull private final Context applicationContext;

    @NonNull private final DataProvider<ConfigurationManager> configurationManager;

    @NonNull private final DataProvider<byte[]> rotatingDeviceIdUniqueIdProvider;

    @NonNull private final DataProvider<CommissioningData> commissioningDataProvider;

    @NonNull private final DataProvider<DeviceAttestationCredentials> dacProvider;

    public AppParameters(
            @NonNull Context applicationContext, @NonNull DataProvider<ConfigurationManager> configurationManager, @NonNull DataProvider<byte[]> rotatingDeviceIdUniqueIdProvider,
            @NonNull DataProvider<CommissioningData> commissioningDataProvider,
            @NonNull DataProvider<DeviceAttestationCredentials> dacProvider) {
        this.applicationContext = applicationContext;
        this.configurationManager = configurationManager;
        this.rotatingDeviceIdUniqueIdProvider = rotatingDeviceIdUniqueIdProvider;
        this.commissioningDataProvider = commissioningDataProvider;
        this.dacProvider = dacProvider;
    }

    @NonNull
    public Context getApplicationContext() {
        return applicationContext;
    }

    @NonNull
    public DataProvider<ConfigurationManager> getConfigurationManager() {
        return configurationManager;
    }

    @NonNull
    public DataProvider<byte[]> getRotatingDeviceIdUniqueIdProvider() {
        return rotatingDeviceIdUniqueIdProvider;
    }

    @NonNull
    public DataProvider<CommissioningData> getCommissioningDataProvider() {
        return commissioningDataProvider;
    }

    @NonNull
    public DataProvider<DeviceAttestationCredentials> getDacProvider() {
        return dacProvider;
    }
}