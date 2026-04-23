// tests/host/test_WB_tcp0465core_address.cpp v1
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

#include <stdlib.h>
#include <iostream>

namespace {

void require(bool condition, const char* message) {
  if (!condition) {
    std::cerr << "FAIL: " << message << std::endl;
    exit(1);
  }
}

void test_WB_constructorSeedsAddressMemberToDefaultAddress() {
  TCP0465Core core;
  require(core.address_ == TCP0465Core::DEFAULT_ADDRESS,
          "constructor should seed address_ to default address");
}

}  // namespace

int main() {
  test_WB_constructorSeedsAddressMemberToDefaultAddress();
  std::cout << "PASS: test_WB_tcp0465core_address" << std::endl;
  return 0;
}
// tests/host/test_WB_tcp0465core_address.cpp v1