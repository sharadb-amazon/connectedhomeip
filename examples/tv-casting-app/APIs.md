
# Matter Casting APIs

Matter Casting consists of three parts:

* **The mobile app**: For most content providers, this would be your consumer-facing mobile app.  By making your moile app a Matter "Casting Client", you enable the user to discover casting targets, cast content, and control casting sessions. The [example Matter tv-casting-app](https://github.com/project-chip/connectedhomeip/tree/master/examples/tv-casting-app) for Android / iOS and Linux builds on top of the Matter SDK to demonstrate how a TV Casting mobile app works.
* **The TV content app**: For most content providers, this would be your consumer-facing app on a Smart TV.  By enhancing your TV app to act as a Matter "Content app", you enable Matter Casting Clients to cast content. The [example Matter content-app](https://github.com/project-chip/connectedhomeip/tree/master/examples/tv-app/android/App/content-app) for Android builds on top of the Matter SDK to demonstrate how a TV Content app works.
* **The TV platform app**: The TV platform app is generally implemented by the TV manufacturer and provides common capabilities around media playback on the TV like the Basic Video Player. The [example Matter tv-app](https://github.com/project-chip/connectedhomeip/tree/master/examples/tv-app) for Android builds on top of the Matter SDK to demonstrate how a TV platform app works.

This document describes how enable your Android and iOS apps to act as a Matter "Casting Client".  This documentation is also designed to work with the example [example Matter tv-casting-app](https://github.com/project-chip/connectedhomeip/tree/master/examples/tv-casting-app) samples so you can see the experience end to end.

## Introduction

A Casting Client (e.g. a mobile phone app) is expected to be a Matter Commissionable Node and a `CastingPlayer` (i.e. a TV) is expected to be a Matter Commissioner. In the context of the [Matter Video Player architecture](https://github.com/CHIP-Specifications/connectedhomeip-spec/blob/master/src/app_clusters/media/VideoPlayerArchitecture.adoc), a `CastingPlayer` would map to [Casting "Video" Player](https://github.com/CHIP-Specifications/connectedhomeip-spec/blob/master/src/app_clusters/media/VideoPlayerArchitecture.adoc#1-introduction). The `CastingPlayer` is expected to be hosting one or more `Endpoints` (similar to [Content Apps](https://github.com/CHIP-Specifications/connectedhomeip-spec/blob/master/src/app_clusters/media/VideoPlayerArchitecture.adoc#1-introduction) in the Matter Video Player architecture) that support one or more Matter Media `Clusters`.

The steps to start a casting session are:

1. [Initialize](#initialize-the-casting-client) the `CastingClient` using the Matter SDK.
1. [Discover](#discover-casting-players) `CastingPlayer` devices using Matter Commissioner discovery.
1. [Connect](#connect-to-a-casting-player) to the `CastingPlayer` to discover available endpoints.
1. [Select](#select-an-endpoint-on-the-casting-player) an available `Endpoint` hosted by the `CastingPlayer`.

Next, you're ready to:

1. [Issue commands](#issuing-commands) to the `Endpoint`.
1. [Read](#read-operations) endpoint attributes like playback state.
1. [Subscribe](#subscriptions) to playback events.

## Build and Setup
The Casting Client is expected to consume the Matter TV Casting library built for its respective platform to consume any of the APIs described in this document. Refer to the tv-casting-app READMEs for [Android](android) and [iOS](darwin) to understand how to build and consume each platform's specicific libraries.

### Initialize the Casting Client
_{Complete Initialization examples: [Android](android/App/app/src/main/java/com/matter/casting/InitializationExample.java) |  [iOS](darwin/TvCasting/TvCasting/TvCastingApp.swift)}_

A Casting Client must first initialize the Matter SDK and define the following [`DataProvider`](android/App/app/src/main/jni/com/matter/casting/params/DataProvider.java) objects for the the Matter Casting library to use throughout the client's lifecycle:

1.  **Rotating Device Identifier** - Refer to the Matter specification for details on how to generate the [Rotating Device Identifier](https://github.com/CHIP-Specifications/connectedhomeip-spec/blob/master/src/rendezvous/DeviceDiscovery.adoc#245-rotating-device-identifier)).  Then, instantiate a `DataProvider` object as described below.
    
    On Android, define a `rotatingDeviceIdUniqueIdProvider` to provide the Casting Client's RotatingDeviceIdUniqueId, by implementing a `DataSource`:

    ```java
    private final static DataProvider<byte[]> rotatingDeviceIdUniqueIdProvider = new DataProvider<byte[]>() {  
        private static final String APP_ID = "YOUR_GENERATED_ROTATING_DEVICE_ID";  
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
            return "YOUR_GENERATED_ROTATING_DEVICE_ID".data(using: .utf8)!
        }
        ...
    }
    ```

3. **Commissioning Data** - This object contains the passcode, discriminator, etc needed to verify an incoming commissioning request. Refer to the Matter specification's [Onboarding Payload](https://github.com/CHIP-Specifications/connectedhomeip-spec/blob/master/src/qr_code/OnboardingPayload.adoc#ref_OnboardingPayload)) section for details on commissioning data.
    
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
        return MTRCommissioningData(
            passcode: 20202021,
            discriminator: 3874,
            spake2pIterationCount: 1000,
            spake2pVerifier: nil,
            spake2pSalt: nil)
    }
    ```

4. **Device Attestation Credentials** - This object contains the `DeviceAttestationCertificate`, `ProductAttestationIntermediateCertificate`, etc. and implements a way to sign messages when called upon by the Matter TV Casting Library as part of the [Device Attestation process](https://github.com/CHIP-Specifications/connectedhomeip-spec/blob/master/src/device_attestation/Device_Attestation_Specification.adoc) during commissoning. 

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
		        ECPrivateKeySpec ecPrivateKeySpec = new ECPrivateKeySpec(new BigInteger(1, privateKeyBytes), parameterSpec);  

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

            deviceAttestationCredentials.setDeviceAttestationCert(
            Base64.decode(kDevelopmentDAC_Cert_FFF1_8001, Base64.DEFAULT));
            deviceAttestationCredentials.setProductAttestationIntermediateCert(
            Base64.decode(KPAI_FFF1_8000_Cert_Array, Base64.DEFAULT));
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
    
Once you have created the `DataProvider` objects above, you are ready to initialize the Casting App as described below.  Note: When you initialize the Casting client, make sure your code initializes it only once, before it starts a Matter casting session.

On Android, create an AppParameters object using the `rotatingDeviceIdUniqueIdProvider`, `commissioningDataProvider` and `dacProvider`, and call `CastingApp.initialize` with it. 

```java
public static void demoInitialization() {  
    // Create an AppParameters object to pass in global casting parameters to the SDK  
    final AppParameters appParameters = new AppParameters(
        rotatingDeviceIdUniqueIdProvider,
        commissioningDataProvider,
        dacProvider);  
  
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

### Discover Casting Players
_{Complete Discovery examples: [Android](android/App/app/src/main/java/com/matter/casting/DiscoveryExample.java) |  [iOS](darwin/TvCasting/TvCasting/MTRDiscoveryExampleViewModel.swift)}_

The Casting Client discovers `CastingPlayers` using Matter Commissioner discovery over DNS-SD by listening for CastingPlayer events as they are discovered, updated, or lost from the network.

On Android, define a concrete anonymous `class castingPlayerChangeListener` that implements the `CastingPlayerChangeListener`.

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

On iOS, implement a `func addDiscoveredCastingPlayers`, `func removeDiscoveredCastingPlayers` and `func updateDiscoveredCastingPlayers` which listen to notifications as Casting Players are added, removed, or updated.

```objectivec
@objc
func addDiscoveredCastingPlayers(notification: Notification)
{
    guard let userInfo = notification.userInfo,
        let castingPlayers     = userInfo[castingPlayersUserInfo] as? [MTRCastingPlayer]
    else {
        print("No MTRCastingPlayer found in notification")
        return
    }
    displayedCastingPlayers.append(contentsOf: castingPlayers)
}
    
@objc
func removeDiscoveredCastingPlayers(notification: Notification)
{
    guard let userInfo = notification.userInfo,
    let castingPlayers = userInfo[castingPlayersUserInfo] as? [MTRCastingPlayer]
    else {
        print("No MTRCastingPlayer found in notification")
        return
    }
    displayedCastingPlayers = displayedCastingPlayers.filter { !castingPlayers.contains($0)}
}
    
@objc
func updateDiscoveredCastingPlayers(notification: Notification)
{
    guard let userInfo = notification.userInfo,
    let castingPlayers = userInfo[castingPlayersUserInfo] as? [MTRCastingPlayer]
    else {
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
    NotificationCenter.default.addObserver(
    self,
    selector: #selector(self.addDiscoveredCastingPlayers),
    name: NSNotification.Name.didAddCastingPlayers,
    object: nil)
    NotificationCenter.default.addObserver(
    self,
    selector: #selector(self.removeDiscoveredCastingPlayers),
    name: NSNotification.Name.didRemoveCastingPlayers,
    object: nil)
    NotificationCenter.default.addObserver(
    self,
    selector: #selector(self.updateDiscoveredCastingPlayers),
    name: NSNotification.Name.didUpdateCastingPlayers,
    object: nil)

    MTRCastingPlayerDiscovery.sharedInstance().start()
    self.discoveryRequestStatus = true
}
```

Note: You will need to connect with a Casting Player as described below to see the list of Endpoints that they support.  Refer to the [Connection](#connection) section for details on how to discover available endpoints supported by a Casting Player.

### Connect to a Casting Player
_{Complete Connection examples: [Android](android/App/app/src/main/java/com/matter/casting/ConnectionExample.java) |  [iOS](darwin/TvCasting/TvCasting/MTRConnectionExampleViewModel.swift)}_

Each CastingPlayer object created during [Discovery](#discovery) contains information such as deviceName, vendorId, productId, etc. which can help the user pick the right CastingPlayer. A Casting Client can attempt to connect to the selectedCastingPlayer using [Matter User Directed Commissioning (UDC)](https://github.com/CHIP-Specifications/connectedhomeip-spec/blob/master/src/rendezvous/UserDirectedCommissioning.adoc). The Matter TV Casting library locally caches information required to reconnect to a CastingPlayer, once it has commissioned with it. After that, it is able to skip the full UDC process by establishing CASE with the CastingPlayer directly. Once connected, the CastingPlayer object would contain the list of available Endpoints on it.

On Android, the Casting Client can connect to a `CastingPlayer` by calling `connect` on it, and handling any exceptions that may arise in the process.

```java
// Maximum time in seconds we'll wait for the connection to the CastingPlayer to go through  
private static final long CONNECTION_TIMEOUT_SECS = 45;  
  
public static void demoConnection(CastingPlayer castingPlayer) {  
    // Connect to the castingPlayer  
    CompletableFuture<Void> connectionFuture = castingPlayer.connect(CONNECTION_TIMEOUT_SECS);  
  
    // handle exception, if any  
    connectionFuture.exceptionally(exc -> {  
        Log.e(TAG, "Exception in connecting to castingPlayer" + exc.getMessage());  
        return null;  
    });
}
```

On iOS, the Casting Client may call `connect` on the `MTRCastingPlayer` object it wants to connect to and handle any `NSErrors` that may happen in the process.

```objectivec
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
```

### Select an Endpoint on the Casting Player
_{Endpoing selections is demonstrated in the Command invocation examples: [Android](android/App/app/src/main/java/com/matter/casting/CommandInvocationExample.java) |  [iOS](darwin/TvCasting/TvCasting/MTRCommandInvocationExampleViewModel.swift)}_

On a successful connection with a `CastingPlayer`, a Casting Client may select one of the `Endpoints` to interact with based on its attributes (e.g. Vendor ID, Product ID, list of supported Clusters, etc).

On Android, for example, it may select an Endpoint with a particular vendorID.

```java
...  
Optional<Endpoint> contentAppEndpoint =  
    castingPlayer  
        .getEndpoints()  
        .stream()  
        .filter(endpoint -> SAMPLE_CONTENT_APP_VID.equals(endpoint.getVendorId()))  
        .findAny();
...
```


On iOS, it can do the above Endpoint selection like the following.
```objectivec
...
if let endpoint: MTREndpoint = castingPlayer.endpoints.filter({ $0.identifier == sampleContentAppVid }).first
...
```

## Interacting with a Casting Endpoint

Once the Casting Client has selected an `Endpoint`, it is ready to [issue commands](#issuing-commands) to it, [read](#read-operations) current playback state, and [subscribe](#subscriptions) to playback events.

### Issuing Commands
_{Complete Command invocation examples: [Android](android/App/app/src/main/java/com/matter/casting/CommandInvocationExample.java) |  [iOS](darwin/TvCasting/TvCasting/MTRCommandInvocationExampleViewModel.swift)}_

To issue a command, the Casting Client can  `contentAppEndpoint` on the CastingPlayer it connected to and handle the command response. It would first ensure the endpoint supports the required cluster/command.

On Android, this can be implemented as follows:

```java
...
if (contentAppEndpoint.get().hasCluster(ContentLauncher.class)) {  
    // get the Cluster to use  
    ContentLauncher contentLauncher = contentAppEndpoint.get().getCluster(ContentLauncher.class);  
  
    // invoke the command  
    CompletableFuture<ContentLauncher.LauncherResponse> contentLauncherResponseFuture =  
                contentLauncher.launchURL(
		            new URL("https://YOUR_URL/id"), "displaystring", null);  
    		contentLauncherResponseFuture.thenApply(  
			(response) -> {  
				// handle the response  
				return null;  
			});
}  
```

On iOS, the commands can be sent as follows:

```objectivec
...
if(endpoint.hasCluster(MTREndpointClusterTypeContentLauncher))
{
    let cluster: MTRContentLauncherCluster = endpoint.cluster(for: MTREndpointClusterTypeContentLauncher) as! MTRContentLauncherCluster
    if(cluster.canLaunchURL())
    {
        cluster.launch(URL(string: "https://YOUR_URL/id")!) { launchResponse, err in
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
    self.status = "No ContentLauncher cluster support found on the endpoint"
}
...
```

### Read Operations
_{Complete Attribute Read examples: [Android](android/App/app/src/main/java/com/matter/casting/AttributeReadExample.java) |  [iOS](darwin/TvCasting/TvCasting/MTRAttributeReadExampleViewModel.swift)}_

The CastingClient may read an Attribute from the `contentAppEndpoint` on the CastingPlayer. It would ensure that the desired cluster / attribute are available for reading on the endpoint before trying to read it.

On Android, an attribute can be read with implementation like the one below.
```java
// check if the Endpoint supports the Cluster to use
if (contentAppEndpoint.get().hasCluster(MediaPlayback.class)) {
    // get the Cluster to use
    MediaPlayback mediaPlayback = contentAppEndpoint.get().getCluster(MediaPlayback.class);

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
```

On iOS, an attribute can be read with implementation like the one below.

```objectivec
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
```

### Subscriptions
_{Complete Attribute subscription examples: [Android](android/App/app/src/main/java/com/matter/casting/AttributeSubscriptionExample.java) |  [iOS](darwin/TvCasting/TvCasting/MTRAttributeSubscriptionExampleViewModel.swift)}_

A Casting Client may subscribe to an attribute from the `contentAppEndpoint` on the CastingPlayer to get event notifications related to that attribute.

On Android, an attribute can be subscribed to in the following way.

```java
// check if the Endpoint supports the Cluster to use
if (contentAppEndpoint.get().hasCluster(MediaPlayback.class)) {
    // get the Cluster to use
     MediaPlayback mediaPlayback = contentAppEndpoint.get().getCluster(MediaPlayback.class);

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
```

On iOS, an attribute can be subscribed to in the following way.
```objectivec
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
```
_{Complete Attribute subscription examples: [Android](android/App/app/src/main/java/com/matter/casting/AttributeSubscriptionExample.java) |  [iOS](darwin/TvCasting/TvCasting/MTRAttributeSubscriptionExampleViewModel.swift)}_
