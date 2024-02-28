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
//import com.matter.casting.support.FailureCallback;
//import com.matter.casting.support.SuccessCallback;

import java.util.List;
import java.util.Objects;
import java.util.concurrent.CompletableFuture;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.TimeoutException;

//import chip.devicecontroller.GetConnectedDeviceCallbackJni;

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

  /*protected native void getDeviceProxy(GetConnectedDeviceCallbackJni.GetConnectedDeviceCallback callback);

  protected CompletableFuture<Long> getDeviceProxy()
  {
    CompletableFuture<Long> future = new CompletableFuture<>();
    getDeviceProxy(new GetConnectedDeviceCallbackJni.GetConnectedDeviceCallback() {
      @Override
      public void onDeviceConnected(long deviceProxyPtr) {
        future.complete(deviceProxyPtr);
      }

      @Override
      public void onConnectionFailure(long nodeId, Exception error) {
        future.completeExceptionally(error);
      }
    });

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

    return future;
  }

  public void testGetCluster()
  {
    CompletableFuture<Long> deviceProxyFuture = getDeviceProxy();
    Log.d(TAG, "getDeviceProxy returned CompletableFuture");
    try {
      Long deviceProxy = deviceProxyFuture.get(5, TimeUnit.SECONDS);
      Log.d(TAG, "getDeviceProxy returned value " + deviceProxy);
    } catch (ExecutionException | InterruptedException | TimeoutException e) {
      Log.e(TAG, "getDeviceProxy exception" + e);
    }
    Log.d(TAG, "getDeviceProxy ending");
  }*/

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
