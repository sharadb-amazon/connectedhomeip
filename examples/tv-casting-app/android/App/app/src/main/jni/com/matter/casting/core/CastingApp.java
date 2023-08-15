/*
 *   Copyright (c) 2023 Project CHIP Authors
 *   All rights reserved.
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 */

package com.matter.casting.core;

import com.matter.casting.support.MatterError;
import com.matter.casting.support.AppParameters;

public final class CastingApp {
    private static CastingApp sInstance;

    private CastingApp() {}

    public static CastingApp getInstance() {
      if (sInstance == null) {
        sInstance = new CastingApp();
      }
      return sInstance;
    }

    public MatterError initialize(AppParameters appParameters)
    {

    }

    private native MatterError initialize(AppParameters appParameters);

    // called after initialize
    public native MatterError start();

    public native MatterError stop();

    static {
        System.loadLibrary("TvCastingApp");
    }
}