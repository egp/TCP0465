// tests/host/test_BB_tcp0465core_address.cpp v1
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

void test_BB_defaultConstructedCoreReportsDefaultAddress() {
  TCP0465Core core;
  require(core.address() == TCP0465Core::DEFAULT_ADDRESS,
          "default-constructed core should report default address");
}

}  // namespace

int main() {
  test_BB_defaultConstructedCoreReportsDefaultAddress();
  std::cout << "PASS: test_BB_tcp0465core_address" << std::endl;
  return 0;
}
// tests/host/test_BB_tcp0465core_address.cpp v1