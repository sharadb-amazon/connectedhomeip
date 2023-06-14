package com.matter.casting.core;

import java.util.List;

public interface CastingPlayerDiscoverer {
  List<CastingPlayer> getDiscoveredCastingPlayers();

  void startDiscovery();

  void stopDiscovery();

  void addCastingPlayerChangeListener(CastingPlayerChangeListener listener);

  void removeCastingPlayerChangeListener(CastingPlayerChangeListener listener);

  interface CastingPlayerChangeListener {
    void onChanged(List<CastingPlayer> castingPlayers);

    void onAdded(List<CastingPlayer> castingPlayers);

    void onRemoved(List<CastingPlayer> castingPlayers);
  }
}
