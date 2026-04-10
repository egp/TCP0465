// tests/host/test_WB_tcp0465core_last_error.cpp v1
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

void test_WB_constructorSeedsLastErrorMemberToNone() {
  TCP0465Core core;
  require(core.lastError_ == TCP0465Core::ERROR_NONE,
          "constructor should seed lastError_ to ERROR_NONE");
}

}  // namespace

int main() {
  test_WB_constructorSeedsLastErrorMemberToNone();
  std::cout << "PASS: test_WB_tcp0465core_last_error" << std::endl;
  return 0;
}
// tests/host/test_WB_tcp0465core_last_error.cpp v1