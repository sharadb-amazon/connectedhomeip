package com.matter.casting.core;

import com.matter.casting.err.UnsupportedClusterException;

public interface ClusterProvider {
  <T extends Cluster> T getCluster(Class<T> clusterClass) throws UnsupportedClusterException;

  boolean hasCluster(Class<? extends Cluster> clusterClass);
}
