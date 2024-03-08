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
package com.matter.casting.core;

import android.util.Log;

import com.chip.casting.MatterError;
import com.matter.casting.support.DeviceTypeStruct;
import com.matter.casting.support.FailureCallback;
import com.matter.casting.support.SuccessCallback;
import java.util.List;
import java.util.Objects;
import java.util.Optional;
import java.util.concurrent.CompletableFuture;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.TimeoutException;

import chip.devicecontroller.ChipClusters;
import chip.devicecontroller.ChipStructs;

public class MatterEndpoint implements Endpoint {
  private static final String TAG = MatterEndpoint.class.getSimpleName();
  protected long _cppEndpoint;

  @Override
  public native int getId();

  @Override
  public native int getVendorId();

  @Override
  public native int getProductId();

  @Override
  public native List<DeviceTypeStruct> getDeviceTypeList();

  @Override
  public native CastingPlayer getCastingPlayer();

  protected native void getDeviceProxy(SuccessCallback<Long> successCallback, FailureCallback failureCallback);

  protected CompletableFuture<Long> getDeviceProxy()
  {
    CompletableFuture<Long> future = new CompletableFuture<>();

    getDeviceProxy(new SuccessCallback<Long>() {
      @Override
      public void handle(Long deviceProxyPtr) {
        future.complete(deviceProxyPtr);
      }
    }, new FailureCallback() {
      @Override
      public void handle(MatterError err) {
        future.completeExceptionally(new RuntimeException("Could not get CastingPlayer device proxy ptr: " + err));
      }
    });

/*    getDeviceProxy(new GetConnectedDeviceCallbackJni.GetConnectedDeviceCallback() {
      @Override
      public void onDeviceConnected(long deviceProxyPtr) {
        future.complete(deviceProxyPtr);
      }

      @Override
      public void onConnectionFailure(long nodeId, Exception error) {
        future.completeExceptionally(error);
      }
    });*/
    return future;
  }

  public void testGetCluster()
  {
    System.out.println("CHIPController loading");
    System.loadLibrary("CHIPController");
    System.out.println("CHIPController loaded");


    getDeviceProxy(new SuccessCallback<Long>() {
      @Override
      public void handle(Long deviceProxyPtr) {

        ChipClusters.ContentLauncherCluster cluster = new ChipClusters.ContentLauncherCluster(deviceProxyPtr, getId());
        Log.d(TAG, "Content launcher cluster created " + cluster);
        cluster.launchURL(new ChipClusters.ContentLauncherCluster.LauncherResponseCallback() {
                            @Override
                            public void onSuccess(Integer status, Optional<String> data) {
                              Log.d(TAG, "Content launcher success " + status + data);
                            }

                            @Override
                            public void onError(Exception error) {
                              Log.e(TAG, "Content launcher failure " + error);
                            }
                          }
                ,"my test url",
                Optional.of("my display str"),
                Optional.empty()
        );
      }
    }, new FailureCallback() {
      @Override
      public void handle(MatterError err) {
        Log.e(TAG, "getDeviceProxy err" + err);
      }
    });

/*    CompletableFuture<Long> deviceProxyFuture = getDeviceProxy();
    Log.d(TAG, "getDeviceProxy returned CompletableFuture");
    try {
      Long deviceProxy = deviceProxyFuture.get(5, TimeUnit.SECONDS);
      Log.d(TAG, "getDeviceProxy returned value " + deviceProxy);



    } catch (ExecutionException | InterruptedException | TimeoutException e) {
      Log.e(TAG, "getDeviceProxy exception" + e);
    }*/
    Log.d(TAG, "getDeviceProxy ending");
  }

  @Override
  public native <T extends Cluster> T getCluster(Class<T> clusterClass);

  @Override
  public boolean hasCluster(Class<? extends Cluster> clusterClass) {
    return getCluster(clusterClass) != null;
  }

  @Override
  public String toString() {
    return "MatterEndpoint{" + "id=" + getId() + '}';
  }

  @Override
  public boolean equals(Object o) {
    if (this == o) return true;
    if (o == null || getClass() != o.getClass()) return false;
    MatterEndpoint that = (MatterEndpoint) o;
    return getId() == that.getId();
  }

  @Override
  public int hashCode() {
    return Objects.hash(getId());
  }
}
