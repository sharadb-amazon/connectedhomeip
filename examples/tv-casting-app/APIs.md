
# Matter Casting APIs

  

The Matter SDK can be used by a Matter Casting client to cast content to a Matter Casting server. The [example Matter tv-casting-app](https://github.com/project-chip/connectedhomeip/tree/master/examples/tv-casting-app) for Android / iOS and Linux builds on top of the Matter SDK to demonstrate this capability. This document describes how the Casting APIs built and tested with the example Matter tv-casting-app can be consumed by any Android/iOS casting app. 

## Introduction
The Matter Casting APIs are consumed by a Casting client like an Android / iOS app that may want to cast videos, music, pictures or some other content on a `CastingPlayer`. The Casting client is expected to be a Matter Commissionable Node and a `CastingPlayer` is expected to be a Matter Commissioner. In the context of the [Matter Video Player architecture](https://github.com/CHIP-Specifications/connectedhomeip-spec/blob/master/src/app_clusters/media/VideoPlayerArchitecture.adoc), a `CastingPlayer` would map to [Casting "Video" Player](https://github.com/CHIP-Specifications/connectedhomeip-spec/blob/master/src/app_clusters/media/VideoPlayerArchitecture.adoc#1-introduction). The `CastingPlayer` is expected to be hosting one or more `Endpoints` (similar to [Content Apps](https://github.com/CHIP-Specifications/connectedhomeip-spec/blob/master/src/app_clusters/media/VideoPlayerArchitecture.adoc#1-introduction) in the Matter Video Player architecture) that support one or more Matter Media `Clusters`. 
The Casting client can use the Matter Casting APIs to **discover** CastingPlayers, **connect** to them and **interact** with one or more of their Endpoints i.e send Cluster commands and read / subscribe to attributes, to realize an End-to-End Casting experience.

## Build and Setup
The Casting Client is expected to consume the Matter TV Casting library built for its respective platform to consume any of the APIs described in this document. Refer to the tv-casting-app READMEs for Android and iOS to understand how the libraries can be built and consumed.

## Initialization
The Casting Client must first initialize the Matter TV Casting library and provide values for a few `DataProviders` that are required through the lifecycle of the client:
1. A Unique ID used to generate the Rotating Device ID - The generated Rotating Device ID is used to uniquely identify the Casting Client before it has been commissioned and is advertised as part of the DNS-SD based Commissionable Node discovery. ([See spec - Rotating Device ID](https://github.com/CHIP-Specifications/connectedhomeip-spec/blob/master/src/rendezvous/DeviceDiscovery.adoc#245-rotating-device-identifier))
    
    On Android, define a `rotatingDeviceIdUniqueIdProvider` to provide the Casting Client's RotatingDeviceIdUniqueId, by implementing a `DataSource`:
    ```java
    private final static DataProvider<byte[]> rotatingDeviceIdUniqueIdProvider = new DataProvider<byte[]>() {  
        private static final String APP_ID = "EXAMPLE_APP_ID";  
        @Override  
        public byte[] get() {  
            return APP_ID.getBytes();  
        }
    }
    ```
    On iOS, define the `func castingAppDidReceiveRequestForRotatingDeviceIdUniqueId` in a class, `AppParametersDataSource`, that implements the `MTRDataSource`:
    ```objectivec
    class AppParametersDataSource : NSObject, MTRDataSource
    {
        func castingAppDidReceiveRequestForRotatingDeviceIdUniqueId(_ sender: Any) -> Data {
            return "EXAMPLE_APP_ID".data(using: .utf8)!
        }
        ...
    }
    ```

2. CommissioningData - This contains the passcode, discriminator, etc) which is required to verify an incoming commissioning request. ([See spec - Onboarding Payload ID](https://github.com/CHIP-Specifications/connectedhomeip-spec/blob/master/src/qr_code/OnboardingPayload.adoc#ref_OnboardingPayload))
    
    On Android, define a `commissioningDataProvider` that can provide the required values to the CastingApp.
    ```java
    private final static DataProvider<CommissioningData> commissioningDataProvider = () -> {  
	    CommissioningData commissioningData = new CommissioningData();  
        commissioningData.setSetupPasscode(20202021);  
        commissioningData.setDiscriminator(3874);  
        return commissioningData;  
    }; 
    ```
    On iOS, add a `func commissioningDataProvider` to the `AppParametersDataSource` class defined above, that can provide the required values to the `MTRCastingApp`.
    ```objectivec
    func castingAppDidReceiveRequestForCommissioningData(_ sender: Any) -> MTRCommissioningData {
        return MTRCommissioningData(passcode: 20202021, discriminator: 3874, spake2pIterationCount: 1000, spake2pVerifier: nil, spake2pSalt: nil)
    }
    ```

3. DeviceAttestationCredentials - This contains the DeviceAttestationCertificate, ProductAttestationIntermediateCertificate, etc. and implements a way to sign messages when called upon by the Matter TV Casting Library as part of the Device Attestation process during commissoning. ([See spec - Device Attestation](https://github.com/CHIP-Specifications/connectedhomeip-spec/blob/master/src/device_attestation/Device_Attestation_Specification.adoc)) 

    On Android, define a `dacProvider` to provide the Casting Client's Device Attestation Credentials, by implementing a `DataSource`:
    ```java
    private final static DataProvider<DeviceAttestationCredentials> dacProvider = new DataProvider<DeviceAttestationCredentials>() {  
        private static final String kDevelopmentDAC_Cert_FFF1_8001 = "MIIB5z...<snipped>...CXE1M=";  
        private static final String kDevelopmentDAC_PrivateKey_FFF1_8001 = "qrYAror...<snipped>...StE+/8=";  
        private static final String KPAI_FFF1_8000_Cert_Array = "MIIByzC...<snipped>...pwP4kQ==";  
        
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
    ```
    
    On iOS, add a `func didReceiveRequestToSignCertificateRequest` to the `AppParametersDataSource` class defined above, that can sign messages for the Casting Client.
    ```objectivec
    func castingApp(_ sender: Any, didReceiveRequestToSignCertificateRequest csrData: Data) async -> Data {
        // sign the message and return the value
        return Data()
    }
    ```
    
Finally, use the parameters defined above to initialize the CastingApp. Make sure this call is made from a place in the Casting client that would be called only once, before it starts doing Matter casting.

On Android, create an AppParameters object using the `rotatingDeviceIdUniqueIdProvider`, `commissioningDataProvider` and `dacProvider`, and call `CastingApp.initialize` with it. 
```java
public static void demoInitialization() 
{  
    // Create an AppParameters object to pass in global casting parameters to the SDK  
    final AppParameters appParameters = new AppParameters(rotatingDeviceIdUniqueIdProvider, 	
												  commissioningDataProvider, dacProvider);  
  
    // Initialize the SDK using the appParameters and check if it returns successfully  
    MatterError err = CastingApp.initialize(appParameters);  
    if(err.hasError()) {  
        Log.e(TAG, "Failed to initialize Matter CastingApp");  
    }  
}  
```

On iOS, call `MTRCastingApp.initialize` with an object of the `AppParametersDataSource`.  
```objectivec
init()
{
    ...
    MTRCastingApp.initialize(with: AppParametersDataSource())
}
```

## Discovery
The Casting Client can then start discovery of `CastingPlayers` using the discovery APIs as shown below. This is implemented based on Matter Commissioner discovery over DNS-SD. The client can listen for CastingPlayers as they are discovered, updated or lost from the network.

First, define the necessary listeners that the Casting Client will be called on, when a CastingPlayer is added / removed / updated in the list of discovered CastingPlayers.

On Android, define a concrete anonymous `class castingPlayerChangeListener` that implements the CastingPlayerChangeListener.
```java
private static final CastingPlayerChangeListener castingPlayerChangeListener = new CastingPlayerChangeListener() 
{  
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
```
On iOS, implement a `func addDiscoveredCastingPlayers`, `func removeDiscoveredCastingPlayers` and `func updateDiscoveredCastingPlayers` which will listen to Notifications when discovered CastingPlayers are added/removed/updated 
```objectivec
@objc
func addDiscoveredCastingPlayers(notification: Notification)
{
    guard let userInfo = notification.userInfo,
            let castingPlayers     = userInfo[castingPlayersUserInfo] as? [MTRCastingPlayer] else {
            print("No MTRCastingPlayer found in notification")
            return
        }
    displayedCastingPlayers.append(contentsOf: castingPlayers)
}
    
@objc
func removeDiscoveredCastingPlayers(notification: Notification)
{
    guard let userInfo = notification.userInfo,
            let castingPlayers     = userInfo[castingPlayersUserInfo] as? [MTRCastingPlayer] else {
        print("No MTRCastingPlayer found in notification")
        return
    }
    displayedCastingPlayers = displayedCastingPlayers.filter { !castingPlayers.contains($0)}
}
    
@objc
func updateDiscoveredCastingPlayers(notification: Notification)
{
    guard let userInfo = notification.userInfo,
            let castingPlayers     = userInfo[castingPlayersUserInfo] as? [MTRCastingPlayer] else {
        print("No MTRCastingPlayer found in notification")
        return
    }
    for castingPlayer in castingPlayers
    {
        if let index = displayedCastingPlayers.firstIndex(where: { castingPlayer.identifier == $0.identifier })
        {
            displayedCastingPlayers[index] = castingPlayer
        }
    }
}
```

Finally, register these listeners and start discovery.

On Android, call `addCastingPlayerChangeListener` on the singleton instance of `MatterCastingPlayerDiscoverer` to register the listener, and then call `startDiscovery`.
```java
public static void demoDiscovery()  
{  
    // Get the singleton instance of the MatterCastingPlayerDiscoverer  
    CastingPlayerDiscoverer discoverer = MatterCastingPlayerDiscoverer.getInstance();  
  
    // Add our castingPlayerChangeListener to listen to changes in the discovered CastingPlayers  
    discoverer.addCastingPlayerChangeListener(castingPlayerChangeListener);  
  
    // Start discovery  
    discoverer.startDiscovery();  
  
    // After some time, stop discovering and remove our castingPlayerChangeListener 
    // from the set of listeners the CastingPlayerDiscoverer informs  
    executorService.schedule(() -> {  
            discoverer.stopDiscovery();  
    discoverer.removeCastingPlayerChangeListener(castingPlayerChangeListener);  
    },     30, TimeUnit.SECONDS);  
}  
```

On iOS, register the listeners by calling `addObserver` on the `NotificationCenter` with the appropriate selector, and then call `start` on the `sharedInstance` of `MTRCastingPlayerDiscovery`.
```objectivec
func startDiscovery() {
        NotificationCenter.default.addObserver(self, selector: #selector(self.addDiscoveredCastingPlayers), name: NSNotification.Name.didAddCastingPlayers, object: nil)
        NotificationCenter.default.addObserver(self, selector: #selector(self.removeDiscoveredCastingPlayers), name: NSNotification.Name.didRemoveCastingPlayers, object: nil)
        NotificationCenter.default.addObserver(self, selector: #selector(self.updateDiscoveredCastingPlayers), name: NSNotification.Name.didUpdateCastingPlayers, object: nil)

        MTRCastingPlayerDiscovery.sharedInstance().start()
        self.discoveryRequestStatus = true
    }
```

Note: at this point, the list of Endpoints in the discovered CastingPlayers will be empty, as this information is not revealed at the time of Matter Commissioner discovery.
## Connection
After discovery, the Casting client may use user input to select on of the CastingPlayers to connect it. Each CastingPlayer object contains information such as deviceName, vendorId, productId, etc. to help make this selection. It can then attempt to connect to the selectedCastingPlayer. This is implemented based on Matter User Directed Commissioning (UDC). The Matter TV Casting library also locally caches information required to re-connect to a CastingPlayer. Once it does that, it is able to skip UDC and establishes CASE directly. Once connected, the CastingPlayer object would contain the list of available Endpoints on it.

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

<details>
  <summary>iOS example</summary>
  
  ```objectivec
  import Foundation
import os.log

class MTRConnectionExampleViewModel: ObservableObject {
    let Log = Logger(subsystem: "com.matter.casting",
                     category: "MTRConnectionExampleViewModel")
        
    @Published var connectionSuccess: Bool?;

    @Published var connectionStatus: String?;

    func connect(selectedCastingPlayer: MTRCastingPlayer?) {
        selectedCastingPlayer?.connect(withTimeout: 60, withCompletionBlock: { err in
            self.Log.error("MTRConnectionExampleViewModel connect() completed with \(err)")
            if(err == nil)
            {
                self.connectionSuccess = true
                self.connectionStatus = "Connected!"
            }
            else
            {
                self.connectionSuccess = false
                self.connectionStatus = "Connection failed with \(String(describing: err))"
            }
        })
    }
}
   
  ```
  </details>

## Interactions
On a successful connection with the CastingPlayer, a Casting client may pick of of the Endpoints to interact with based on its attributes (like Vendor or Product IDs, list of supported Clusters, etc). It may then send commands to it, or subscribe to / read attribute values from it.

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
  import Foundation
import os.log

class MTRCommandInvocationExampleViewModel: ObservableObject {
    let Log = Logger(subsystem: "com.matter.casting",
                     category: "MTRCommandInvocationExampleViewModel")
    
    let sampleContentAppVid: String = "12345"
    
    @Published var status: String?;
    
    func invokeCommand(castingPlayer: MTRCastingPlayer)
    {
        if let endpoint: MTREndpoint = castingPlayer.endpoints.filter({ $0.identifier == sampleContentAppVid }).first
        {
            if(endpoint.hasCluster(MTREndpointClusterTypeContentLauncher))
            {
                let cluster: MTRContentLauncherCluster = endpoint.cluster(for: MTREndpointClusterTypeContentLauncher) as! MTRContentLauncherCluster
                if(cluster.canLaunchURL())
                {
                    cluster.launch(URL(string: "https://www.samplemattercontent.xyz/id")!) { launchResponse, err in
                        self.Log.info("invokeCommand completed with \(err)")
                        self.status = "ContentLauncher.LaunchURL completed!"

                        // consume the launchResponse
                    }
                }
                else
                {
                    self.status = "Cannot launchURL on cluster"
                }
            }
            else
            {
                self.status = "No ContentLauncher cluster supporting endpoint found"
            }
        }
        else
        {
            self.status = "No endpoint matching the example VID found"
        }
    }
}     
  ```
  </details>

### Reads
The CastingClient may read an Attribute from the selectedEndpoint on the selectedCastingPlayer, like so:

<details>
  <summary>Android example</summary>
  
  ```java
  package com.matter.casting;

import android.util.Log;

import com.matter.casting.clusters.MediaPlayback;
import com.matter.casting.core.Attribute;
import com.matter.casting.core.CastingPlayer;
import com.matter.casting.core.Endpoint;
import com.matter.casting.err.UnsupportedClusterException;

import java.util.Optional;
import java.util.concurrent.CompletableFuture;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.TimeoutException;

public class AttributeReadExample {
    private static final String TAG = AttributeReadExample.class.getSimpleName();

    private static final String SAMPLE_CONTENT_APP_VID = "12345";

    public static void demoRead(CastingPlayer castingPlayer) throws UnsupportedClusterException {

        // pick the Content app Endpoint to invoke the command on
        Optional<Endpoint> contentAppEndpoint =
                castingPlayer
                        .getEndpoints()
                        .stream()
                        .filter(endpoint -> SAMPLE_CONTENT_APP_VID.equals(endpoint.getVendorId()))
                        .findAny();

        if (contentAppEndpoint.isPresent()) {
            // check if the Endpoint supports the Cluster to use
            if (contentAppEndpoint.get().hasCluster(MediaPlayback.class)) {
                // get the Cluster to use
                MediaPlayback mediaPlayback =
                        contentAppEndpoint.get().getCluster(MediaPlayback.class);

                Optional<Attribute<MediaPlayback.CurrentState>> currentStateAttribute = mediaPlayback.getCurrentState();
                if(currentStateAttribute.isPresent() && currentStateAttribute.get().isAvailable())
                {
                    CompletableFuture<MediaPlayback.CurrentState> currentStateValue = currentStateAttribute.get().read();
                    try {
                        Log.i(TAG, "Read Current State value: " + currentStateValue.get(1, TimeUnit.SECONDS));

                    } catch (ExecutionException | InterruptedException | TimeoutException e) {
                        Log.e(TAG, "Exception when reading CurrentState " + e);
                    }
                }
                else
                {
                    Log.e(TAG, "Attribute unavailable on the selected endpoint");
                }
            }
            else
            {
                Log.e(TAG, "Required cluster not found on the selected endpoint");
            }
        } else {
            Log.e(TAG, "Content app endpoint not found");
        }
    }
}
  ```
</details>

<details>
  <summary>iOS example</summary>
  
  ```objectivec
  import Foundation
import os.log

class MTRAttributeReadExampleViewModel: ObservableObject {
    let Log = Logger(subsystem: "com.matter.casting",
                     category: "MTRAttributeReadExampleViewModel")
    
    let sampleContentAppVid: String = "12345"
    
    @Published var status: String?;
    
    func readAttribute(castingPlayer: MTRCastingPlayer)
    {
        if let endpoint: MTREndpoint = castingPlayer.endpoints.filter({ $0.identifier == sampleContentAppVid }).first
        {
            if(endpoint.hasCluster(MTREndpointClusterTypeContentLauncher))
            {
                let cluster: MTRMediaPlaybackCluster = endpoint.cluster(for: MTREndpointClusterTypeMediaPlayback) as! MTRMediaPlaybackCluster
                let currentStateAttribute: MTRAttribute<MTRCurrentState> = cluster.currentState
                
                currentStateAttribute.read { currentStateValue, err in
                    if(err == nil)
                    {
                        self.status = "Read CurrentState value \(String(describing: currentStateValue))"
                    }
                    else
                    {
                        self.status = "Error when reading CurrentState value \(String(describing: err))"
                    }
                }
            }
            else
            {
                self.status = "No MediaPlayback cluster supporting endpoint found"
            }
        }
        else
        {
            self.status = "No endpoint matching the example VID found"
        }
    }
}
  ```
</details>

### Subscriptions
The CastingClient may subscribe to an Attribute from the selectedEndpoint on the selectedCastingPlayer, like so:

<details>
  <summary>Android example</summary>
  
  ```java
  package com.matter.casting;

import android.util.Log;

import com.matter.casting.clusters.MediaPlayback;
import com.matter.casting.core.Attribute;
import com.matter.casting.core.CastingPlayer;
import com.matter.casting.core.Endpoint;
import com.matter.casting.err.UnsupportedClusterException;

import java.util.Optional;

public class AttributeSubscriptionExample {
    private static final String TAG = AttributeSubscriptionExample.class.getSimpleName();

    private static final String SAMPLE_CONTENT_APP_VID = "12345";

    public static void demoSubscription(CastingPlayer castingPlayer) throws UnsupportedClusterException {

        // pick the Content app Endpoint to invoke the command on
        Optional<Endpoint> contentAppEndpoint =
                castingPlayer
                        .getEndpoints()
                        .stream()
                        .filter(endpoint -> SAMPLE_CONTENT_APP_VID.equals(endpoint.getVendorId()))
                        .findAny();

        if (contentAppEndpoint.isPresent()) {
            // check if the Endpoint supports the Cluster to use
            if (contentAppEndpoint.get().hasCluster(MediaPlayback.class)) {
                // get the Cluster to use
                MediaPlayback mediaPlayback =
                        contentAppEndpoint.get().getCluster(MediaPlayback.class);

                Optional<Attribute<MediaPlayback.CurrentState>> currentStateAttribute = mediaPlayback.getCurrentState();
                if(currentStateAttribute.isPresent() && currentStateAttribute.get().isAvailable())
                {
                    currentStateAttribute.get().addObserver(new Attribute.Listener() {
                        @Override
                        public void onError(Error error) {
                            Log.e(TAG, "Error when listening to CurrentState " + error);
                        }

                        @Override
                        public void onChange(Object currentStateValue) {
                            Log.i(TAG, "CurrentState changed to value " + currentStateValue);
                        }
                    }, 0, 1);
                }
                else
                {
                    Log.e(TAG, "Attribute unavailable on the selected endpoint");
                }
            }
            else
            {
                Log.e(TAG, "Required cluster not found on the selected endpoint");
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
  import Foundation
import os.log

class MTRAttributeSubscriptionExampleViewModel: ObservableObject {
    let Log = Logger(subsystem: "com.matter.casting",
                     category: "MTRAttributeSubscriptionExampleViewModel")
    
    let sampleContentAppVid: String = "12345"
    
    @Published var status: String?;
    
    func subscribeToAttribute(castingPlayer: MTRCastingPlayer)
    {
        if let endpoint: MTREndpoint = castingPlayer.endpoints.filter({ $0.identifier == sampleContentAppVid }).first
        {
            if(endpoint.hasCluster(MTREndpointClusterTypeContentLauncher))
            {
                let cluster: MTRMediaPlaybackCluster = endpoint.cluster(for: MTREndpointClusterTypeMediaPlayback) as! MTRMediaPlaybackCluster
                let currentStateAttribute: MTRAttribute<MTRCurrentState> = cluster.currentState
                
                class CurrentStateObserver : NSObject, MTRObserver
                {
                    weak var parent: MTRAttributeSubscriptionExampleViewModel! = nil
                    
                    init(parent: MTRAttributeSubscriptionExampleViewModel!) {
                        self.parent = parent
                    }
                    
                    func attribute(_ sender: NSObject, valueDidChange value: NSValue?, oldValue: NSValue?) {
                        self.parent.status = "Value changed from \(String(describing: oldValue)) to \(String(describing: value))"
                    }
                }
                
                currentStateAttribute.add(CurrentStateObserver(parent: self), withMinInterval: 0, maxInterval: 1) { err in
                    self.status = "Could not establish subscription"
                }
            }
            else
            {
                self.status = "No MediaPlayback cluster supporting endpoint found"
            }
        }
        else
        {
            self.status = "No endpoint matching the example VID found"
        }
    }
}
  ```
</details>

## Conclusion
As described above, the Casting APIs allows a Casting Client to discover CastingPlayers on a users network, connect to one of them and interact with various Endpoints on them using Matter commands, attribute reads and subscriptions.