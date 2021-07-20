package com.google.chip.chiptool.casting;

import android.content.Context;
import android.net.nsd.NsdManager;
import android.net.nsd.NsdServiceInfo;
import android.util.Log;

import java.util.ArrayList;
import java.util.List;

public class CommissionerDiscoveryListener implements NsdManager.DiscoveryListener {

    private static final String TAG = CommissionerDiscoveryListener.class.getSimpleName();

    private CastingFragment fragment;
    private final NsdManager nsdManager;
    private final List<Commissioner> commissioners = new ArrayList<>();

    public CommissionerDiscoveryListener(CastingFragment fragment) {
        this.fragment = fragment;
        this.nsdManager = (NsdManager) fragment.getContext().getSystemService(Context.NSD_SERVICE);
    }

    @Override
    public void onDiscoveryStarted(String regType) {
        Log.d(TAG, "Service discovery started. regType: " + regType);
    }

    @Override
    public void onServiceFound(NsdServiceInfo service) {
        Log.d(TAG, "Service discovery success. " + service);
        if (service.getServiceType().equals(CastingConstants.CommissionerServiceType) &&
                !service.getServiceName().equals((CastingConstants.CommissioneeServiceName))) {
            nsdManager.resolveService(service, new CommissionerResolveListener(fragment, commissioners));
        } else {
            Log.d(TAG, "Ignoring discovered service: " + service.toString());
        }

    }

    @Override
    public void onServiceLost(NsdServiceInfo service) {
        // When the network service is no longer available.
        // Internal bookkeeping code goes here.
        Log.e(TAG, "Service lost: " + service);
    }

    @Override
    public void onDiscoveryStopped(String serviceType) {
        Log.i(TAG, "Discovery stopped: " + serviceType);
    }

    @Override
    public void onStartDiscoveryFailed(String serviceType, int errorCode) {
        Log.e(TAG, "Discovery failed to start: Error code:" + errorCode);
        nsdManager.stopServiceDiscovery(this);
    }

    @Override
    public void onStopDiscoveryFailed(String serviceType, int errorCode) {
        Log.e(TAG, "Discovery failed to stop: Error code:" + errorCode);
        nsdManager.stopServiceDiscovery(this);
    }
}
