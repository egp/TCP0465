// test_BB_tcp0465_core_full_sequence.cpp

#include "TCP0465.h"
#include "TCP0465Protocol.h"
#include <cassert>
#include <cstdint>

// -----------------------------------------------------------------------------
// Fake BBI2C using TCP1819-style host seam expectations
// -----------------------------------------------------------------------------
struct FakeBBI2C : public BBI2C {

  uint8_t pendingCommand = 0;

  int write(uint8_t, uint8_t, const uint8_t* data, size_t len) {

    if (len > 2) {
      pendingCommand = data[2];
    }

    return (int)len;
  }

  int read(uint8_t, uint8_t, uint8_t* out, size_t len) {

    for (size_t i = 0; i < len; i++) {
      out[i] = 0;
    }

    // -------------------------
    // MODE SWITCH RESPONSE
    // -------------------------
    if (pendingCommand == tcp0465::COMMAND_SET_MODE) {

      out[0] = tcp0465::START_BYTE;
      out[1] = tcp0465::COMMAND_SET_MODE;
      out[2] = tcp0465::MODE_SWITCH_ACCEPTED;

      out[len - 1] =
        tcp0465::computeChecksum(out, len - 1);

      pendingCommand = 0; // IMPORTANT: reset

      return (int)len;
    }

    // -------------------------
    // GAS RESPONSE
    // -------------------------
    if (pendingCommand == tcp0465::COMMAND_READ_GAS) {

      out[0] = tcp0465::START_BYTE;
      out[1] = tcp0465::COMMAND_READ_GAS;

      out[2] = 0x01;
      out[3] = 0xF4;

      out[4] = tcp0465::EXPECTED_GAS_TYPE_OXYGEN;
      out[5] = 2;

      out[len - 1] =
        tcp0465::computeChecksum(out, len - 1);

      pendingCommand = 0; // IMPORTANT

      return (int)len;
    }

    return (int)len;
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