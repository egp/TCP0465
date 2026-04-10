src/TCP0465.cpp

// src/TCP0465.cpp v1
#include "TCP0465.h"

TCP0465::TCP0465()
    : wire_(nullptr),
      address_(0x00),
      lastError_(ERROR_NONE),
      initialized_(false) {}

bool TCP0465::begin(TwoWire& wire, uint8_t address) {
  (void)wire;
  (void)address;
  return false;
}

bool TCP0465::readOxygenPercent(float& percentVol) {
  percentVol = -1.0f;
  return false;
}

TCP0465::ErrorCode TCP0465::lastError() const {
  return ERROR_NONE;
}

const char* TCP0465::errorString() const {
  return "stub";
}

uint8_t TCP0465::address() const {
  return 0x00;
}

bool TCP0465::setPassiveMode() {
  return false;
}

bool TCP0465::writeCommand(uint8_t command, uint8_t data3) {
  (void)command;
  (void)data3;
  return false;
}

bool TCP0465::requestFrame(uint8_t* frame, size_t frameLength) {
  (void)frame;
  (void)frameLength;
  return false;
}

void TCP0465::setError(ErrorCode error) {
  (void)error;
}
// src/TCP0465.cpp v1