
# Matter Casting APIs

  

The Matter SDK can be used by a Matter Casting client to cast content to a Matter Casting server. The [example Matter tv-casting-app](https://github.com/project-chip/connectedhomeip/tree/master/examples/tv-casting-app) for Android / iOS and Linux builds on top of the Matter SDK to demonstrate this capability. This document describes how the Casting APIs built and tested with the example Matter tv-casting-app can be consumed by any Android/iOS casting app. 
<a word about the refactor>
## Introduction
The Matter Casting APIs are consumed by a Casting client like an Android / iOS app that may want to cast videos, music, pictures or some other content on a `CastingPlayer`. The Casting client is expected to be a Matter Commissionable Node and a `CastingPlayer` is expected to be a Matter Commissioner. In the context of the [Matter Video Player architecture](https://github.com/CHIP-Specifications/connectedhomeip-spec/blob/master/src/app_clusters/media/VideoPlayerArchitecture.adoc), a `CastingPlayer` would map to [Casting "Video" Player](https://github.com/CHIP-Specifications/connectedhomeip-spec/blob/master/src/app_clusters/media/VideoPlayerArchitecture.adoc#1-introduction). The `CastingPlayer` is expected to be hosting one or more `Endpoints` (similar to [Content Apps](https://github.com/CHIP-Specifications/connectedhomeip-spec/blob/master/src/app_clusters/media/VideoPlayerArchitecture.adoc#1-introduction) in the Matter Video Player architecture) that support one or more Matter Media `Clusters`. 
The Casting client can use the Matter Casting APIs to **discover** CastingPlayers, **connect** to them and **interact** with one or more of their Endpoints i.e send Cluster commands and read / subscribe to attributes, to realize an End-to-End Casting experience.

## Build and Setup
The Casting Client is expected to consume the Matter TV Casting library built for its respective platform to consume any of the APIs described in this document. Refer to the tv-casting-app READMEs for Android and iOS to understand how the libraries can be built and consumed.

## Initialization
The Casting Client must first initialize the Matter TV Casting library and provide values for global AppParameters and a few DataProviders that are required through the lifecycle of the client.

<details>
  <summary>Android example</summary>
  
  ```java
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
	  final AppParameters appParameters = new AppParameters(rotatingDeviceIdUniqueIdProvider, 	
												  commissioningDataProvider, dacProvider);  
  
	  // Initialize the SDK using the appParameters and check if it returns successfully  
	  MatterError err = CastingApp.initialize(appParameters);  
	  if(err.hasError()) {  
            Log.e(TAG, "Failed to initialize Matter CastingApp");  
	  }  
    }  
}
```
</details>

<details>
  <summary>iOS example</summary>
  
  ```objectivec
      enter code here   
  ```
  </details>

## Discovery
The Casting Client can then start discovery of `CastingPlayers` using the discovery APIs as shown below. This is implemented based on Matter Commissioner discovery over DNS-SD. The client can listen for CastingPlayers as they are discovered, updated or lost from the network.

<details>
  <summary>Android example</summary>
  
  ```java
import android.util.Log;  
  
import com.matter.casting.core.CastingPlayer;  
import com.matter.casting.core.CastingPlayerDiscoverer;  
import com.matter.casting.core.CastingPlayerDiscoverer.CastingPlayerChangeListener;  
import com.matter.casting.core.MatterCastingPlayerDiscoverer;  
  
import java.util.List;  
import java.util.concurrent.Executors;  
import java.util.concurrent.ScheduledExecutorService;  
import java.util.concurrent.TimeUnit;  
  
public class DiscoveryExample {  
    private static final ScheduledExecutorService executorService = Executors.newSingleThreadScheduledExecutor();  
  
  /**  
 * Implementation of a CastingPlayerChangeListener used to listen to changes in the discovered CastingPlayers */  private static final CastingPlayerChangeListener castingPlayerChangeListener = new CastingPlayerChangeListener() {  
        private final String TAG = CastingPlayerChangeListener.class.getSimpleName();  
  
  @Override  
  public void onChanged(List<CastingPlayer> castingPlayers) {  
            Log.i(TAG, "Discovered changes to " + castingPlayers.size() + " CastingPlayers");  
  // consume changes to the provided castingPlayers  
  }  
  
        @Override  
  public void onAdded(List<CastingPlayer> castingPlayers) {  
            Log.i(TAG, "Discovered " + castingPlayers.size() + " CastingPlayers");  
  // consume discovered castingPlayers  
  }  
  
        @Override  
  public void onRemoved(List<CastingPlayer> castingPlayers) {  
            Log.i(TAG, "Removed " + castingPlayers.size() + " CastingPlayers");  
  // consume castingPlayers removed or lost from the network  
  }  
    };  
  
 public static void demoDiscovery()  
    {  
        // Get the singleton instance of the MatterCastingPlayerDiscoverer  
  CastingPlayerDiscoverer discoverer = MatterCastingPlayerDiscoverer.getInstance();  
  
  // Add our castingPlayerChangeListener to listen to changes in the discovered CastingPlayers  
  discoverer.addCastingPlayerChangeListener(castingPlayerChangeListener);  
  
  // Start discovery  
  discoverer.startDiscovery();  
  
  // After some time, stop discovering and remove our castingPlayerChangeListener from the set of listeners the CastingPlayerDiscoverer informs  
  executorService.schedule(() -> {  
            discoverer.stopDiscovery();  
  discoverer.removeCastingPlayerChangeListener(castingPlayerChangeListener);  
  }, 30, TimeUnit.SECONDS);  
  }  
}
```
</details>

<details>
  <summary>iOS example</summary>
  
  ```objectivec
      enter code here   
  ```
  </details>

Note: at this point, the list of Endpoints in the discovered CastingPlayers will be empty, as this information is not revealed at the time of Matter Commissioner discovery.
## Connection
After discovery, the Casting client may use user input to select on of the CastingPlayers to connect it. Each CastingPlayer object contains information such as deviceName, vendorId, productId, etc. to help make this selection. It can then attempt to connect to the selectedCastingPlayer. This is implemented based on Matter User Directed Commissioning (UDC). The Matter TV Casting library also locally caches information required to re-connect to a CastingPlayer. Once it does that, it is able to skip UDC and establishes CASE directly.

<details>
  <summary>Android example</summary>
  
  ```java
import android.util.Log;  
  
import com.matter.casting.core.CastingPlayer;  
  
import java.util.concurrent.CompletableFuture;  
  
public class ConnectionExample {  
    private final static String TAG = ConnectionExample.class.getSimpleName();  
  
  // Maximum time in seconds we'll wait for the connection to the CastingPlayer to go through  
  private static final long CONNECTION_TIMEOUT_SECS = 45;  
  
 public static void demoConnection(CastingPlayer castingPlayer) {  
        // Connect to the castingPlayer  
  CompletableFuture<Void> connectionFuture = castingPlayer.connect(CONNECTION_TIMEOUT_SECS);  
  
  // handle exception, if any  
  connectionFuture.exceptionally(exc -> {  
            Log.e(TAG, "Exception in connecting to castingPlayer" + exc.getMessage());  
 return null;  });  
  }  
}
```
</details>

## Interactions
On a successful connection with the CastingPlayer, a Casting client may pick of of the Endpoints to interact with based on its attributes (like Vendor or Product IDs, list of supported Clusters, etc). Here is an example, where the client code selects an Endpoint on the CastingPlayer with VendorID ("12345") that supports the ContentLauncher cluster LaunchURL command.

On Android,

    enter code here

On iOS,

    enter code here

### Commands
The CastingClient may send a command to the selectedEndpoint on the selectedCastingPlayer and handle the command response, like so:

<details>
  <summary>Android example</summary>
  
  ```java
import android.util.Log;  
import com.matter.casting.clusters.ContentLauncher;  
import com.matter.casting.core.CastingPlayer;  
import com.matter.casting.core.Endpoint;  
import com.matter.casting.err.UnsupportedClusterException;  
import com.matter.casting.err.UnsupportedCommandException;  
import java.net.MalformedURLException;  
import java.net.URL;  
import java.util.Optional;  
import java.util.concurrent.CompletableFuture;  
  
public class CommandInvocationExample {  
  private static final String TAG = CommandInvocationExample.class.getSimpleName();  
  
 private static final String SAMPLE_CONTENT_APP_VID = "12345";  
  
 public static void demoCommandInvocation(CastingPlayer castingPlayer)  
      throws MalformedURLException, UnsupportedCommandException, UnsupportedClusterException {  
  
    // pick the Content app Endpoint to invoke the command on  
  Optional<Endpoint> contentAppEndpoint =  
        castingPlayer  
            .getEndpoints()  
            .stream()  
            .filter(endpoint -> SAMPLE_CONTENT_APP_VID.equals(endpoint.getVendorId()))  
            .findAny();  
  
 if (contentAppEndpoint.isPresent()) {  
      // check if the Endpoint supports the Cluster to use  
  if (contentAppEndpoint.get().hasCluster(ContentLauncher.class)) {  
        // get the Cluster to use  
  ContentLauncher contentLauncher =  
            contentAppEndpoint.get().getCluster(ContentLauncher.class);  
  
  // invoke the command  
  CompletableFuture<ContentLauncher.LauncherResponse> contentLauncherResponseFuture =  
            contentLauncher.launchURL(  
                new URL("https://www.samplemattercontent.xyz/id"), "displaystring", null);  
  contentLauncherResponseFuture.thenApply(  
            (response) -> {  
              // handle the response  
  return null;  
  });  
  }  
    } else {  
      Log.d(TAG, "Content app endpoint not found");  
  }  
  }  
}
```
</details>

<details>
  <summary>iOS example</summary>
  
  ```objectivec
      enter code here   
  ```
  </details>

### Reads
The CastingClient may read an Attribute from the selectedEndpoint on the selectedCastingPlayer, like so:

On Android,

    enter code here

On iOS,

    enter code here

### Subscriptions
The CastingClient may subscribe to an Attribute from the selectedEndpoint on the selectedCastingPlayer, like so:

On Android,

    enter code here

On iOS,

    enter code here

## Conclusion
As described above, the Casting APIs allows a Casting Client to discover CastingPlayers on a users network, connect to one of them and interact with various Endpoints on them using Matter commands, attribute reads and subscriptions.