// src/TCP0465Core.cpp v1
#include "TCP0465Core.h"

TCP0465Core::TCP0465Core()
    : address_(DEFAULT_ADDRESS),
      lastError_(ERROR_NONE),
      initialized_(false) {}

bool TCP0465Core::begin(uint8_t address) {
  address_ = address;
  initialized_ = false;
  lastError_ = ERROR_MODE_SWITCH_FAILED;
  return false;
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
  return ERROR_MODE_SWITCH_FAILED;
}

const char* TCP0465Core::errorString() const {
  return "stub";
}

uint8_t TCP0465Core::address() const {
  return address_;
}

void TCP0465Core::setError(ErrorCode error) {
  lastError_ = error;
}
// src/TCP0465Core.cpp v1