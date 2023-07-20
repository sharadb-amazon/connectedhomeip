/*
 *   Copyright (c) 2020-23 Project CHIP Authors
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
 *
 */
package com.chip.casting;

public class CommissioningCallbacks {
  private SuccessCallback<Void> sessionEstablishmentStarted;
  private SuccessCallback<Void> sessionEstablished;
  private FailureCallback sessionEstablishmentStopped;
  private Object commissioningComplete;
  private Object commissioningWindowOpened;

  public SuccessCallback<Void> getSessionEstablishmentStarted() {
    return sessionEstablishmentStarted;
  }

  public void setSessionEstablishmentStarted(SuccessCallback<Void> sessionEstablishmentStarted) {
    this.sessionEstablishmentStarted = sessionEstablishmentStarted;
  }

  public SuccessCallback<Void> getSessionEstablished() {
    return sessionEstablished;
  }

  public Object getCommissioningWindowOpened() {
    return commissioningWindowOpened;
  }

  public void setCommissioningWindowOpened(Object commissioningWindowOpened) {
    this.commissioningWindowOpened = commissioningWindowOpened;
  }

  public void setSessionEstablished(SuccessCallback<Void> sessionEstablished) {
    this.sessionEstablished = sessionEstablished;
  }

  public FailureCallback getSessionEstablishmentStopped() {
    return sessionEstablishmentStopped;
  }

  public void setSessionEstablishmentStopped(FailureCallback sessionEstablishmentStopped) {
    this.sessionEstablishmentStopped = sessionEstablishmentStopped;
  }

  public Object getCommissioningComplete() {
    return commissioningComplete;
  }

  public void setCommissioningComplete(Object commissioningComplete) {
    this.commissioningComplete = commissioningComplete;
  }
}
