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


struct TvCastingApp: App {
    @UIApplicationDelegateAdaptor(AppDelegate.self) var appDelegate
    
    let Log = Logger(subsystem: "com.matter.casting",
                     category: "TvCastingApp")
    @State
    var firstAppActivation: Bool = true

    var body: some Scene {
        WindowGroup {
            ContentView()
                .onAppear(perform: {

                })
                /*.onReceive(NotificationCenter.default.publisher(for: UIApplication.willResignActiveNotification)) { _ in
                    //self.Log.info("TvCastingApp: UIApplication.willResignActiveNotification")

                }
                .onReceive(NotificationCenter.default.publisher(for: UIApplication.didBecomeActiveNotification)) { _ in
                    //self.Log.info("TvCastingApp: UIApplication.didBecomeActiveNotification")
                }
                .onReceive(NotificationCenter.default.publisher(for: UIApplication.didEnterBackgroundNotification)) { _ in
                    //self.Log.info("TvCastingApp: UIApplication.didEnterBackgroundNotification")
                }
                .onReceive(NotificationCenter.default.publisher(for: UIApplication.willEnterForegroundNotification)) { _ in
                    //self.Log.info("TvCastingApp: UIApplication.willEnterForegroundNotification")
                }
                .onReceive(NotificationCenter.default.publisher(for: UIApplication.willTerminateNotification)) { _ in
                    //self.Log.info("TvCastingApp: UIApplication.willTerminateNotification")
                }*/
        }
    }
}

@main
class AppDelegate: NSObject, UIApplicationDelegate {
    let Log = Logger(subsystem: "com.matter.casting",
                     category: "AppDelegate")
    @State
    var firstAppActivation: Bool = true
    
    var window: UIWindow?

    func application(_ application: UIApplication, didFinishLaunchingWithOptions launchOptions: [UIApplication.LaunchOptionsKey : Any]? = nil) -> Bool {
        self.Log.info("AppDelegate: UIApplication.didFinishLaunchingWithOptions")
        
        let window = UIWindow()
        self.window = window
        window.rootViewController = UIHostingController(rootView: ContentView())
        window.makeKeyAndVisible()
        
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
        return true
    }
    
    func applicationWillResignActive(_ application: UIApplication) {
        self.Log.info("AppDelegate: UIApplication.applicationWillResignActive")
        if let castingServerBridge = CastingServerBridge.getSharedInstance()
        {
            castingServerBridge.stopMatterServer()
        }
    }
    
    func applicationDidBecomeActive(_ application: UIApplication) {
        self.Log.info("AppDelegate: UIApplication.applicationDidBecomeActive")
        if(!firstAppActivation)
        {
            if let castingServerBridge = CastingServerBridge.getSharedInstance()
            {
                castingServerBridge.startMatterServer(DispatchQueue.main, startMatterServerCompletionCallback: { (error: MatterError) -> () in
                    DispatchQueue.main.async {
                        self.Log.info("AppDelegate.startMatterServerCompletionCallback called with \(error)")
                    }
                })
            }
        }
        firstAppActivation = false
    }

    func applicationWillTerminate(_ application: UIApplication) {
        self.Log.info("AppDelegate: UIApplication.applicationWillTerminate")
/*        if let castingServerBridge = CastingServerBridge.getSharedInstance()
        {
            castingServerBridge.stopMatterServer()
        }*/
    }
    
    func applicationDidEnterBackground(_ application: UIApplication) {
        self.Log.info("AppDelegate: UIApplication.applicationDidEnterBackground")
        /*if let castingServerBridge = CastingServerBridge.getSharedInstance()
        {
            castingServerBridge.stopMatterServer()
        }*/
    }
    
    func applicationWillEnterForeground(_ application: UIApplication) {
        self.Log.info("AppDelegate: UIApplication.applicationWillEnterForeground")
        /*if let castingServerBridge = CastingServerBridge.getSharedInstance()
        {
            castingServerBridge.startMatterServer(DispatchQueue.main, startMatterServerCompletionCallback: { (error: MatterError) -> () in
                DispatchQueue.main.async {
                    self.Log.info("AppDelegate.startMatterServerCompletionCallback called with \(error)")
                }
            })
        }*/
    }
}
