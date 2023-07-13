package com.matter.casting.err;

import java.util.Objects;

public class MatterError {
  private long errorCode;
  private String errorMessage;

  public static final com.chip.casting.MatterError NO_ERROR =
      new com.chip.casting.MatterError(0, null);

  public MatterError(long errorCode, String errorMessage) {
    this.errorCode = errorCode;
    this.errorMessage = errorMessage;
  }

  public boolean hasError() {
    return !this.equals(NO_ERROR);
  }

  public boolean hasNoError() {
    return this.equals(NO_ERROR);
  }

  public long getErrorCode() {
    return errorCode;
  }

  public String getErrorMessage() {
    return errorMessage;
  }

  @Override
  public String toString() {
    return "MatterError{"
        + (hasNoError()
            ? "No error"
            : "errorCode=" + errorCode + ", errorMessage='" + errorMessage + '\'')
        + '}';
  }

  @Override
  public boolean equals(Object o) {
    if (this == o) return true;
    if (o == null || getClass() != o.getClass()) return false;
    com.chip.casting.MatterError matterError = (com.chip.casting.MatterError) o;
    return errorCode == matterError.getErrorCode();
  }

  @Override
  public int hashCode() {
    return Objects.hash(errorCode);
  }
}
