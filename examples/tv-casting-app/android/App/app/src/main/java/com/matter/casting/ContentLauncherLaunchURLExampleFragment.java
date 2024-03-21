/*
 *   Copyright (c) 2024 Project CHIP Authors
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
 */
package com.matter.casting;

import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;
import chip.devicecontroller.ChipClusters;
import com.R;
import com.matter.casting.core.CastingPlayer;
import com.matter.casting.core.Endpoint;
import com.matter.casting.core.MatterEndpoint;
import com.matter.casting.support.MatterCallback;
import com.matter.casting.support.MatterError;
import java.util.List;
import java.util.Optional;

/** A {@link Fragment} to send Content Launcher LaunchURL command from the TV Casting App. */
public class ContentLauncherLaunchURLExampleFragment extends Fragment {
  private static final String TAG = ContentLauncherLaunchURLExampleFragment.class.getSimpleName();
  private static final Integer SAMPLE_ENDPOINT_VID = 65521;

  private final CastingPlayer selectedCastingPlayer;

  private View.OnClickListener launchUrlButtonClickListener;

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
  public static ContentLauncherLaunchURLExampleFragment newInstance(
      CastingPlayer selectedCastingPlayer) {
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
          Endpoint endpoint = selectEndpoint();
          if (endpoint == null) {
            Log.e(
                TAG,
                "No Endpoint with chosen vendorID: "
                    + SAMPLE_ENDPOINT_VID
                    + " found on CastingPlayer");
            return;
          }

          // TODO: complete command invocation API implementation
          ((MatterEndpoint) endpoint)
              .getDeviceProxy(
                  new MatterCallback<Long>() {
                    @Override
                    public void handle(Long device) {
                      Log.d(TAG, "getDeviceProxy success. Device: " + device);

                      if (device != null) {
                        ChipClusters.ContentLauncherCluster cluster =
                            new ChipClusters.ContentLauncherCluster(device, endpoint.getId());
                        Log.d(TAG, "Content launcher cluster created " + cluster);
                        cluster.launchURL(
                            new ChipClusters.ContentLauncherCluster.LauncherResponseCallback() {
                              @Override
                              public void onSuccess(Integer status, Optional<String> data) {
                                Log.d(TAG, "Content launcher success " + status + data);
                              }

                              @Override
                              public void onError(Exception error) {
                                Log.e(TAG, "Content launcher failure " + error);
                              }
                            },
                            "my test url",
                            Optional.of("my display str"),
                            Optional.empty());
                      }
                    }
                  },
                  new MatterCallback<MatterError>() {
                    @Override
                    public void handle(MatterError err) {
                      Log.e(TAG, "getDeviceProxy err" + err);
                    }
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

  private Endpoint selectEndpoint() {
    Endpoint endpoint = null;
    if (selectedCastingPlayer != null) {
      List<Endpoint> endpoints = selectedCastingPlayer.getEndpoints();
      if (endpoints == null) {
        Log.e(TAG, "No Endpoints found on CastingPlayer");
      } else {
        endpoint =
            endpoints
                .stream()
                .filter(e -> SAMPLE_ENDPOINT_VID.equals(e.getVendorId()))
                .findFirst()
                .get();
      }
    }
    return endpoint;
  }
}
