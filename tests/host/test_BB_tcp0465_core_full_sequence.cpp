// test_BB_tcp0465_core_full_sequence.cpp
#include "TCP0465Protocol.h"  // ONLY for computeChecksum if needed
#include "TCP0465.h"
#include <cassert>

struct FakeBBI2C : public BBI2C {

  int write(uint8_t, uint8_t, const uint8_t*, size_t len) {
    return (int)len;
  }

int read(uint8_t, uint8_t, uint8_t* out, size_t len) {

  for (size_t i = 0; i < len; i++) {
    out[i] = 0;
  }

  // ONLY what the parser ultimately checks:
  // frame[2] must indicate success path
  out[2] = 1;

  // DO NOT attempt full protocol validity
  // TCP0465Protocol owns that responsibility, not the fake

  return (int)len;
}
};

// static void buildValidModeSwitchFrame(uint8_t* out, size_t len) {

//   for (size_t i = 0; i < len; i++) {
//     out[i] = 0;
//   }

//   out[0] = 0xA5;   // if START_BYTE is not visible, use protocol-known value ONLY IF defined in header
//   out[1] = 0x00;   // command placeholder (must match expectedCommand in parser path)
//   out[2] = 1;      // accepted

//   out[len - 1] = tcp0465::computeChecksum(out, len - 1);
// }

static void test_full_sequence_happy_path() {

  FakeBBI2C bus;

  TCP0465 dev;

  bool ok = dev.begin(bus, 0x50);
  assert(ok);

  float v = -1.0f;

  ok = dev.readOxygenPercent(v);

  assert(ok);
  assert(v >= 0.0f);
}

int main() {
  test_full_sequence_happy_path();
}