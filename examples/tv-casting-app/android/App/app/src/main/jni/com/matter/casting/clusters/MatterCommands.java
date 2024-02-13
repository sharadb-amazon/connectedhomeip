package com.matter.casting.clusters;

import com.matter.casting.core.MatterCommand;

public class MatterCommands {
  public static class ContentLauncherClusterLaunchURLRequest {
    public String contentURL;
    public String displayString;
    // Optional<ContentLauncherBrandingInformationStruct> brandingInformation;
  }

  public static class ContentLauncherClusterResponse {
    public String data;
    public Integer status;
  }

  public static class ContentLauncherClusterLaunchURLCommand
      extends MatterCommand<
          ContentLauncherClusterLaunchURLRequest, ContentLauncherClusterResponse> {}
}
