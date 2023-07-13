package com.matter.casting.core;

import com.matter.casting.err.UnsupportedClusterException;

public interface Endpoint {
  String getId();

  String getVendorId();

  String getProductId();

  String getType();

  CastingPlayer getCastingPlayer();

  <T extends Cluster> T getCluster(Class<T> clusterClass) throws UnsupportedClusterException;

  boolean hasCluster(Class<? extends Cluster> clusterClass);
}
