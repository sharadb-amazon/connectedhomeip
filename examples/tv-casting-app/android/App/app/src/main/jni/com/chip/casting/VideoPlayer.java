/** a.k.a. TV */
public class VideoPlayer {
  private long nodeID;
  private byte fabricIndex;

  private boolean isConnected;
  // Empty if we don't have a connection to the TargetVideoPlayer yet (for e.g. when the casting app
  // was just restarted)
  private List<ContentApp> contentApps;

  private String deviceName;
  private long vendorID;
  private long productID;
  private long deviceType;

  /** getter methods for each of the fields above */
}
