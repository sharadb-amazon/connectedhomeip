package com.matter.casting.clusters;

import com.matter.casting.core.Attribute;
import com.matter.casting.core.Cluster;
import java.util.Optional;

public interface MediaPlayback extends Cluster {
  Optional<Attribute<CurrentState>> getCurrentState();

  enum CurrentState {}
}
