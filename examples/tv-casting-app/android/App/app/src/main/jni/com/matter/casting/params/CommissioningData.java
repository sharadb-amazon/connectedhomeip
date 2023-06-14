package com.matter.casting.params;

import androidx.annotation.Nullable;

public class CommissioningData {
  private int setupPasscode;

  private int discriminator;

  @Nullable private String spake2pVerifierBase64;

  @Nullable private String spake2pSaltBase64;

  private int spake2pIterationCount;

  public int getSetupPasscode() {
    return setupPasscode;
  }

  public void setSetupPasscode(int setupPasscode) {
    this.setupPasscode = setupPasscode;
  }

  public int getDiscriminator() {
    return discriminator;
  }

  public void setDiscriminator(int discriminator) {
    this.discriminator = discriminator;
  }

  @Nullable
  public String getSpake2pVerifierBase64() {
    return spake2pVerifierBase64;
  }

  public void setSpake2pVerifierBase64(@Nullable String spake2pVerifierBase64) {
    this.spake2pVerifierBase64 = spake2pVerifierBase64;
  }

  @Nullable
  public String getSpake2pSaltBase64() {
    return spake2pSaltBase64;
  }

  public void setSpake2pSaltBase64(@Nullable String spake2pSaltBase64) {
    this.spake2pSaltBase64 = spake2pSaltBase64;
  }

  public int getSpake2pIterationCount() {
    return spake2pIterationCount;
  }

  public void setSpake2pIterationCount(int spake2pIterationCount) {
    this.spake2pIterationCount = spake2pIterationCount;
  }
}
