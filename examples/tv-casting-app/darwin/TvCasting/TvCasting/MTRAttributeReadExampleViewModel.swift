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

class MTRAttributeReadExampleViewModel: ObservableObject {
    let Log = Logger(subsystem: "com.matter.casting",
                     category: "MTRAttributeReadExampleViewModel")
    
    let sampleContentAppVid: String = "12345"
    
    @Published var status: String?;
    
    func readAttribute(castingPlayer: MTRCastingPlayer)
    {
        if let endpoint: MTREndpoint = castingPlayer.endpoints().filter({ $0.identifier == sampleContentAppVid }).first
        {
            if(endpoint.hasCluster(MTREndpointClusterTypeContentLauncher))
            {
                let cluster: MTRMediaPlaybackCluster = endpoint.cluster(for: MTREndpointClusterTypeMediaPlayback) as! MTRMediaPlaybackCluster
                let currentStateAttribute: MTRAttribute<MTRCurrentState> = cluster.currentState
                
                if(currentStateAttribute.isAvailable())
                {
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
