#ifndef TCP1819_H
#define TCP1819_H

#include <cstdint>

struct BBI2C {
  uint8_t bWire = 0;
  int iSDA = -1;
  int iSCL = -1;
};

inline int I2CInit(BBI2C*, uint32_t) {
  return 1;
}

inline int I2CWrite(BBI2C*, uint8_t, const uint8_t*, int) {
  return 1;
}

inline int I2CRead(BBI2C*, uint8_t, uint8_t*, int) {
  return 1;
}

#endif