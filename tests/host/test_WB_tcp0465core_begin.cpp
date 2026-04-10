// tests/host/test_WB_tcp0465core_begin.cpp v1
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

#include <cstdlib>
#include <iostream>

namespace {

void require(bool condition, const char* message) {
  if (!condition) {
    std::cerr << "FAIL: " << message << std::endl;
    std::exit(1);
  }
}

void test_WB_beginSetsInitializedAddressAndClearsError() {
  TCP0465Core core;
  core.lastError_ = TCP0465Core::ERROR_I2C_READ;

  const uint8_t customAddress = 0x76;
  require(core.begin(customAddress), "begin(customAddress) should succeed");
  require(core.initialized_, "begin(customAddress) should set initialized_");
  require(core.address_ == customAddress,
          "begin(customAddress) should store address_");
  require(core.lastError_ == TCP0465Core::ERROR_NONE,
          "begin(customAddress) should clear lastError_ to ERROR_NONE");
}

}  // namespace

int main() {
  test_WB_beginSetsInitializedAddressAndClearsError();
  std::cout << "PASS: test_WB_tcp0465core_begin" << std::endl;
  return 0;
}
// tests/host/test_WB_tcp0465core_begin.cpp v1