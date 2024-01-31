package com.matter.casting.core;

import com.matter.casting.support.DeviceTypeStruct;
import com.matter.casting.support.UnsupportedClusterException;

import java.util.List;

public class MatterEndpoint implements Endpoint {
    protected long _cppEndpoint;

    @Override
    public native int getId();

    @Override
    public native int getVendorId();

    @Override
    public native int getProductId();

    @Override
    public native List<DeviceTypeStruct> getDeviceTypeList();

    @Override
    public native CastingPlayer getCastingPlayer();

    @Override
    public <T extends Cluster> T getCluster(Class<T> clusterClass) throws UnsupportedClusterException {
        return null;
    }

    @Override
    public boolean hasCluster(Class<? extends Cluster> clusterClass) {
        return false;
    }
}
