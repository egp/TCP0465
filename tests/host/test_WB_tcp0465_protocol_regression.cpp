// test_WB_tcp0465_protocol_regression.cpp

#include "TCP0465Protocol.h"
#include <cassert>
#include <cstring>

using namespace tcp0465;

static void test_frame_structure_invariants() {
  uint8_t frame[FRAME_SIZE] = {};
  buildCommandFrame(0x11, 0x22, frame);

  assert(frame[0] == START_BYTE);
  assert(frame[1] == PROTOCOL_ADDRESS);
  assert(frame[2] == 0x11);
  assert(frame[3] == 0x22);

  for (int i = 4; i < FRAME_SIZE - 1; ++i) {
    assert(frame[i] == 0x00);
  }

  uint8_t checksum = computeChecksum(frame, FRAME_SIZE - 1);
  assert(frame[FRAME_SIZE - 1] == checksum);
}

static void test_validate_frame_checksum_corruption() {
  uint8_t frame[FRAME_SIZE] = {};
  buildCommandFrame(COMMAND_READ_GAS, 0x00, frame);

  frame[FRAME_SIZE - 1] ^= 0xFF; // corrupt checksum

  auto status = validateFrame(frame, FRAME_SIZE, COMMAND_READ_GAS);
  assert(status == PARSE_CHECKSUM_MISMATCH);
}

static void test_parse_invalid_gas_type() {
  uint8_t frame[FRAME_SIZE] = {};
  buildCommandFrame(COMMAND_READ_GAS, 0x00, frame);

  frame[2] = 0x01;
  frame[3] = 0xF4;
  frame[4] = 0x99; // invalid gas type
  frame[5] = 1;

  GasReading r{};
  auto status = parseGasResponse(frame, FRAME_SIZE, r);

  assert(status == PARSE_UNEXPECTED_GAS_TYPE);
}

static void test_parse_invalid_decimals() {
  uint8_t frame[FRAME_SIZE] = {};
  buildCommandFrame(COMMAND_READ_GAS, 0x00, frame);

  frame[2] = 0x01;
  frame[3] = 0xF4;
  frame[4] = EXPECTED_GAS_TYPE_OXYGEN;
  frame[5] = 9; // invalid

  GasReading r{};
  auto status = parseGasResponse(frame, FRAME_SIZE, r);

  assert(status == PARSE_INVALID_DECIMALS);
}

int main() {
  test_frame_structure_invariants();
  test_validate_frame_checksum_corruption();
  test_parse_invalid_gas_type();
  test_parse_invalid_decimals();
  return 0;
}