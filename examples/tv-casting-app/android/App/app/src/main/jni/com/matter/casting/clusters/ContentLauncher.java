package com.matter.casting.clusters;

import com.matter.casting.core.Cluster;
import com.matter.casting.err.UnsupportedCommandException;
import java.net.URL;
import java.util.concurrent.CompletableFuture;

public interface ContentLauncher extends Cluster {
  boolean canLaunchURL();

  boolean canLaunchContent();

  CompletableFuture<LauncherResponse> launchURL(
      URL contentUrl, String displayStr, BrandingInformation BrandingInformation)
      throws UnsupportedCommandException;

  CompletableFuture<LauncherResponse> launchContent(
      ContentSearch search, boolean autoPlay, String data) throws UnsupportedCommandException;

  static class LauncherResponse {}

  static class BrandingInformation {}

  static class ContentSearch {}
}
