package com.matter.casting.params;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

public abstract class DeviceAttestationCredentials {
  @Nullable private byte[] certificationDeclaration;

  @Nullable private byte[] firmwareInformation;

  @NonNull private byte[] deviceAttestationCert;

  @NonNull private byte[] productAttestationIntermediateCert;

  @NonNull
  public byte[] getCertificationDeclaration() {
    return certificationDeclaration;
  }

  public void setCertificationDeclaration(@NonNull byte[] certificationDeclaration) {
    this.certificationDeclaration = certificationDeclaration;
  }

  @NonNull
  public byte[] getFirmwareInformation() {
    return firmwareInformation;
  }

  public void setFirmwareInformation(@NonNull byte[] firmwareInformation) {
    this.firmwareInformation = firmwareInformation;
  }

  @NonNull
  public byte[] getDeviceAttestationCert() {
    return deviceAttestationCert;
  }

  public void setDeviceAttestationCert(@NonNull byte[] deviceAttestationCert) {
    this.deviceAttestationCert = deviceAttestationCert;
  }

  @NonNull
  public byte[] getProductAttestationIntermediateCert() {
    return productAttestationIntermediateCert;
  }

  public void setProductAttestationIntermediateCert(
      @NonNull byte[] productAttestationIntermediateCert) {
    this.productAttestationIntermediateCert = productAttestationIntermediateCert;
  }

  /**
   * Sign a mesage with the device attestation key.
   *
   * <p>The signature should be a SHA256withECDSA Signature that's returned in the ECDSA X9.62 Asn1
   * format. This is the default behavior when using java.security.Signature with an EC P-256 curve.
   *
   * @param message The message to sign
   * @return The signature in ECDSA X9.62 Asn1 format.
   */
  public abstract byte[] SignWithDeviceAttestationKey(byte[] message);
}
