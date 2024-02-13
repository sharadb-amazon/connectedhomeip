package com.matter.casting.core;

import com.matter.casting.support.DeviceTypeStruct;
import java.util.List;
import java.util.Objects;

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
  public native <T extends Cluster> T getCluster(Class<T> clusterClass);

  @Override
  public boolean hasCluster(Class<? extends Cluster> clusterClass) {
    return getCluster(clusterClass) != null;
  }

  @Override
  public String toString() {
    return "MatterEndpoint{" + "id=" + getId() + '}';
  }

  @Override
  public boolean equals(Object o) {
    if (this == o) return true;
    if (o == null || getClass() != o.getClass()) return false;
    MatterEndpoint that = (MatterEndpoint) o;
    return getId() == that.getId();
  }

  @Override
  public int hashCode() {
    return Objects.hash(getId());
  }
}
