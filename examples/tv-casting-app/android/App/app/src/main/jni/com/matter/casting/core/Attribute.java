package com.matter.casting.core;

import java.util.concurrent.CompletableFuture;

public interface Attribute<T> {
  Cluster getCluster();

  CompletableFuture<T> read();

  CompletableFuture<Void> write(T value);

  void addObserver(Listener listener, int minInterval, int maxInterval);

  void removeObserver(Listener listener);

  boolean isAvailable();

  interface Listener {
    void onError(Error error);

    void onChange(Object value);
  }
}
