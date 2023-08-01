/*
 *   Copyright (c) 2020-23 Project CHIP Authors
 *   All rights reserved.
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 *
 */

package com.matter.casting;

import android.util.Log;
import com.matter.casting.clusters.MediaPlayback;
import com.matter.casting.core.Attribute;
import com.matter.casting.core.CastingPlayer;
import com.matter.casting.core.Endpoint;
import com.matter.casting.err.UnsupportedClusterException;
import java.util.Optional;
import java.util.concurrent.CompletableFuture;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.TimeoutException;

public class AttributeReadExample {
  private static final String TAG = AttributeReadExample.class.getSimpleName();

  private static final String SAMPLE_CONTENT_APP_VID = "12345";

  public static void demoRead(CastingPlayer castingPlayer) throws UnsupportedClusterException {

    // pick the Content app Endpoint to invoke the command on
    Optional<Endpoint> contentAppEndpoint =
        castingPlayer
            .getEndpoints()
            .stream()
            .filter(endpoint -> SAMPLE_CONTENT_APP_VID.equals(endpoint.getVendorId()))
            .findAny();

    if (contentAppEndpoint.isPresent()) {
      // check if the Endpoint supports the Cluster to use
      if (contentAppEndpoint.get().hasCluster(MediaPlayback.class)) {
        // get the Cluster to use
        MediaPlayback mediaPlayback = contentAppEndpoint.get().getCluster(MediaPlayback.class);

        Attribute<MediaPlayback.CurrentState> currentStateAttribute =
            mediaPlayback.getCurrentState();
        if (currentStateAttribute.isAvailable()) {
          CompletableFuture<MediaPlayback.CurrentState> currentStateValue =
              currentStateAttribute.read();
          try {
            Log.i(TAG, "Read Current State value: " + currentStateValue.get(1, TimeUnit.SECONDS));

          } catch (ExecutionException | InterruptedException | TimeoutException e) {
            Log.e(TAG, "Exception when reading CurrentState " + e);
          }
        } else {
          Log.e(TAG, "Attribute unavailable on the selected endpoint");
        }
      } else {
        Log.e(TAG, "Required cluster not found on the selected endpoint");
      }
    } else {
      Log.e(TAG, "Content app endpoint not found");
    }
  }
}
