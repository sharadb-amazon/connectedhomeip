package com.matter.casting.core;

import com.matter.casting.err.MatterError;
import com.matter.casting.params.AppParameters;
import com.matter.casting.params.CommissioningData;
import com.matter.casting.params.DataProvider;
import com.matter.casting.params.DeviceAttestationCredentials;

public class CastingApp {
  public static MatterError initialize(
      AppParameters appParameters,
      DataProvider<byte[]> rotatingDeviceIdUniqueIdProvider,
      DataProvider<CommissioningData> commissioningDataProvider,
      DataProvider<DeviceAttestationCredentials> dacProvider) {
    return null;
  }

  // called after initialize
  public static MatterError start() {
    return null;
  }

  public static MatterError stop() {
    return null;
  }
}
