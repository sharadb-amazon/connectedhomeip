package com.matter.casting.core;

public class BasicMatterCluster implements Cluster {
  private Endpoint endpoint;

  public BasicMatterCluster(Endpoint endpoint) {
    this.endpoint = endpoint;
  }

  public Endpoint getEndpoint() {
    return endpoint;
  }
}
