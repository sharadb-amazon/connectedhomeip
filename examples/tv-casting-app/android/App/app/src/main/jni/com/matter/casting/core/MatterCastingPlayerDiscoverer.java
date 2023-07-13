package com.matter.casting.core;

import java.util.List;

public final class MatterCastingPlayerDiscoverer implements CastingPlayerDiscoverer {

  private static MatterCastingPlayerDiscoverer instance = null;

  private MatterCastingPlayerDiscoverer() {}

  public static MatterCastingPlayerDiscoverer getInstance() {
    if (instance == null) {
      instance = new MatterCastingPlayerDiscoverer();
    }
    return instance;
  }

  @Override
  public List<CastingPlayer> getDiscoveredCastingPlayers() {
    return null;
  }

  @Override
  public void startDiscovery() {}

  @Override
  public void stopDiscovery() {}

  @Override
  public void addCastingPlayerChangeListener(CastingPlayerChangeListener listener) {}

  @Override
  public void removeCastingPlayerChangeListener(CastingPlayerChangeListener listener) {}
}
