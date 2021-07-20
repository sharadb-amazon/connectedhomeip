/*
 *   Copyright (c) 2020 Project CHIP Authors
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
 *
 */

package com.google.chip.chiptool.casting;

import android.content.Context;
import android.net.nsd.NsdManager;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import androidx.annotation.NonNull;
import androidx.fragment.app.Fragment;

import com.google.chip.chiptool.R;

public class CastingFragment extends Fragment {

    private static final String TAG = CastingFragment.class.getSimpleName();

    public CastingFragment() {
        Log.i(TAG, "CastingFragment constructor called");
    }

    @Override
    public View onCreateView(
            LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        Log.i(TAG, "onCreateView: CastingFragment");
        return inflater.inflate(R.layout.casting_fragment, container, false);
    }

    @Override
    public void onViewCreated(@NonNull View view, Bundle savedInstanceState) {
        Log.i(TAG, "onViewCreated: CastingFragment");
        super.onViewCreated(view, savedInstanceState);

        startCommissionerDiscovery();
    }

    private void startCommissionerDiscovery() {
        NsdManager nsdManager = (NsdManager) getContext().getSystemService(Context.NSD_SERVICE);
        NsdManager.DiscoveryListener commissionerDiscoveryListener =
                new CommissionerDiscoveryListener(this);
        nsdManager.discoverServices(
                CastingConstants.CommissionerServiceType, NsdManager.PROTOCOL_DNS_SD,
                commissionerDiscoveryListener);
    }
}
