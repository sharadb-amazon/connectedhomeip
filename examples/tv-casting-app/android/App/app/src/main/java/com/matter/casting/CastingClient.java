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

public class CastingClient {
  private static String Tag = CastingClient.class.getSimpleName();

  private static final String SAMPLE_CONTENT_APP_VID = "12345";

  public static void runAndLaunchContent(CastingPlayer castingPlayer)
      throws MalformedURLException, UnsupportedCommandException, UnsupportedClusterException {
    Optional<Endpoint> contentAppEndpoint =
        castingPlayer
            .getEndpoints()
            .stream()
            .filter(endpoint -> SAMPLE_CONTENT_APP_VID.equals(endpoint.getVendorId()))
            .findAny();
    if (contentAppEndpoint.isPresent()) {
      if (contentAppEndpoint.get().hasCluster(ContentLauncher.class)) {
        ContentLauncher contentLauncher = null;
        contentLauncher = contentAppEndpoint.get().getCluster(ContentLauncher.class);
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
      Log.d(Tag, "Content app endpoint not found");
    }
  }
}
