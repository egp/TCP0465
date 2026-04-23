// test_BB_tcp0465_core_full_sequence.cpp

#include "TCP0465.h"
#include "TCP0465Protocol.h"
#include <cassert>
#include <cstdint>

// -----------------------------------------------------------------------------
// Fake BBI2C using TCP1819-style host seam expectations
// -----------------------------------------------------------------------------
struct FakeBBI2C : public BBI2C {

  int write(uint8_t, uint8_t, const uint8_t*, size_t len) {
    // Always succeed write at BB level
    return static_cast<int>(len);
  }

  int read(uint8_t, uint8_t, uint8_t* out, size_t len) {

    // Default zero-fill
    for (size_t i = 0; i < len; i++) {
      out[i] = 0;
    }

    // -------------------------------------------------------------------------
    // This frame is consumed by setPassiveMode() -> parseModeSwitchResponse()
    //
    // Required conditions from TCP0465Protocol:
    //   frame[0] = START_BYTE
    //   frame[1] = expectedCommand (COMMAND_SET_MODE)
    //   frame[2] = MODE_SWITCH_ACCEPTED
    //   frame[last] = checksum
    // -------------------------------------------------------------------------

    if (len >= tcp0465::FRAME_SIZE) {
      out[0] = tcp0465::START_BYTE;
      out[1] = tcp0465::COMMAND_SET_MODE;
      out[2] = tcp0465::MODE_SWITCH_ACCEPTED;

      out[tcp0465::FRAME_SIZE - 1] =
          tcp0465::computeChecksum(out, tcp0465::FRAME_SIZE - 1);
    }

    return static_cast<int>(len);
  }
};

// -----------------------------------------------------------------------------
// Full sequence test
// -----------------------------------------------------------------------------
static void test_full_sequence_happy_path() {

  FakeBBI2C bus;

  TCP0465 dev;

  bool ok = dev.begin(bus, 0x50);
  assert(ok);

  float v = -1.0f;

  ok = dev.readOxygenPercent(v);

  // BB-level guarantee: if protocol + wiring + parsing are correct
  assert(ok);
  assert(v >= 0.0f);
  assert(v <= 100.0f);
}

// -----------------------------------------------------------------------------
// main
// -----------------------------------------------------------------------------
int main() {
  test_full_sequence_happy_path();
  return 0;
}