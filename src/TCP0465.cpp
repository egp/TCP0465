// src/TCP0465.cpp v6
#include "TCP0465.h"

#include "TCP0465Protocol.h"

namespace {
TCP0465::ErrorCode mapParseStatus(tcp0465::ParseStatus status) {
  switch (status) {
    case tcp0465::PARSE_OK:
      return TCP0465::ERROR_NONE;
    case tcp0465::PARSE_SHORT_RESPONSE:
      return TCP0465::ERROR_SHORT_RESPONSE;
    case tcp0465::PARSE_INVALID_START_BYTE:
      return TCP0465::ERROR_INVALID_START_BYTE;
    case tcp0465::PARSE_UNEXPECTED_COMMAND:
      return TCP0465::ERROR_UNEXPECTED_COMMAND;
    case tcp0465::PARSE_CHECKSUM_MISMATCH:
      return TCP0465::ERROR_CHECKSUM_MISMATCH;
    case tcp0465::PARSE_UNEXPECTED_GAS_TYPE:
      return TCP0465::ERROR_UNEXPECTED_GAS_TYPE;
    case tcp0465::PARSE_INVALID_DECIMALS:
      return TCP0465::ERROR_INVALID_DECIMALS;
    case tcp0465::PARSE_MODE_SWITCH_REJECTED:
      return TCP0465::ERROR_MODE_SWITCH_REJECTED;
    default:
      return TCP0465::ERROR_I2C_READ;
  }
}
}  // namespace

TCP0465::TCP0465()
    : i2c_(nullptr),
      address_(DEFAULT_ADDRESS),
      lastError_(ERROR_NOT_INITIALIZED),
      initialized_(false) {}

bool TCP0465::begin(BBI2C& i2c, uint8_t address) {
  i2c_ = &i2c;
  address_ = address;
  initialized_ = false;
  setError(ERROR_NONE);

  if (!setPassiveMode()) {
    return false;
  }

  initialized_ = true;
  setError(ERROR_NONE);
  return true;
}

bool TCP0465::readOxygenPercent(float& percentVol) {
  if (!initialized_ || i2c_ == nullptr) {
    percentVol = -1.0f;
    setError(ERROR_NOT_INITIALIZED);
    return false;
  }

  if (!writeCommand(kCommandReadGas)) {
    percentVol = -1.0f;
    return false;
  }

  uint8_t frame[kFrameSize] = {0};
  if (!requestFrame(frame, sizeof(frame))) {
    percentVol = -1.0f;
    return false;
  }

  tcp0465::GasReading reading = {};
  const tcp0465::ParseStatus status =
      tcp0465::parseGasResponse(frame, sizeof(frame), reading);
  if (status != tcp0465::PARSE_OK) {
    percentVol = -1.0f;
    setError(mapParseStatus(status));
    return false;
  }

  percentVol = reading.percentVol;
  setError(ERROR_NONE);
  return true;
}

TCP0465::ErrorCode TCP0465::lastError() const {
  return lastError_;
}

const char* TCP0465::errorString() const {
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

uint8_t TCP0465::address() const {
  return address_;
}

bool TCP0465::setPassiveMode() {
  if (!writeCommand(kCommandSetMode, kModePassive)) {
    setError(ERROR_MODE_SWITCH_FAILED);
    return false;
  }

  uint8_t frame[kFrameSize] = {0};
  if (!requestFrame(frame, sizeof(frame))) {
    setError(ERROR_MODE_SWITCH_FAILED);
    return false;
  }

  const tcp0465::ParseStatus status =
      tcp0465::parseModeSwitchResponse(frame, sizeof(frame));
  if (status != tcp0465::PARSE_OK) {
    setError(mapParseStatus(status));
    return false;
  }

  delay(10);
  setError(ERROR_NONE);
  return true;
}

bool TCP0465::writeCommand(uint8_t command, uint8_t data3) {
  if (i2c_ == nullptr) {
    setError(ERROR_NOT_INITIALIZED);
    return false;
  }

  uint8_t frame[kFrameSize] = {0};
  tcp0465::buildCommandFrame(command, data3, frame);

  const int written = I2CWrite(i2c_, address_, frame, sizeof(frame));
  if (written != static_cast<int>(sizeof(frame))) {
    setError(ERROR_I2C_WRITE);
    return false;
  }

  return true;
}

bool TCP0465::requestFrame(uint8_t* frame, size_t frameLength) {
  if (i2c_ == nullptr) {
    setError(ERROR_NOT_INITIALIZED);
    return false;
  }

  const int received = I2CRead(i2c_, address_, frame, static_cast<int>(frameLength));
  if (received <= 0) {
    setError(ERROR_I2C_READ);
    return false;
  }
  if (received < static_cast<int>(frameLength)) {
    setError(ERROR_SHORT_RESPONSE);
    return false;
  }

  return true;
}

void TCP0465::setError(ErrorCode error) {
  lastError_ = error;
}
// src/TCP0465.cpp v6