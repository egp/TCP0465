// src/TCP0465Protocol.h V1

#ifndef TCP0465_PROTOCOL_H
#define TCP0465_PROTOCOL_H

#include <stddef.h>
#include <stdint.h>

namespace tcp0465 {

static const uint8_t START_BYTE = 0xFF;
static const uint8_t PROTOCOL_ADDRESS = 0x01;
static const uint8_t COMMAND_SET_MODE = 0x78;
static const uint8_t COMMAND_READ_GAS = 0x86;
static const uint8_t MODE_PASSIVE = 0x04;
static const uint8_t MODE_SWITCH_ACCEPTED = 0x01;
static const uint8_t EXPECTED_GAS_TYPE_OXYGEN = 0x05;
static const size_t FRAME_SIZE = 9;

enum ParseStatus : uint8_t {
  PARSE_OK = 0,
  PARSE_SHORT_RESPONSE,
  PARSE_INVALID_START_BYTE,
  PARSE_UNEXPECTED_COMMAND,
  PARSE_CHECKSUM_MISMATCH,
  PARSE_UNEXPECTED_GAS_TYPE,
  PARSE_INVALID_DECIMALS,
  PARSE_MODE_SWITCH_REJECTED
};

struct GasReading {
  uint16_t rawCounts;
  uint8_t gasType;
  uint8_t decimals;
  float percentVol;
};

uint8_t computeChecksum(const uint8_t* bytes, size_t lengthWithoutChecksum);
void buildCommandFrame(uint8_t command, uint8_t data3, uint8_t outFrame[FRAME_SIZE]);

ParseStatus validateFrame(
    const uint8_t* frame,
    size_t frameLength,
    uint8_t expectedCommand);

ParseStatus parseGasResponse(
    const uint8_t* frame,
    size_t frameLength,
    GasReading& outReading);

ParseStatus parseModeSwitchResponse(
    const uint8_t* frame,
    size_t frameLength);

const char* parseStatusString(ParseStatus status);

}  // namespace tcp0465

#endif
// src/TCP0465Protocol.h V1