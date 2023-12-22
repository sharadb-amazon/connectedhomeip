/**
 *
 *    Copyright (c) 2020-2023 Project CHIP Authors
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */


import Foundation
import os.log

class MTRAttributeSubscriptionExampleViewModel: ObservableObject {
    let Log = Logger(subsystem: "com.matter.casting",
                     category: "MTRAttributeSubscriptionExampleViewModel")
    
    let sampleContentAppVid: String = "12345"
    let MIN_INTERVAL: Int32 = 0
    let MAX_INTERVAL: Int32 = 1
    
    @Published var status: String?;
    
    func subscribeToAttribute(castingPlayer: MTRCastingPlayer)
    {
        if let endpoint: MTREndpoint = castingPlayer.endpoints().filter({ $0.identifier == sampleContentAppVid }).first
        {
            if(endpoint.hasCluster(MTREndpointClusterTypeContentLauncher))
            {
                let cluster: MTRMediaPlaybackCluster = endpoint.cluster(for: MTREndpointClusterTypeMediaPlayback) as! MTRMediaPlaybackCluster
                let currentStateAttribute: MTRAttribute<MTRCurrentState> = cluster.currentState
                
                if(currentStateAttribute.isAvailable())
                {
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
                    currentStateAttribute.add(CurrentStateObserver(parent: self), withMinInterval: MIN_INTERVAL, maxInterval: MAX_INTERVAL) { err in
                        self.status = "Could not establish subscription"
                    }
                }
                else
                {
                    self.status = "Attribute unavailable on the selected endpoint"
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
