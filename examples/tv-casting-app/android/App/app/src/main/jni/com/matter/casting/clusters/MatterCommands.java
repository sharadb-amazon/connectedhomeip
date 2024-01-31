package com.matter.casting.clusters;

import com.matter.casting.core.MatterCommand;

import java.util.Optional;
import java.util.concurrent.CompletableFuture;

public class MatterCommands {
    public static class ContentLauncherClusterLaunchURLRequest
    {
        public String contentURL;
        public Optional<String> displayString;
        //Optional<ContentLauncherBrandingInformationStruct> brandingInformation;
    }

    public static class ContentLauncherClusterResponse
    {

    }

    public static class ContentLauncherClusterLaunchURLCommand extends MatterCommand<ContentLauncherClusterLaunchURLRequest, ContentLauncherClusterResponse>
    {
        @Override
        public native CompletableFuture<ContentLauncherClusterResponse> invoke(ContentLauncherClusterLaunchURLRequest contentLauncherLaunchURLRequest, Object context, Integer timedInvokeTimeoutMs);
    }
}
