// tests/host/test_BB_tcp0465core_begin.cpp v1
#include <stdlib.h>
#include <iostream>

#include "TCP0465Core.h"

namespace {

void require(bool condition, const char* message) {
  if (!condition) {
    std::cerr << "FAIL: " << message << std::endl;
    exit(1);
  }
}

void test_BB_beginWithDefaultAddressSucceeds() {
  TCP0465Core core;

  require(core.begin(), "begin() should succeed");
  require(core.address() == TCP0465Core::DEFAULT_ADDRESS,
          "begin() with default args should keep default address");
  require(core.lastError() == TCP0465Core::ERROR_NONE,
          "successful begin() should report ERROR_NONE");
}

void test_BB_beginWithCustomAddressSucceedsAndStoresAddress() {
  TCP0465Core core;
  const uint8_t customAddress = 0x77;

  require(core.begin(customAddress), "begin(customAddress) should succeed");
  require(core.address() == customAddress,
          "begin(customAddress) should store the configured address");
  require(core.lastError() == TCP0465Core::ERROR_NONE,
          "successful begin(customAddress) should report ERROR_NONE");
}

}  // namespace

int main() {
  test_BB_beginWithDefaultAddressSucceeds();
  test_BB_beginWithCustomAddressSucceedsAndStoresAddress();
  std::cout << "PASS: test_BB_tcp0465core_begin" << std::endl;
  return 0;
}
// tests/host/test_BB_tcp0465core_begin.cpp v1