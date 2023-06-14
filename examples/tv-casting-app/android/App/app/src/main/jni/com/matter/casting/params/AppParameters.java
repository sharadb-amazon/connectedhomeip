package com.matter.casting.params;

import androidx.annotation.NonNull;

public class AppParameters {
  @NonNull private final DataProvider<byte[]> rotatingDeviceIdUniqueIdProvider;

  @NonNull private final DataProvider<CommissioningData> commissioningDataProvider;

  @NonNull private final DataProvider<DeviceAttestationCredentials> dacProvider;

  public AppParameters(
      @NonNull DataProvider<byte[]> rotatingDeviceIdUniqueIdProvider,
      @NonNull DataProvider<CommissioningData> commissioningDataProvider,
      @NonNull DataProvider<DeviceAttestationCredentials> dacProvider) {
    this.rotatingDeviceIdUniqueIdProvider = rotatingDeviceIdUniqueIdProvider;
    this.commissioningDataProvider = commissioningDataProvider;
    this.dacProvider = dacProvider;
  }

  @NonNull
  public DataProvider<byte[]> getRotatingDeviceIdUniqueIdProvider() {
    return rotatingDeviceIdUniqueIdProvider;
  }

  @NonNull
  public DataProvider<CommissioningData> getCommissioningDataProvider() {
    return commissioningDataProvider;
  }

  @NonNull
  public DataProvider<DeviceAttestationCredentials> getDacProvider() {
    return dacProvider;
  }
}
