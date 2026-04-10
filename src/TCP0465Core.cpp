// src/TCP0465Core.cpp v4
#include "TCP0465Core.h"

TCP0465Core::TCP0465Core()
    : address_(DEFAULT_ADDRESS),
      lastError_(ERROR_NONE),
      initialized_(false) {}

bool TCP0465Core::begin(uint8_t address) {
  address_ = address;
  initialized_ = true;
  lastError_ = ERROR_NONE;
  return true;
}

bool TCP0465Core::readOxygenPercent(float& percentVol) {
  if (!initialized_) {
    lastError_ = ERROR_NOT_INITIALIZED;
  } else {
    lastError_ = ERROR_I2C_READ;
  }
  percentVol = -1.0f;
  return false;
}

TCP0465Core::ErrorCode TCP0465Core::lastError() const {
  return lastError_;
}

const char* TCP0465Core::errorString() const {
  switch (lastError_) {
    case ERROR_NONE:
      return "no error";
    case ERROR_NOT_INITIALIZED:
      return "not initialized";
    case ERROR_I2C_WRITE:
      return "I2C write failed";
    case ERROR_I2C_READ:
      return "I2C read failed";
    case ERROR_SHORT_RESPONSE:
      return "short response";
    case ERROR_INVALID_START_BYTE:
      return "invalid start byte";
    case ERROR_UNEXPECTED_COMMAND:
      return "unexpected command byte";
    case ERROR_CHECKSUM_MISMATCH:
      return "checksum mismatch";
    case ERROR_UNEXPECTED_GAS_TYPE:
      return "unexpected gas type";
    case ERROR_INVALID_DECIMALS:
      return "invalid decimal field";
    case ERROR_MODE_SWITCH_FAILED:
      return "mode switch command failed";
    case ERROR_MODE_SWITCH_REJECTED:
      return "mode switch rejected";
    default:
      return "unknown error";
  }
}

uint8_t TCP0465Core::address() const {
  return address_;
}

void TCP0465Core::setError(ErrorCode error) {
  lastError_ = error;
}
// src/TCP0465Core.cpp v4