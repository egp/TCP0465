// tests/host/test_BB_tcp0465core_last_error.cpp v1
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

void test_BB_defaultConstructedCoreReportsNoError() {
  TCP0465Core core;
  require(core.lastError() == TCP0465Core::ERROR_NONE,
          "default-constructed core should report ERROR_NONE");
}

}  // namespace

int main() {
  test_BB_defaultConstructedCoreReportsNoError();
  std::cout << "PASS: test_BB_tcp0465core_last_error" << std::endl;
  return 0;
}
// tests/host/test_BB_tcp0465core_last_error.cpp v1