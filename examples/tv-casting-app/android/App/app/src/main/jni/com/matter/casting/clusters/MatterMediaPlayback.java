package com.matter.casting.clusters;

import com.matter.casting.core.Attribute;
import com.matter.casting.core.BasicMatterCluster;
import com.matter.casting.core.Endpoint;

public class MatterMediaPlayback extends BasicMatterCluster implements MediaPlayback {
  private final Attribute<CurrentState> currentState;

  MatterMediaPlayback(Endpoint endpoint) {
    super(endpoint);
    // creates attributes
    currentState = null;
  }

  public Attribute<CurrentState> getCurrentState() {
    return currentState;
  }
}
