package chip.devicecontroller;

import chip.devicecontroller.model.AttributeWriteRequest;
import chip.devicecontroller.model.ChipAttributePath;
import chip.devicecontroller.model.ChipEventPath;
import chip.devicecontroller.model.DataVersionFilter;
import chip.devicecontroller.model.InvokeElement;
import java.util.List;
import javax.annotation.Nullable;

public class ChipInteractionClient {
  static native void subscribe(
      long deviceControllerPtr,
      long callbackHandle,
      long devicePtr,
      List<ChipAttributePath> attributePaths,
      List<ChipEventPath> eventPaths,
      List<DataVersionFilter> dataVersionFilters,
      int minInterval,
      int maxInterval,
      boolean keepSubscriptions,
      boolean isFabricFiltered,
      int imTimeoutMs,
      @Nullable Long eventMin);

  static native void read(
      long deviceControllerPtr,
      long callbackHandle,
      long devicePtr,
      List<ChipAttributePath> attributePaths,
      List<ChipEventPath> eventPaths,
      List<DataVersionFilter> dataVersionFilters,
      boolean isFabricFiltered,
      int imTimeoutMs,
      @Nullable Long eventMin);

  static native void write(
      long deviceControllerPtr,
      long callbackHandle,
      long devicePtr,
      List<AttributeWriteRequest> attributeList,
      int timedRequestTimeoutMs,
      int imTimeoutMs);

  static native void invoke(
      long deviceControllerPtr,
      long callbackHandle,
      long devicePtr,
      InvokeElement invokeElement,
      int timedRequestTimeoutMs,
      int imTimeoutMs);

  static native void extendableInvoke(
      long deviceControllerPtr,
      long callbackHandle,
      long devicePtr,
      List<InvokeElement> invokeElementList,
      int timedRequestTimeoutMs,
      int imTimeoutMs);

  public static void loadJni() {
    System.out.println("ChipInteractionClient loadJni called");
  }

  static {
    /*System.out.println("Loading CHIPInteractionModel");
    System.loadLibrary("CHIPInteractionModel");
    System.out.println("Loading CHIPInteractionModel done");*/
  }
}
