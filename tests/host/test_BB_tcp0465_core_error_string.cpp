// tests/host/test_BB_tcp0465core_error_string.cpp v1
#include <stdlib.h>
#include <cstring>
#include <iostream>

#include "TCP0465Core.h"

namespace {

void require(bool condition, const char* message) {
  if (!condition) {
    std::cerr << "FAIL: " << message << std::endl;
    exit(1);
  }
}

void test_BB_defaultConstructedCoreReportsNoErrorString() {
  TCP0465Core core;
  require(std::strcmp(core.errorString(), "no error") == 0,
          "default-constructed core should report \"no error\"");
}

}  // namespace

int main() {
  test_BB_defaultConstructedCoreReportsNoErrorString();
  std::cout << "PASS: test_BB_tcp0465core_error_string" << std::endl;
  return 0;
}
// tests/host/test_BB_tcp0465core_error_string.cpp v1