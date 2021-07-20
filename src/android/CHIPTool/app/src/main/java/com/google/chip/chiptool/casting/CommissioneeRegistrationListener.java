package com.google.chip.chiptool.casting;

import android.net.nsd.NsdManager;
import android.net.nsd.NsdServiceInfo;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;
import android.widget.TextView;

import com.google.chip.chiptool.R;

public class CommissioneeRegistrationListener implements NsdManager.RegistrationListener {

    private static final String TAG = CommissioneeRegistrationListener.class.getSimpleName();
    private final CastingFragment fragment;

    public CommissioneeRegistrationListener(CastingFragment fragment) {
        this.fragment = fragment;
    }

    @Override
    public void onServiceRegistered(NsdServiceInfo nsdServiceInfo) {
        Log.d(TAG, "DNS-SD service registered: " + nsdServiceInfo + " Attributes: " + nsdServiceInfo.getAttributes());
        new Handler(Looper.getMainLooper()).post(() -> {
            TextView pinLabel = fragment.getView().findViewById(R.id.castingPinLabel);
            pinLabel.setText("PIN");
            TextView pinValue = fragment.getView().findViewById(R.id.castingPinValue);
            pinValue.setText("X1Y2Z3");
        });

    }

    @Override
    public void onRegistrationFailed(NsdServiceInfo serviceInfo, int errorCode) {
        String status = "DNS-SD service registration failed: errorCode: " + errorCode;
        Log.e(TAG, status);
        new Handler(Looper.getMainLooper()).post(() -> {
            TextView commissioneeRegistrationStatus = fragment.getView().findViewById(R.id.castingCommissioneeRegistrationStatus);
            commissioneeRegistrationStatus.setText(status);
        });
    }

    @Override
    public void onServiceUnregistered(NsdServiceInfo serviceInfo) {
        // Service has been unregistered. This only happens when you call
        // NsdManager.unregisterService() and pass in this listener.
    }

    @Override
    public void onUnregistrationFailed(NsdServiceInfo serviceInfo, int errorCode) {
        // Unregistration failed. Put debugging code here to determine why.
    }
}