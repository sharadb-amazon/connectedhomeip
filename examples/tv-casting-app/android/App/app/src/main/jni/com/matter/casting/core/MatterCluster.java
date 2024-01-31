package com.matter.casting.core;

public class MatterCluster implements Cluster {
    protected long _cppCluster;

    @Override
    public native Endpoint getEndpoint();

    @Override
    public native Object getCommand(int commandId);
}
