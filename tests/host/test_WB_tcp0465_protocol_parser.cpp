// tests/host/test_WB_tcp0465_protocol_parser.cpp V1

#include <cmath>
#include <cstdlib>
#include <iostream>

#include "TCP0465Protocol.h"

namespace {

void require(bool condition, const char* message) {
  if (!condition) {
    std::cerr << "FAIL: " << message << std::endl;
    std::exit(1);
  }
}

void test_WB_knownChecksumForReadGasCommand() {
  uint8_t frame[tcp0465::FRAME_SIZE] = {0};
  tcp0465::buildCommandFrame(tcp0465::COMMAND_READ_GAS, 0x00, frame);

  require(frame[0] == 0xFF, "start byte should be 0xFF");
  require(frame[1] == 0x01, "protocol address should be 0x01");
  require(frame[2] == 0x86, "command should be 0x86");
  require(frame[8] == 0x79, "known checksum for passive gas command should be 0x79");
}

void test_WB_parseValidOxygenResponse() {
  const uint8_t frame[tcp0465::FRAME_SIZE] = {
      0xFF, 0x86, 0x00, 0xD1, 0x05, 0x01, 0x00, 0x00, 0xA3};

  tcp0465::GasReading reading = {};
  const tcp0465::ParseStatus status =
      tcp0465::parseGasResponse(frame, tcp0465::FRAME_SIZE, reading);

  require(status == tcp0465::PARSE_OK, "valid gas response should parse");
  require(reading.rawCounts == 209, "raw count should be 209");
  require(reading.gasType == 0x05, "gas type should be oxygen");
  require(reading.decimals == 1, "decimals should be 1");
  require(std::fabs(static_cast<double>(reading.percentVol) - 20.9) < 0.0001,
          "percentVol should be 20.9");
}

void test_WB_rejectUnexpectedGasType() {
  const uint8_t frame[tcp0465::FRAME_SIZE] = {
      0xFF, 0x86, 0x00, 0xD1, 0x03, 0x01, 0x00, 0x00, 0xA5};

  tcp0465::GasReading reading = {};
  const tcp0465::ParseStatus status =
      tcp0465::parseGasResponse(frame, tcp0465::FRAME_SIZE, reading);

  require(status == tcp0465::PARSE_UNEXPECTED_GAS_TYPE,
          "unexpected gas type should fail");
}

void test_WB_rejectInvalidDecimals() {
  const uint8_t frame[tcp0465::FRAME_SIZE] = {
      0xFF, 0x86, 0x00, 0xD1, 0x05, 0x03, 0x00, 0x00, 0xA1};

  tcp0465::GasReading reading = {};
  const tcp0465::ParseStatus status =
      tcp0465::parseGasResponse(frame, tcp0465::FRAME_SIZE, reading);

  require(status == tcp0465::PARSE_INVALID_DECIMALS,
          "invalid decimals should fail");
}

void test_WB_rejectChecksumMismatch() {
  const uint8_t frame[tcp0465::FRAME_SIZE] = {
      0xFF, 0x86, 0x00, 0xD1, 0x05, 0x01, 0x00, 0x00, 0x00};

  tcp0465::GasReading reading = {};
  const tcp0465::ParseStatus status =
      tcp0465::parseGasResponse(frame, tcp0465::FRAME_SIZE, reading);

  require(status == tcp0465::PARSE_CHECKSUM_MISMATCH,
          "checksum mismatch should fail");
}

void test_WB_rejectShortFrame() {
  const uint8_t frame[8] = {
      0xFF, 0x86, 0x00, 0xD1, 0x05, 0x01, 0x00, 0x00};

  tcp0465::GasReading reading = {};
  const tcp0465::ParseStatus status =
      tcp0465::parseGasResponse(frame, 8, reading);

  require(status == tcp0465::PARSE_SHORT_RESPONSE,
          "short frame should fail");
}

void test_WB_acceptModeSwitchAck() {
  const uint8_t frame[tcp0465::FRAME_SIZE] = {
      0xFF, 0x78, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x87};

  const tcp0465::ParseStatus status =
      tcp0465::parseModeSwitchResponse(frame, tcp0465::FRAME_SIZE);

  require(status == tcp0465::PARSE_OK, "mode switch ack should parse");
}

}  // namespace

int main() {
  test_WB_knownChecksumForReadGasCommand();
  test_WB_parseValidOxygenResponse();
  test_WB_rejectUnexpectedGasType();
  test_WB_rejectInvalidDecimals();
  test_WB_rejectChecksumMismatch();
  test_WB_rejectShortFrame();
  test_WB_acceptModeSwitchAck();

  std::cout << "PASS: test_WB_tcp0465_protocol_parser" << std::endl;
  return 0;
}
// tests/host/test_WB_tcp0465_protocol_parser.cpp V1