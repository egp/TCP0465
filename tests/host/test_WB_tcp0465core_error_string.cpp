// tests/host/test_WB_tcp0465core_error_string.cpp v1
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
#include <cstring>
#include <iostream>

namespace {

void require(bool condition, const char* message) {
  if (!condition) {
    std::cerr << "FAIL: " << message << std::endl;
    std::exit(1);
  }
}

void test_WB_errorStringMapsKnownErrorCodes() {
  TCP0465Core core;

  core.lastError_ = TCP0465Core::ERROR_NOT_INITIALIZED;
  require(std::strcmp(core.errorString(), "not initialized") == 0,
          "ERROR_NOT_INITIALIZED should map to \"not initialized\"");

  core.lastError_ = TCP0465Core::ERROR_MODE_SWITCH_REJECTED;
  require(std::strcmp(core.errorString(), "mode switch rejected") == 0,
          "ERROR_MODE_SWITCH_REJECTED should map to \"mode switch rejected\"");
}

}  // namespace

int main() {
  test_WB_errorStringMapsKnownErrorCodes();
  std::cout << "PASS: test_WB_tcp0465core_error_string" << std::endl;
  return 0;
}
// tests/host/test_WB_tcp0465core_error_string.cpp v1