package com.matter.casting.clusters;

import com.matter.casting.core.Attribute;
import com.matter.casting.core.Cluster;

public interface MediaPlayback extends Cluster {
  Attribute<CurrentState> getCurrentState();

  enum CurrentState {}
}
