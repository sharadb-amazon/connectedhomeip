package com.matter.casting.core;

import java.util.List;
import java.util.Observable;
import java.util.concurrent.CompletableFuture;

public interface CastingPlayer {
  String getId();

  String getName();

  String getVendorId();

  String getProductId();

  String getType();

  List<Endpoint> getEndpoints();

  ConnectionState getConnectionState();

  CompletableFuture<Void> connect(long timeout);

  static class ConnectionState extends Observable {
    private boolean connected;

    void setConnected(boolean connected) {
      this.connected = connected;
      setChanged();
      notifyObservers(this.connected);
    }

    boolean isConnected() {
      return connected;
    }
  }
}
