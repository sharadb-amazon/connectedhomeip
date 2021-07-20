package com.google.chip.chiptool.casting;

import android.content.Context;
import android.net.nsd.NsdManager;
import android.net.nsd.NsdServiceInfo;
import android.util.Log;
import android.view.View;

import com.google.chip.chiptool.util.DeviceType;
import com.google.chip.chiptool.util.Vendor;

public class CommissionerOnClickListener implements View.OnClickListener {

    private static final String TAG = CommissionerOnClickListener.class.getSimpleName();

    private final CastingFragment fragment;
    private final Commissioner commissioner;

    public CommissionerOnClickListener(CastingFragment fragment, Commissioner commissioner) {
        this.fragment = fragment;
        this.commissioner = commissioner;
    }

    @Override
    public void onClick(View v) {
        Log.d(TAG, "Commissioner selected " + commissioner);
        registerCommissionee();
        sendUdcUdpPacket(commissioner);
    }

    private void registerCommissionee() {
        NsdServiceInfo serviceInfo = new NsdServiceInfo();
        serviceInfo.setServiceName(CastingConstants.CommissioneeServiceName);
        serviceInfo.setServiceType(CastingConstants.CommissioneeServiceType);
        serviceInfo.setPort(CastingConstants.CommissioneeServicePort);
        serviceInfo.setAttribute("DN", CastingConstants.CommissioneeServiceName);
        serviceInfo.setAttribute("DT", DeviceType.ANDROID_APP.getId());
        serviceInfo.setAttribute("VP", Vendor.Amazon.getId());

        NsdManager nsdManager = (NsdManager) fragment.getContext().getSystemService(Context.NSD_SERVICE);

        NsdManager.RegistrationListener registrationListener = new CommissioneeRegistrationListener(fragment);
        nsdManager.registerService(
                serviceInfo, NsdManager.PROTOCOL_DNS_SD, registrationListener);
    }

    private void sendUdcUdpPacket(Commissioner commissioner) {
        Log.d(TAG, "(TODO) Need to send User Directed Commissioning request to " + commissioner);
    }
}
