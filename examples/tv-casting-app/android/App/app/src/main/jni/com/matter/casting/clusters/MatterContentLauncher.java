package com.matter.casting.clusters;

import com.matter.casting.core.BasicMatterCluster;
import com.matter.casting.core.Endpoint;
import com.matter.casting.err.UnsupportedCommandException;
import java.net.URL;
import java.util.concurrent.CompletableFuture;

public class MatterContentLauncher extends BasicMatterCluster implements ContentLauncher {
  public MatterContentLauncher(Endpoint endpoint) {
    super(endpoint);
  }

  @Override
  public boolean canLaunchURL() {
    return true;
  }

  @Override
  public boolean canLaunchContent() {
    return true;
  }

  @Override
  public CompletableFuture<LauncherResponse> launchURL(
      URL contentUrl, String displayStr, BrandingInformation BrandingInformation)
      throws UnsupportedCommandException {
    return null;
  }

  @Override
  public CompletableFuture<LauncherResponse> launchContent(
      ContentSearch search, boolean autoPlay, String data) throws UnsupportedCommandException {
    return null;
  }
}
