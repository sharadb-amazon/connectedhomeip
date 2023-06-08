package com.matter.casting.clusters;

import com.matter.casting.core.Attribute;
import com.matter.casting.core.BasicMatterCluster;
import com.matter.casting.core.Endpoint;
import java.util.Optional;

public class MatterMediaPlayback extends BasicMatterCluster implements MediaPlayback {
  private final Optional<Attribute<CurrentState>> currentState;

  MatterMediaPlayback(Endpoint endpoint) {
    super(endpoint);
    // creates attributes
    currentState = Optional.empty();
  }

  public Optional<Attribute<CurrentState>> getCurrentState() {
    return currentState;
  }
}
