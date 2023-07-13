package com.matter.casting;

import android.util.Log;
import com.matter.casting.clusters.ContentLauncher;
import com.matter.casting.core.CastingPlayer;
import com.matter.casting.core.Endpoint;
import com.matter.casting.err.UnsupportedClusterException;
import com.matter.casting.err.UnsupportedCommandException;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.Optional;
import java.util.concurrent.CompletableFuture;

public class CommandInvocationExample {
  private static final String TAG = CommandInvocationExample.class.getSimpleName();

  private static final String SAMPLE_CONTENT_APP_VID = "12345";

  public static void demoCommandInvocation(CastingPlayer castingPlayer)
      throws MalformedURLException, UnsupportedCommandException, UnsupportedClusterException {

    // pick the Content app Endpoint to invoke the command on
    Optional<Endpoint> contentAppEndpoint =
        castingPlayer
            .getEndpoints()
            .stream()
            .filter(endpoint -> SAMPLE_CONTENT_APP_VID.equals(endpoint.getVendorId()))
            .findAny();

    if (contentAppEndpoint.isPresent()) {
      // check if the Endpoint supports the Cluster to use
      if (contentAppEndpoint.get().hasCluster(ContentLauncher.class)) {
        // get the Cluster to use
        ContentLauncher contentLauncher =
            contentAppEndpoint.get().getCluster(ContentLauncher.class);

        // invoke the command
        CompletableFuture<ContentLauncher.LauncherResponse> contentLauncherResponseFuture =
            contentLauncher.launchURL(
                new URL("https://www.samplemattercontent.xyz/id"), "displaystring", null);
        contentLauncherResponseFuture.thenApply(
            (response) -> {
              // handle the response
              return null;
            });
      }
    } else {
      Log.d(TAG, "Content app endpoint not found");
    }
  }
}
