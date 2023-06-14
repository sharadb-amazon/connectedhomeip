package com.matter.casting;

import android.util.Base64;
import android.util.Log;

import com.matter.casting.core.CastingApp;
import com.matter.casting.err.MatterError;
import com.matter.casting.params.AppParameters;
import com.matter.casting.params.CommissioningData;
import com.matter.casting.params.DataProvider;
import com.matter.casting.params.DeviceAttestationCredentials;

import java.math.BigInteger;
import java.security.AlgorithmParameters;
import java.security.KeyFactory;
import java.security.PrivateKey;
import java.security.Signature;
import java.security.spec.ECGenParameterSpec;
import java.security.spec.ECParameterSpec;
import java.security.spec.ECPrivateKeySpec;

public class InitializationExample {
    private final static String TAG = InitializationExample.class.getSimpleName();

    /**
     * DataProvider implementation for the Unique ID that is used by the SDK to generate the Rotating Device ID
     */
    private final static DataProvider<byte[]> rotatingDeviceIdUniqueIdProvider = new DataProvider<byte[]>() {
        private static final String APP_ID = "EXAMPLE_APP_ID";
        @Override
        public byte[] get() {
            return APP_ID.getBytes();
        }
    };

    /**
     * DataProvider implementation for the Commissioning Data used by the SDK when the CastingApp goes through commissioning
     */
    private final static DataProvider<CommissioningData> commissioningDataProvider = () -> {
        CommissioningData commissioningData = new CommissioningData();
        commissioningData.setSetupPasscode(20202021);
        commissioningData.setDiscriminator(3874);
        return commissioningData;
    };

    /**
     * DataProvider implementation for the Device Attestation Credentials required at the time of commissioning
     */
    private final static DataProvider<DeviceAttestationCredentials> dacProvider = new DataProvider<DeviceAttestationCredentials>() {
        private static final String kDevelopmentDAC_Cert_FFF1_8001 =
                "MIIB5zCCAY6gAwIBAgIIac3xDenlTtEwCgYIKoZIzj0EAwIwPTElMCMGA1UEAwwcTWF0dGVyIERldiBQQUkgMHhGRkYxIG5vIFBJRDEUMBIGCisGAQQBgqJ8AgEMBEZGRjEwIBcNMjIwMjA1MDAwMDAwWhgPOTk5OTEyMzEyMzU5NTlaMFMxJTAjBgNVBAMMHE1hdHRlciBEZXYgREFDIDB4RkZGMS8weDgwMDExFDASBgorBgEEAYKifAIBDARGRkYxMRQwEgYKKwYBBAGConwCAgwEODAwMTBZMBMGByqGSM49AgEGCCqGSM49AwEHA0IABEY6xpNCkQoOVYj8b/Vrtj5i7M7LFI99TrA+5VJgFBV2fRalxmP3k+SRIyYLgpenzX58/HsxaznZjpDSk3dzjoKjYDBeMAwGA1UdEwEB/wQCMAAwDgYDVR0PAQH/BAQDAgeAMB0GA1UdDgQWBBSI3eezADgpMs/3NMBGJIEPRBaKbzAfBgNVHSMEGDAWgBRjVA5H9kscONE4hKRi0WwZXY/7PDAKBggqhkjOPQQDAgNHADBEAiABJ6J7S0RhDuL83E0reIVWNmC8D3bxchntagjfsrPBzQIga1ngr0Xz6yqFuRnTVzFSjGAoxBUjlUXhCOTlTnCXE1M=";

        private static final String kDevelopmentDAC_PrivateKey_FFF1_8001 =
                "qrYAroroqrfXNifCF7fCBHCcppRq9fL3UwgzpStE+/8=";

        private static final String KPAI_FFF1_8000_Cert_Array =
                "MIIByzCCAXGgAwIBAgIIVq2CIq2UW2QwCgYIKoZIzj0EAwIwMDEYMBYGA1UEAwwPTWF0dGVyIFRlc3QgUEFBMRQwEgYKKwYBBAGConwCAQwERkZGMTAgFw0yMjAyMDUwMDAwMDBaGA85OTk5MTIzMTIzNTk1OVowPTElMCMGA1UEAwwcTWF0dGVyIERldiBQQUkgMHhGRkYxIG5vIFBJRDEUMBIGCisGAQQBgqJ8AgEMBEZGRjEwWTATBgcqhkjOPQIBBggqhkjOPQMBBwNCAARBmpMVwhc+DIyHbQPM/JRIUmR/f+xeUIL0BZko7KiUxZQVEwmsYx5MsDOSr2hLC6+35ls7gWLC9Sv5MbjneqqCo2YwZDASBgNVHRMBAf8ECDAGAQH/AgEAMA4GA1UdDwEB/wQEAwIBBjAdBgNVHQ4EFgQUY1QOR/ZLHDjROISkYtFsGV2P+zwwHwYDVR0jBBgwFoAUav0idx9RH+y/FkGXZxDc3DGhcX4wCgYIKoZIzj0EAwIDSAAwRQIhALLvJ/Sa6bUPuR7qyUxNC9u415KcbLiPrOUpNo0SBUwMAiBlXckrhr2QmIKmxiF3uCXX0F7b58Ivn+pxIg5+pwP4kQ==";

        @Override
        public DeviceAttestationCredentials get() {
            DeviceAttestationCredentials deviceAttestationCredentials = new DeviceAttestationCredentials() {
                @Override
                public byte[] SignWithDeviceAttestationKey(byte[] message) {
                    try {
                        byte[] privateKeyBytes = Base64.decode(kDevelopmentDAC_PrivateKey_FFF1_8001, Base64.DEFAULT);

                        AlgorithmParameters algorithmParameters = AlgorithmParameters.getInstance("EC");
                        algorithmParameters.init(new ECGenParameterSpec("secp256r1"));
                        ECParameterSpec parameterSpec = algorithmParameters.getParameterSpec(ECParameterSpec.class);
                        ECPrivateKeySpec ecPrivateKeySpec =
                                new ECPrivateKeySpec(new BigInteger(1, privateKeyBytes), parameterSpec);

                        KeyFactory keyFactory = KeyFactory.getInstance("EC");
                        PrivateKey privateKey = keyFactory.generatePrivate(ecPrivateKeySpec);

                        Signature signature = Signature.getInstance("SHA256withECDSA");
                        signature.initSign(privateKey);

                        signature.update(message);

                        return signature.sign();

                    } catch (Exception e) {
                        return null;
                    }
                }
            };

            deviceAttestationCredentials.setDeviceAttestationCert(Base64.decode(kDevelopmentDAC_Cert_FFF1_8001, Base64.DEFAULT));
            deviceAttestationCredentials.setProductAttestationIntermediateCert(Base64.decode(KPAI_FFF1_8000_Cert_Array, Base64.DEFAULT));
            return deviceAttestationCredentials;
        }
    };

    public static void demoInitialization() {
        // Create an AppParameters object to pass in global casting parameters to the SDK
        final AppParameters appParameters = new AppParameters(rotatingDeviceIdUniqueIdProvider, commissioningDataProvider, dacProvider);

        // Initialize the SDK using the appParameters and check if it returns successfully
        MatterError err = CastingApp.initialize(appParameters);
        if(err.hasError()) {
            Log.e(TAG, "Failed to initialize Matter CastingApp");
        }
    }
}
