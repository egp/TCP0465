// tests/host/test_WB_tcp0465core_read_oxygen_percent.cpp v1
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wkeyword-macro"
#endif
#define private public
#include "TCP0465Core.h"
#undef private
#if defined(__clang__)
#pragma clang diagnostic pop
#endif

#include <cmath>
#include <cstdlib>
#include <iostream>

namespace {

void require(bool condition, const char* message) {
  if (!condition) {
    std::cerr << "FAIL: " << message << std::endl;
    std::exit(1);
  }
}

void test_WB_readSucceedsForValidPendingOxygenResponse() {
  TCP0465Core core;
  float percentVol = -99.0f;

  require(core.begin(), "begin() should succeed");

  const uint8_t frame[TCP0465Core::RESPONSE_FRAME_SIZE] = {
      0xFF, 0x86, 0x00, 0xD1, 0x05, 0x01, 0x00, 0x00, 0xA3};

  for (size_t i = 0; i < TCP0465Core::RESPONSE_FRAME_SIZE; ++i) {
    core.pendingResponseFrame_[i] = frame[i];
  }
  core.pendingResponseLength_ = TCP0465Core::RESPONSE_FRAME_SIZE;
  core.pendingResponseValid_ = true;

  require(core.readOxygenPercent(percentVol),
          "valid pending oxygen response should succeed");
  require(std::fabs(static_cast<double>(percentVol) - 20.9) < 0.0001,
          "valid pending oxygen response should decode to 20.9");
  require(core.lastError_ == TCP0465Core::ERROR_NONE,
          "successful read should leave ERROR_NONE");
  require(!core.pendingResponseValid_,
          "successful read should consume pending response");
  require(core.pendingResponseLength_ == 0,
          "successful read should clear pending response length");
}

void test_WB_readMapsChecksumMismatchToPublicErrorCode() {
  TCP0465Core core;
  float percentVol = -99.0f;

  require(core.begin(), "begin() should succeed");

  const uint8_t frame[TCP0465Core::RESPONSE_FRAME_SIZE] = {
      0xFF, 0x86, 0x00, 0xD1, 0x05, 0x01, 0x00, 0x00, 0x00};

  for (size_t i = 0; i < TCP0465Core::RESPONSE_FRAME_SIZE; ++i) {
    core.pendingResponseFrame_[i] = frame[i];
  }
  core.pendingResponseLength_ = TCP0465Core::RESPONSE_FRAME_SIZE;
  core.pendingResponseValid_ = true;

  require(!core.readOxygenPercent(percentVol),
          "checksum-mismatched pending response should fail");
  require(core.lastError_ == TCP0465Core::ERROR_CHECKSUM_MISMATCH,
          "checksum mismatch should map to ERROR_CHECKSUM_MISMATCH");
  require(percentVol == -1.0f,
          "failed read should overwrite output with sentinel");
  require(!core.pendingResponseValid_,
          "failed read should still consume pending response");
  require(core.pendingResponseLength_ == 0,
          "failed read should clear pending response length");
}

}  // namespace

int main() {
  test_WB_readSucceedsForValidPendingOxygenResponse();
  test_WB_readMapsChecksumMismatchToPublicErrorCode();
  std::cout << "PASS: test_WB_tcp0465core_read_oxygen_percent" << std::endl;
  return 0;
}
// tests/host/test_WB_tcp0465core_read_oxygen_percent.cpp v1