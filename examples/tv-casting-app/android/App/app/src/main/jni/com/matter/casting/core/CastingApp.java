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

package com.matter.casting.core;

import android.content.Context;
import android.util.Log;

import com.chip.casting.TvCastingApp;
import com.matter.casting.support.CommissionableData;
import com.matter.casting.support.MatterError;
import com.matter.casting.support.AppParameters;

import chip.appserver.ChipAppServer;
import chip.platform.AndroidBleManager;
import chip.platform.AndroidChipPlatform;
import chip.platform.ChipMdnsCallbackImpl;
import chip.platform.DiagnosticDataProviderImpl;
import chip.platform.NsdManagerServiceBrowser;
import chip.platform.NsdManagerServiceResolver;
import chip.platform.PreferencesKeyValueStoreManager;

public final class CastingApp {
    private static final String TAG = CastingApp.class.getSimpleName();

    private static CastingApp sInstance;

    private AppParameters appParameters;
    private NsdManagerServiceResolver.NsdManagerResolverAvailState nsdManagerResolverAvailState;

    private ChipAppServer chipAppServer;

    private CastingApp() {}

    public static CastingApp getInstance() {
      if (sInstance == null) {
        sInstance = new CastingApp();
      }
      return sInstance;
    }

    public MatterError initialize(AppParameters appParameters)
    {
        this.appParameters = appParameters;
        this.nsdManagerResolverAvailState = new NsdManagerServiceResolver.NsdManagerResolverAvailState();

        Context applicationContext = appParameters.getApplicationContext();
        AndroidChipPlatform chipPlatform =
                new AndroidChipPlatform(
                        new AndroidBleManager(),
                        new PreferencesKeyValueStoreManager(appParameters.getApplicationContext()),
                        appParameters.getConfigurationManagerProvider().get(),
                        new NsdManagerServiceResolver(applicationContext, nsdManagerResolverAvailState),
                        new NsdManagerServiceBrowser(applicationContext),
                        new ChipMdnsCallbackImpl(),
                        new DiagnosticDataProviderImpl(applicationContext));

        CommissionableData commissionableData = appParameters.getCommissionableDataProvider().get();
        boolean updated =
                chipPlatform.updateCommissionableDataProviderData(
                        commissionableData.getSpake2pVerifierBase64(),
                        commissionableData.getSpake2pSaltBase64(),
                        commissionableData.getSpake2pIterationCount(),
                        commissionableData.getSetupPasscode(),
                        commissionableData.getDiscriminator());
        if (!updated) {
            Log.e(TAG,"CastingApp.initApp failed to updateCommissionableDataProviderData on chipPlatform");
            return MatterError.CHIP_ERROR_INVALID_ARGUMENT;
        }

        return _initialize(appParameters);
    }

    private native MatterError _initialize(AppParameters appParameters);

    // called after initialize
    public MatterError start() {
        boolean serverStarted = chipAppServer.startApp();
        if (!serverStarted) {
            Log.e(TAG, "CCastingApp.start failed to start Matter server");
            return MatterError.CHIP_ERROR_INCORRECT_STATE;
        }
        return _postStartRegistrations();
    }

    private native MatterError _postStartRegistrations();

    public native MatterError stop();


    static {
        System.loadLibrary("TvCastingApp");
    }
}