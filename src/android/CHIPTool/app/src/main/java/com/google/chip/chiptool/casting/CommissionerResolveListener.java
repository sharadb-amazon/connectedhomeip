package com.google.chip.chiptool.casting;

import android.content.Context;
import android.net.nsd.NsdManager;
import android.net.nsd.NsdServiceInfo;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;
import android.widget.Button;
import android.widget.LinearLayout;

import com.google.chip.chiptool.R;
import com.google.chip.chiptool.util.DeviceType;
import com.google.chip.chiptool.util.Product;
import com.google.chip.chiptool.util.Vendor;

import java.nio.charset.StandardCharsets;
import java.util.List;
import java.util.Map;

public class CommissionerResolveListener implements NsdManager.ResolveListener {

    private static final String TAG = CommissionerResolveListener.class.getSimpleName();
    private final CastingFragment fragment;
    private final List<Commissioner> commissioners;

    public CommissionerResolveListener(CastingFragment fragment, List<Commissioner> commissioners) {
        this.fragment = fragment;
        this.commissioners = commissioners;
    }

    @Override
    public void onResolveFailed(NsdServiceInfo serviceInfo, int errorCode) {
        switch (errorCode) {
            case NsdManager.FAILURE_ALREADY_ACTIVE:
                Log.e(TAG, "FAILURE_ALREADY_ACTIVE - Service: " + serviceInfo);
                NsdManager nsdManager = (NsdManager) fragment.getContext()
                        .getSystemService(Context.NSD_SERVICE);
                nsdManager.resolveService(serviceInfo,
                        new CommissionerResolveListener(fragment, commissioners));
                break;
            case NsdManager.FAILURE_INTERNAL_ERROR:
                Log.e(TAG, "FAILURE_INTERNAL_ERROR - Service: " + serviceInfo);
                break;
            case NsdManager.FAILURE_MAX_LIMIT:
                Log.e(TAG, "FAILURE_MAX_LIMIT - Service: " + serviceInfo);
                break;
        }
    }

    @Override
    public void onServiceResolved(NsdServiceInfo serviceInfo) {
        Commissioner commissioner = buildCommissioner(serviceInfo);
        commissioners.add(commissioner);
        Log.d(TAG, "Commissioner resolved: " + commissioner);

        String buttonText = getCommissionerButtonText(commissioner);
        if (!buttonText.isEmpty()) {
            LinearLayout commissionersLayout = fragment.getView().findViewById(R.id.castingCommissioners);
            Button commissionerButton = new Button(fragment.getContext());
            commissionerButton.setText(buttonText);
            commissionerButton.setOnClickListener(
                    new CommissionerOnClickListener(fragment, commissioner));
            new Handler(Looper.getMainLooper()).post(() -> commissionersLayout.addView(commissionerButton));
        } else
            Log.e(TAG, "Skipped displaying " + commissioner);
    }

    private Commissioner buildCommissioner(NsdServiceInfo serviceInfo) {
        Commissioner commissioner = new Commissioner();
        commissioner.setServiceName(serviceInfo.getServiceName());
        commissioner.setServiceType(serviceInfo.getServiceType());
        commissioner.setHost(serviceInfo.getHost());
        serviceInfo.getAttributes();
        Map<String, byte[]> attributes = serviceInfo.getAttributes();
        commissioner.setDeviceName(new String(attributes.get("DN"), StandardCharsets.UTF_8));
        String deviceType = new String(attributes.get("DT"), StandardCharsets.UTF_8);
        commissioner.setDeviceType(DeviceType.fromId(deviceType));

        String vp = new String(attributes.get("VP"), StandardCharsets.UTF_8);
        if (vp != null) {
            String[] vpArray = vp.split("\\+");
            if (vpArray.length > 0) {
                commissioner.setVendor(Vendor.fromId(vpArray[0]));
                if (vpArray.length == 2) {
                    commissioner.setProduct(Product.fromId(vpArray[1]));
                }
            }
        }
        return commissioner;
    }

    private String getCommissionerButtonText(Commissioner commmissioner) {
        String main = commmissioner.getDeviceName() != null ? commmissioner.getDeviceName() : "";
        String aux = commmissioner.getProduct() != null ? commmissioner.getProduct().toString() : "";
        aux += commmissioner.getDeviceType() != null ? (aux.isEmpty() ? "" : " ") + commmissioner.getDeviceType() : "";
        aux += commmissioner.getVendor() != null ? (aux.isEmpty() ? "" : " from ") + commmissioner.getVendor().toString() : "";
        aux = aux.isEmpty() ? aux : " [" + aux + "]";
        return main + aux;
    }
}
