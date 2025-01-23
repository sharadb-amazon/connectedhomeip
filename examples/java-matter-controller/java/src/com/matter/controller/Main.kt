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
 *
 */
package com.matter.controller

import chip.devicecontroller.ChipDeviceController
import chip.devicecontroller.ControllerParams
import com.matter.controller.commands.bdx.*
import com.matter.controller.commands.common.*
import com.matter.controller.commands.discover.*
import com.matter.controller.commands.icd.*
import com.matter.controller.commands.ota.PairOnNetworkLongOtaOverBdxCommand
import com.matter.controller.commands.pairing.*

import chip.devicecontroller.ChipClusters
import chip.devicecontroller.ChipStructs
import chip.devicecontroller.ChipTLVType.BaseTLVType
import chip.devicecontroller.ChipTLVType.EmptyType
import chip.devicecontroller.ChipTLVType.StringType
import chip.devicecontroller.ChipTLVType.StructElement
import chip.devicecontroller.ChipTLVType.StructType
import matter.tlv.TlvReader

import java.util.Optional

private fun getDiscoveryCommands(
  controller: ChipDeviceController,
  credentialsIssuer: CredentialsIssuer
): List<Command> {
  return listOf(
    DiscoverCommand(controller, credentialsIssuer),
    DiscoverCommissionablesCommand(controller, credentialsIssuer),
    DiscoverCommissionersCommand(controller, credentialsIssuer),
  )
}

private fun getPairingCommands(
  controller: ChipDeviceController,
  credentialsIssuer: CredentialsIssuer
): List<Command> {
  return listOf(
    UnpairCommand(controller, credentialsIssuer),
    PairCodeCommand(controller, credentialsIssuer),
    PairCodePaseCommand(controller, credentialsIssuer),
    PairCodeWifiCommand(controller, credentialsIssuer),
    PairCodeThreadCommand(controller, credentialsIssuer),
    PairAddressPaseCommand(controller, credentialsIssuer),
    PairAlreadyDiscoveredCommand(controller, credentialsIssuer),
    PairOnNetworkCommand(controller, credentialsIssuer),
    PairOnNetworkShortCommand(controller, credentialsIssuer),
    PairOnNetworkLongCommand(controller, credentialsIssuer),
    PairOnNetworkVendorCommand(controller, credentialsIssuer),
    PairOnNetworkCommissioningModeCommand(controller, credentialsIssuer),
    PairOnNetworkCommissionerCommand(controller, credentialsIssuer),
    PairOnNetworkDeviceTypeCommand(controller, credentialsIssuer),
    PairOnNetworkInstanceNameCommand(controller, credentialsIssuer),
  )
}

private fun getImCommands(
  controller: ChipDeviceController,
  credentialsIssuer: CredentialsIssuer
): List<Command> {
  return listOf(
    PairOnNetworkLongImReadCommand(controller, credentialsIssuer),
    PairOnNetworkLongImSubscribeCommand(controller, credentialsIssuer),
    PairOnNetworkLongImWriteCommand(controller, credentialsIssuer),
    PairOnNetworkLongImInvokeCommand(controller, credentialsIssuer),
    PairOnNetworkLongImExtendableInvokeCommand(controller, credentialsIssuer),
  )
}

private fun getICDCommands(
  controller: ChipDeviceController,
  credentialsIssuer: CredentialsIssuer
): List<Command> {
  return listOf(
    ICDListCommand(controller, credentialsIssuer),
  )
}

private fun getBdxCommands(
  controller: ChipDeviceController,
  credentialsIssuer: CredentialsIssuer
): List<Command> {
  return listOf(
    DownloadLogCommand(controller, credentialsIssuer),
    PairOnNetworkLongDownloadLogCommand(controller, credentialsIssuer),
  )
}

private fun getOtaCommands(
  controller: ChipDeviceController,
  credentialsIssuer: CredentialsIssuer
): List<Command> {
  return listOf(
    PairOnNetworkLongOtaOverBdxCommand(controller, credentialsIssuer),
  )
}

class MyCluster(devicePtr: Long, endpointId: Int) :
ChipClusters.ContentLauncherCluster(devicePtr, endpointId) {

fun launchURL(contentURL: String?, displayString: Optional<String?>, brandingInformation: Optional<ChipStructs.ContentLauncherClusterBrandingInformationStruct?>, timedInvokeTimeoutMs: Int): String {
val commandId = 1L

val elements = ArrayList<StructElement>()
val contentURLFieldID = 0L
val contentURLtlvValue: BaseTLVType = StringType(contentURL)
elements.add(StructElement(contentURLFieldID, contentURLtlvValue))

val displayStringFieldID = 1L
val displayStringtlvValue: BaseTLVType = displayString
    ?.orElse(null) // Convert Optional<String?> to String?
    ?.let { nonOptionalDisplayString -> StringType(nonOptionalDisplayString ?: "") } // Ensure non-null
    ?: EmptyType()

elements.add(StructElement(displayStringFieldID, displayStringtlvValue))

val brandingInformationFieldID = 2L
val brandingInformationtlvValue: BaseTLVType = brandingInformation
    ?.orElse(null) // Unwrap the Optional, returning null if empty
    ?.let { nonOptionalBrandingInformation -> nonOptionalBrandingInformation.encodeTlv() }
    ?: EmptyType()
elements.add(StructElement(brandingInformationFieldID, brandingInformationtlvValue))

val commandArgs = StructType(elements)
val tlvEncoding = encodeToTlv(commandArgs);
return TlvReader(tlvEncoding).toJsonString()
}
}

fun main(args: Array<String>) {
  val controller =
    ChipDeviceController(
      ControllerParams.newBuilder()
        .setUdpListenPort(0)
        .setControllerVendorId(0xFFF1)
        .setCountryCode("US")
        .build()
    )
  val credentialsIssuer = CredentialsIssuer()
  val commandManager = CommandManager()

  commandManager.register("discover", getDiscoveryCommands(controller, credentialsIssuer))
  commandManager.register("pairing", getPairingCommands(controller, credentialsIssuer))
  commandManager.register("im", getImCommands(controller, credentialsIssuer))
  commandManager.register("icd", getICDCommands(controller, credentialsIssuer))
  commandManager.register("bdx", getBdxCommands(controller, credentialsIssuer))
  commandManager.register("ota", getOtaCommands(controller, credentialsIssuer))
  try {
    commandManager.run(args)
  } catch (e: Exception) {
    println("Run command failed with exception: " + e.message)
    System.exit(1)
  }

  controller.shutdownCommissioning()
}
