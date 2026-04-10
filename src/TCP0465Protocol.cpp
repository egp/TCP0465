// src/TCP0465Protocol.cpp v1

#include "TCP0465Protocol.h"

namespace tcp0465 {

uint8_t computeChecksum(const uint8_t* bytes, size_t lengthWithoutChecksum) {
  uint16_t sum = 0;
  for (size_t i = 1; i < lengthWithoutChecksum; ++i) {
    sum += bytes[i];
  }
  const uint8_t low = static_cast<uint8_t>(sum & 0xFF);
  return static_cast<uint8_t>(0xFF - low + 0x01);
}

void buildCommandFrame(uint8_t command, uint8_t data3, uint8_t outFrame[FRAME_SIZE]) {
  outFrame[0] = START_BYTE;
  outFrame[1] = PROTOCOL_ADDRESS;
  outFrame[2] = command;
  outFrame[3] = data3;
  outFrame[4] = 0x00;
  outFrame[5] = 0x00;
  outFrame[6] = 0x00;
  outFrame[7] = 0x00;
  outFrame[8] = computeChecksum(outFrame, FRAME_SIZE - 1);
}

ParseStatus validateFrame(
    const uint8_t* frame,
    size_t frameLength,
    uint8_t expectedCommand) {
  if (frameLength < FRAME_SIZE) {
    return PARSE_SHORT_RESPONSE;
  }

  if (frame[0] != START_BYTE) {
    return PARSE_INVALID_START_BYTE;
  }

  if (frame[1] != expectedCommand) {
    return PARSE_UNEXPECTED_COMMAND;
  }

  const uint8_t expectedChecksum = computeChecksum(frame, FRAME_SIZE - 1);
  if (frame[FRAME_SIZE - 1] != expectedChecksum) {
    return PARSE_CHECKSUM_MISMATCH;
  }

  return PARSE_OK;
}

ParseStatus parseGasResponse(
    const uint8_t* frame,
    size_t frameLength,
    GasReading& outReading) {
  const ParseStatus status = validateFrame(frame, frameLength, COMMAND_READ_GAS);
  if (status != PARSE_OK) {
    return status;
  }

  outReading.rawCounts =
      static_cast<uint16_t>((static_cast<uint16_t>(frame[2]) << 8) | frame[3]);
  outReading.gasType = frame[4];
  outReading.decimals = frame[5];

  if (outReading.gasType != EXPECTED_GAS_TYPE_OXYGEN) {
    return PARSE_UNEXPECTED_GAS_TYPE;
  }

  float scale = 0.0f;
  switch (outReading.decimals) {
    case 0:
      scale = 1.0f;
      break;
    case 1:
      scale = 0.1f;
      break;
    case 2:
      scale = 0.01f;
      break;
    default:
      return PARSE_INVALID_DECIMALS;
  }

  outReading.percentVol = static_cast<float>(outReading.rawCounts) * scale;
  return PARSE_OK;
}

ParseStatus parseModeSwitchResponse(
    const uint8_t* frame,
    size_t frameLength) {
  const ParseStatus status = validateFrame(frame, frameLength, COMMAND_SET_MODE);
  if (status != PARSE_OK) {
    return status;
  }

  if (frame[2] != MODE_SWITCH_ACCEPTED) {
    return PARSE_MODE_SWITCH_REJECTED;
  }

  return PARSE_OK;
}

const char* parseStatusString(ParseStatus status) {
  switch (status) {
    case PARSE_OK:
      return "ok";
    case PARSE_SHORT_RESPONSE:
      return "short response";
    case PARSE_INVALID_START_BYTE:
      return "invalid start byte";
    case PARSE_UNEXPECTED_COMMAND:
      return "unexpected command byte";
    case PARSE_CHECKSUM_MISMATCH:
      return "checksum mismatch";
    case PARSE_UNEXPECTED_GAS_TYPE:
      return "unexpected gas type";
    case PARSE_INVALID_DECIMALS:
      return "invalid decimals";
    case PARSE_MODE_SWITCH_REJECTED:
      return "mode switch rejected";
    default:
      return "unknown parse status";
  }
}

}  // namespace tcp0465

// src/TCP0465Protocol.cpp v1