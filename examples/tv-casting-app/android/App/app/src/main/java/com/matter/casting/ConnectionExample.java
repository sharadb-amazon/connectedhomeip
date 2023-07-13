package com.matter.casting;

import android.util.Log;
import com.matter.casting.core.CastingPlayer;
import java.util.concurrent.CompletableFuture;

public class ConnectionExample {
  private static final String TAG = ConnectionExample.class.getSimpleName();

  // Maximum time in seconds we'll wait for the connection to the CastingPlayer to go through
  private static final long CONNECTION_TIMEOUT_SECS = 45;

  public static void demoConnection(CastingPlayer castingPlayer) {
    // Connect to the castingPlayer
    CompletableFuture<Void> connectionFuture = castingPlayer.connect(CONNECTION_TIMEOUT_SECS);

    // handle exception, if any
    connectionFuture.exceptionally(
        exc -> {
          Log.e(TAG, "Exception in connecting to castingPlayer" + exc.getMessage());
          return null;
        });
  }
}
