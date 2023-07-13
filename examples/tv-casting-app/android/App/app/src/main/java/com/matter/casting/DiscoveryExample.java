package com.matter.casting;

import android.util.Log;
import com.matter.casting.core.CastingPlayer;
import com.matter.casting.core.CastingPlayerDiscoverer;
import com.matter.casting.core.CastingPlayerDiscoverer.CastingPlayerChangeListener;
import com.matter.casting.core.MatterCastingPlayerDiscoverer;
import java.util.List;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;

public class DiscoveryExample {
  private static final ScheduledExecutorService executorService =
      Executors.newSingleThreadScheduledExecutor();

  /**
   * Implementation of a CastingPlayerChangeListener used to listen to changes in the discovered
   * CastingPlayers
   */
  private static final CastingPlayerChangeListener castingPlayerChangeListener =
      new CastingPlayerChangeListener() {
        private final String TAG = CastingPlayerChangeListener.class.getSimpleName();

        @Override
        public void onChanged(List<CastingPlayer> castingPlayers) {
          Log.i(TAG, "Discovered changes to " + castingPlayers.size() + " CastingPlayers");
          // consume changes to the provided castingPlayers
        }

        @Override
        public void onAdded(List<CastingPlayer> castingPlayers) {
          Log.i(TAG, "Discovered " + castingPlayers.size() + " CastingPlayers");
          // consume discovered castingPlayers
        }

        @Override
        public void onRemoved(List<CastingPlayer> castingPlayers) {
          Log.i(TAG, "Removed " + castingPlayers.size() + " CastingPlayers");
          // consume castingPlayers removed or lost from the network
        }
      };

  public static void demoDiscovery() {
    // Get the singleton instance of the MatterCastingPlayerDiscoverer
    CastingPlayerDiscoverer discoverer = MatterCastingPlayerDiscoverer.getInstance();

    // Add our castingPlayerChangeListener to listen to changes in the discovered CastingPlayers
    discoverer.addCastingPlayerChangeListener(castingPlayerChangeListener);

    // Start discovery
    discoverer.startDiscovery();

    // After some time, stop discovering and remove our castingPlayerChangeListener from the set of
    // listeners the CastingPlayerDiscoverer informs
    executorService.schedule(
        () -> {
          discoverer.stopDiscovery();
          discoverer.removeCastingPlayerChangeListener(castingPlayerChangeListener);
        },
        30,
        TimeUnit.SECONDS);
  }
}
