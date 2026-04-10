// tests/host/fakes/Wire.h v1
#ifndef TESTS_HOST_FAKES_WIRE_H
#define TESTS_HOST_FAKES_WIRE_H

#include <stddef.h>
#include <stdint.h>

class TwoWire {
public:
  void begin() {}
  void beginTransmission(uint8_t) {}
  size_t write(const uint8_t*, size_t size) { return size; }
  uint8_t endTransmission() { return 0; }
  uint8_t requestFrom(int, int quantity) { return static_cast<uint8_t>(quantity); }
  int available() { return 0; }
  int read() { return 0; }
};

extern TwoWire Wire;

#endif
// tests/host/fakes/Wire.h v1