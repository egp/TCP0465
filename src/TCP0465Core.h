// src/TCP0465Core.h v2
#ifndef TCP0465_CORE_H
#define TCP0465_CORE_H

#include <stddef.h>
#include <stdint.h>

class TCP0465Core {
public:
  static const uint8_t DEFAULT_ADDRESS = 0x74;
  static const uint8_t EXPECTED_GAS_TYPE_OXYGEN = 0x05;
  static const size_t RESPONSE_FRAME_SIZE = 9;

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

  TCP0465Core();

  bool begin(uint8_t address = DEFAULT_ADDRESS);
  bool readOxygenPercent(float& percentVol);

  ErrorCode lastError() const;
  const char* errorString() const;
  uint8_t address() const;

private:
  void setError(ErrorCode error);

  uint8_t address_;
  ErrorCode lastError_;
  bool initialized_;

  uint8_t pendingResponseFrame_[RESPONSE_FRAME_SIZE];
  size_t pendingResponseLength_;
  bool pendingResponseValid_;
};

#endif
// src/TCP0465Core.h v2