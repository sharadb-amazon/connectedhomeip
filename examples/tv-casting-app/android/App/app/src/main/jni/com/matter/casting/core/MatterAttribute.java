package com.matter.casting.core;

import java.util.concurrent.CompletableFuture;

public class MatterAttribute<T> implements Attribute {
  public MatterAttribute(int id, BasicMatterCluster cluster) {}

  @Override
  public Cluster getCluster() {
    return null;
  }

  @Override
  public CompletableFuture<T> read() {
    return null;
  }

  @Override
  public CompletableFuture<Void> write(Object value) {
    return null;
  }

  @Override
  public void addObserver(Listener listener, int minInterval, int maxInterval) {}

  @Override
  public void removeObserver(Listener listener) {}

  @Override
  public boolean isAvailable() {
    return false;
  }
}
