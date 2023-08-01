/**
 *
 *    Copyright (c) 2020-2022 Project CHIP Authors
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

import SwiftUI
import os.log

@main
struct TvCastingApp: App {
    let Log = Logger(subsystem: "com.matter.casting",
                     category: "TvCastingApp")
    @State
    var firstAppActivation: Bool = true

    init()
    {
        class AppParametersDataSource : NSObject, MTRDataSource
        {
            func castingAppDidReceiveRequestForRotatingDeviceIdUniqueId(_ sender: Any) -> Data {
                // dummy value for demonstration only
                return "EXAMPLE_APP_ID".data(using: .utf8)!
            }
            
            func castingAppDidReceiveRequestForCommissioningData(_ sender: Any) -> MTRCommissioningData {
                // dummy values for demonstration only
                return MTRCommissioningData(passcode: 20202021, discriminator: 3874, spake2pIterationCount: 1000, spake2pVerifier: nil, spake2pSalt: nil)
            }
            
            func castingApp(_ sender: Any, didReceiveRequestToSignCertificateRequest csrData: Data) async -> Data {
                // sign the message and return that
                return Data()
            }
        }
        MTRCastingApp.initialize(with: AppParametersDataSource())
        MTRCastingApp.start()
    }
    
    var body: some Scene {
        WindowGroup {
            ContentView()
                .onAppear(perform: {
                    if let castingServerBridge = CastingServerBridge.getSharedInstance()
                    {
                        let appParameters: AppParameters = AppParameters()
                        
                        var rotatingDeviceIdUniqueId: [UInt8] = [UInt8](repeating: 0, count: 16 )
                        for i in (0...15)
                        {
                            rotatingDeviceIdUniqueId[i] = UInt8.random(in: 0..<255)
                        }
                        appParameters.rotatingDeviceIdUniqueId = Data(rotatingDeviceIdUniqueId)
                        
                        let onboardingParameters: OnboardingPayload = OnboardingPayload()
                        onboardingParameters.setupPasscode = 20202021
                        onboardingParameters.setupDiscriminator = 3840
                        
                        appParameters.onboardingPayload = onboardingParameters
                        
                        let err = castingServerBridge.initializeApp(appParameters, clientQueue: DispatchQueue.main, initAppStatusHandler: { (result: Bool) -> () in
                            self.Log.info("initializeApp result \(result)")
                        })
                        self.Log.info("initializeApp return value \(err)")
                    }
                })
                .onReceive(NotificationCenter.default.publisher(for: UIApplication.willResignActiveNotification)) { _ in
                    self.Log.info("TvCastingApp: UIApplication.willResignActiveNotification")
                    if let castingServerBridge = CastingServerBridge.getSharedInstance()
                    {
                        castingServerBridge.stopMatterServer()
                    }
                }
                .onReceive(NotificationCenter.default.publisher(for: UIApplication.didBecomeActiveNotification)) { _ in
                    self.Log.info("TvCastingApp: UIApplication.didBecomeActiveNotification")
                    if(!firstAppActivation)
                    {
                        if let castingServerBridge = CastingServerBridge.getSharedInstance()
                        {
                            castingServerBridge.startMatterServer(DispatchQueue.main, startMatterServerCompletionCallback: { (error: MatterError) -> () in
                                DispatchQueue.main.async {
                                    self.Log.info("TvCastingApp.startMatterServerCompletionCallback called with \(error)")
                                }
                            })
                        }
                    }
                    firstAppActivation = false
                }
                .onReceive(NotificationCenter.default.publisher(for: UIApplication.didEnterBackgroundNotification)) { _ in
                    self.Log.info("TvCastingApp: UIApplication.didEnterBackgroundNotification")
                }
                .onReceive(NotificationCenter.default.publisher(for: UIApplication.didFinishLaunchingNotification)) { _ in
                    self.Log.info("TvCastingApp: UIApplication.didFinishLaunchingNotification")
                }
                .onReceive(NotificationCenter.default.publisher(for: UIApplication.willEnterForegroundNotification)) { _ in
                    self.Log.info("TvCastingApp: UIApplication.willEnterForegroundNotification")
                }
                .onReceive(NotificationCenter.default.publisher(for: UIApplication.willTerminateNotification)) { _ in
                    self.Log.info("TvCastingApp: UIApplication.willTerminateNotification")
                }
        }
    }
}
