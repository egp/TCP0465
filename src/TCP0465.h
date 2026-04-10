// src/TCP0465.h V1

#ifndef TCP0465_H
#define TCP0465_H

#include <Arduino.h>
#include <Wire.h>

class TCP0465 {
public:
  static const uint8_t DEFAULT_ADDRESS = 0x74;
  static const uint8_t EXPECTED_GAS_TYPE_OXYGEN = 0x05;

  enum ErrorCode : uint8_t {
    ERROR_NONE = 0,
    ERROR_NOT_INITIALIZED,
    ERROR_I2C_WRITE,
    ERROR_I2C_READ,
    ERROR_SHORT_RESPONSE,
    ERROR_INVALID_START_BYTE,
    ERROR_UNEXPECTED_COMMAND,
    ERROR_CHECKSUM_MISMATCH,
    ERROR_UNEXPECTED_GAS_TYPE,
    ERROR_INVALID_DECIMALS,
    ERROR_MODE_SWITCH_FAILED,
    ERROR_MODE_SWITCH_REJECTED
  };

  TCP0465();

  bool begin(TwoWire& wire = Wire, uint8_t address = DEFAULT_ADDRESS);
  bool readOxygenPercent(float& percentVol);

  ErrorCode lastError() const;
  const char* errorString() const;
  uint8_t address() const;

private:
  static const uint8_t kProtocolAddress = 0x01;
  static const uint8_t kStartByte = 0xFF;
  static const uint8_t kFrameSize = 9;

  static const uint8_t kCommandSetMode = 0x78;
  static const uint8_t kCommandReadGas = 0x86;

  static const uint8_t kModePassive = 0x04;
  static const uint8_t kModeSwitchAccepted = 0x01;

  bool setPassiveMode();
  bool writeCommand(uint8_t command, uint8_t data3 = 0x00);
  bool readFrame(uint8_t expectedCommand, uint8_t* frame, size_t frameLength);

  static uint8_t computeChecksum(const uint8_t* bytes, size_t lengthWithoutChecksum);

  void setError(ErrorCode error);

  TwoWire* wire_;
  uint8_t address_;
  ErrorCode lastError_;
  bool initialized_;
};

#endif
// src/TCP0465.h V1