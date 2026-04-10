// tests/host/test_BB_tcp0465core_read_oxygen_percent.cpp v1
#include <cstdlib>
#include <iostream>

#include "TCP0465Core.h"

namespace {

void require(bool condition, const char* message) {
  if (!condition) {
    std::cerr << "FAIL: " << message << std::endl;
    std::exit(1);
  }
}

void test_BB_readBeforeBeginFailsNotInitialized() {
  TCP0465Core core;
  float percentVol = 123.0f;

  require(!core.readOxygenPercent(percentVol),
          "readOxygenPercent() before begin() should fail");
  require(core.lastError() == TCP0465Core::ERROR_NOT_INITIALIZED,
          "read before begin() should set ERROR_NOT_INITIALIZED");
  require(percentVol == -1.0f,
          "failed read before begin() should overwrite output with sentinel");
}

void test_BB_readAfterBeginWithoutPendingResponseFailsReadError() {
  TCP0465Core core;
  float percentVol = 123.0f;

  require(core.begin(), "begin() should succeed before read test");
  require(!core.readOxygenPercent(percentVol),
          "readOxygenPercent() without pending response should fail");
  require(core.lastError() == TCP0465Core::ERROR_I2C_READ,
          "read without pending response should set ERROR_I2C_READ");
  require(percentVol == -1.0f,
          "failed read without pending response should overwrite output with sentinel");
}

}  // namespace

int main() {
  test_BB_readBeforeBeginFailsNotInitialized();
  test_BB_readAfterBeginWithoutPendingResponseFailsReadError();
  std::cout << "PASS: test_BB_tcp0465core_read_oxygen_percent" << std::endl;
  return 0;
}
// tests/host/test_BB_tcp0465core_read_oxygen_percent.cpp v1