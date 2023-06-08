
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

On Android,

    enter code here

On iOS,

    enter code here

## Discovery
The Casting Client can then start discovery of `CastingPlayers` using the discovery APIs as shown below. This is implemented based on Matter Commissioner discovery over DNS-SD. The client can listen for CastingPlayers as they are discovered, updated or lost from the network.

On Android,

    enter code here

On iOS,

    enter code here

The client may stop an ongoing discovery, like so:

On Android,

    enter code here

On iOS,

    enter code here

And it may stop discovery notifications altogether, like so:

On Android,

    enter code here

On iOS,

    enter code here

Note: at this point, the list of Endpoints in the discovered CastingPlayers will be empty, as this information is not revealed at the time of Matter Commissioner discovery.
## Connection
After discovery, the Casting client may use user input to select on of the CastingPlayers to connect it. Each CastingPlayer object contains information such as deviceName, vendorId, productId, etc. to help make this selection. It can then attempt to connect to the selectedCastingPlayer. This is implemented based on Matter User Directed Commissioning (UDC). The Matter TV Casting library also locally caches information required to re-connect to a CastingPlayer. Once it does that, it is able to skip UDC and establishes CASE directly.

On Android,

    enter code here

On iOS,

    enter code here

## Interactions
On a successful connection with the CastingPlayer, a Casting client may pick of of the Endpoints to interact with based on its attributes (like Vendor or Product IDs, list of supported Clusters, etc). Here is an example, where the client code selects an Endpoint on the CastingPlayer with VendorID ("12345") that supports the ContentLauncher cluster LaunchURL command.

On Android,

    enter code here

On iOS,

    enter code here

### Commands
The CastingClient may send a command to the selectedEndpoint on the selectedCastingPlayer and handle the command response, like so:

On Android,

    enter code here

On iOS,

    enter code here

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