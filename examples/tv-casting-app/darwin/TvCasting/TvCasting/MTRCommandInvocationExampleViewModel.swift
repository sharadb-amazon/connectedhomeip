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

class MTRCommandInvocationExampleViewModel: ObservableObject {
    let Log = Logger(subsystem: "com.matter.casting",
                     category: "MTRCommandInvocationExampleViewModel")
    
    let sampleContentAppVid: String = "12345"
    
    @Published var status: String?;
    
    func invokeCommand(castingPlayer: MTRCastingPlayer)
    {
        if let endpoint: MTREndpoint = castingPlayer.endpoints().filter({ $0.identifier == sampleContentAppVid }).first
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
