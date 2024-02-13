package com.matter.casting;

import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.EditText;
import android.widget.TextView;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;
import com.R;
import com.chip.casting.ContentApp;
import com.chip.casting.MatterCallbackHandler;
import com.chip.casting.MatterError;
import com.chip.casting.TvCastingApp;
import com.matter.casting.clusters.MatterClusters;
import com.matter.casting.clusters.MatterCommands;
import com.matter.casting.core.CastingPlayer;
import com.matter.casting.core.Endpoint;

import java.util.List;
import java.util.Optional;
import java.util.concurrent.CompletableFuture;

/** A {@link Fragment} to send Content Launcher LaunchURL command from the TV Casting App. */
public class ContentLauncherLaunchURLExampleFragment extends Fragment {
    private static final String TAG = ContentLauncherLaunchURLExampleFragment.class.getSimpleName();
    private static final Integer SAMPLE_ENDPOINT_VID = 65521;

    private final CastingPlayer selectedCastingPlayer;

    private View.OnClickListener launchUrlButtonClickListener;

    private static final ContentApp kContentApp = new ContentApp((short) 4, null);

    public ContentLauncherLaunchURLExampleFragment(CastingPlayer selectedCastingPlayer) {
        this.selectedCastingPlayer = selectedCastingPlayer;
    }

    /**
     * Use this factory method to create a new instance of this fragment using the provided
     * parameters.
     *
     * @param selectedCastingPlayer CastingPlayer that the casting app connected to
     * @return A new instance of fragment ContentLauncherLaunchURLExampleFragment.
     */
    public static ContentLauncherLaunchURLExampleFragment newInstance(CastingPlayer selectedCastingPlayer) {
        return new ContentLauncherLaunchURLExampleFragment(selectedCastingPlayer);
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }

    @Override
    public View onCreateView(
            LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        this.launchUrlButtonClickListener =
                v -> {
                    List<Endpoint> endpoints = selectedCastingPlayer.getEndpoints();
                    if(endpoints == null)
                    {
                        Log.e(TAG, "No Endpoints found on CastingPlayer");
                        return;
                    }

                    Endpoint endpoint = endpoints.stream().filter(e -> SAMPLE_ENDPOINT_VID.equals(e.getVendorId())).findFirst().get();
                    if(endpoint == null)
                    {
                        Log.e(TAG, "No Endpoint with chosen vendorID: " + SAMPLE_ENDPOINT_VID + " found on CastingPlayer");
                        return;
                    }

                    if(!endpoint.hasCluster(MatterClusters.ContentLauncherCluster.class))
                    {
                        Log.e(TAG, "Endpoint with chosen vendorID does not support ContentLauncher cluster");
                        return;
                    }

                    MatterClusters.ContentLauncherCluster cluster = endpoint.getCluster(MatterClusters.ContentLauncherCluster.class);
                    MatterCommands.ContentLauncherClusterLaunchURLCommand command = cluster.getCommand(MatterCommands.ContentLauncherClusterLaunchURLCommand.class);
                    if(command == null)
                    {
                        Log.e(TAG, "ContentLauncher cluster on Endpoint with chosen vendorID does not support LaunchURL command");
                        return;
                    }

                    MatterCommands.ContentLauncherClusterLaunchURLRequest request = new MatterCommands.ContentLauncherClusterLaunchURLRequest();
                    request.contentURL = ((EditText) getView().findViewById(R.id.contentUrlEditText)).getText().toString();
                    request.displayString = ((EditText) getView().findViewById(R.id.contentDisplayStringEditText)).getText().toString();
                    CompletableFuture<MatterCommands.ContentLauncherClusterResponse> responseFuture = command.invoke(request, null, 5000);
                    responseFuture.thenAccept(response -> {
                        Log.d(TAG, "Command response " + response);
                        TextView launchUrlStatus = getView().findViewById(R.id.launchUrlStatus);
                        getActivity().runOnUiThread(() -> launchUrlStatus.setText("Success! Response data: " + response.data));
                    })
                    .exceptionally(exc -> {
                        Log.e(TAG, "Command failure: " + exc.getMessage());
                        TextView launchUrlStatus = getView().findViewById(R.id.launchUrlStatus);
                        getActivity().runOnUiThread(() -> launchUrlStatus.setText("Command failure: " + exc.getMessage()));
                        return null;
                    });
                };

        return inflater.inflate(R.layout.fragment_content_launcher, container, false);
    }

    @Override
    public void onViewCreated(View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        Log.d(TAG, "ContentLauncherLaunchURLExampleFragment.onViewCreated called");
        getView().findViewById(R.id.launchUrlButton).setOnClickListener(launchUrlButtonClickListener);
    }
}
