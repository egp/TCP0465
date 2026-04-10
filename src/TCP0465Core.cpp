// src/TCP0465Core.cpp v5
#include "TCP0465Core.h"

#include "TCP0465Protocol.h"

namespace {

TCP0465Core::ErrorCode mapParseStatusToErrorCode(tcp0465::ParseStatus status) {
  switch (status) {
    case tcp0465::PARSE_OK:
      return TCP0465Core::ERROR_NONE;
    case tcp0465::PARSE_SHORT_RESPONSE:
      return TCP0465Core::ERROR_SHORT_RESPONSE;
    case tcp0465::PARSE_INVALID_START_BYTE:
      return TCP0465Core::ERROR_INVALID_START_BYTE;
    case tcp0465::PARSE_UNEXPECTED_COMMAND:
      return TCP0465Core::ERROR_UNEXPECTED_COMMAND;
    case tcp0465::PARSE_CHECKSUM_MISMATCH:
      return TCP0465Core::ERROR_CHECKSUM_MISMATCH;
    case tcp0465::PARSE_UNEXPECTED_GAS_TYPE:
      return TCP0465Core::ERROR_UNEXPECTED_GAS_TYPE;
    case tcp0465::PARSE_INVALID_DECIMALS:
      return TCP0465Core::ERROR_INVALID_DECIMALS;
    case tcp0465::PARSE_MODE_SWITCH_REJECTED:
      return TCP0465Core::ERROR_MODE_SWITCH_REJECTED;
    default:
      return TCP0465Core::ERROR_I2C_READ;
  }
}

}  // namespace

TCP0465Core::TCP0465Core()
    : address_(DEFAULT_ADDRESS),
      lastError_(ERROR_NONE),
      initialized_(false),
      pendingResponseFrame_{0},
      pendingResponseLength_(0),
      pendingResponseValid_(false) {}

bool TCP0465Core::begin(uint8_t address) {
  address_ = address;
  initialized_ = true;
  lastError_ = ERROR_NONE;
  pendingResponseLength_ = 0;
  pendingResponseValid_ = false;
  return true;
}

bool TCP0465Core::readOxygenPercent(float& percentVol) {
  if (!initialized_) {
    setError(ERROR_NOT_INITIALIZED);
    percentVol = -1.0f;
    return false;
  }

  if (!pendingResponseValid_) {
    setError(ERROR_I2C_READ);
    percentVol = -1.0f;
    return false;
  }

  tcp0465::GasReading reading = {};
  const tcp0465::ParseStatus status =
      tcp0465::parseGasResponse(
          pendingResponseFrame_,
          pendingResponseLength_,
          reading);

  pendingResponseLength_ = 0;
  pendingResponseValid_ = false;

  if (status != tcp0465::PARSE_OK) {
    setError(mapParseStatusToErrorCode(status));
    percentVol = -1.0f;
    return false;
  }

  percentVol = reading.percentVol;
  setError(ERROR_NONE);
  return true;
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
// src/TCP0465Core.cpp v5