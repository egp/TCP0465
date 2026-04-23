// test_BB_tcp0465_core_tcp1819_fault_matrix.cpp

#include "TCP0465.h"
#include "TCP1819_ScriptedBus.h"
#include <cassert>

using namespace tcp1819;

static void test_write_failure() {
  ScriptedBus bus;
  bus.expectWriteFail();

  TCP0465 dev(bus);
  dev.begin(bus, 0x50);

  float v = 0;
  bool ok = dev.readOxygenPercent(v);

  assert(!ok);
  assert(dev.lastError() == TCP0465::ERROR_I2C_WRITE);
}

static void test_read_failure() {
  ScriptedBus bus;

  bus.expectWrite({kCommandReadGas});
  bus.expectReadFail();

  TCP0465 dev(bus);
  dev.begin(bus, 0x50);

  float v = 0;
  bool ok = dev.readOxygenPercent(v);

  assert(!ok);
  assert(dev.lastError() == TCP0465::ERROR_I2C_READ);
}

static void test_short_read() {
  ScriptedBus bus;

  bus.expectWrite({kCommandReadGas});
  bus.expectReadShort(3);

  TCP0465 dev(bus);
  dev.begin(bus, 0x50);

  float v = 0;
  bool ok = dev.readOxygenPercent(v);

  assert(!ok);
  assert(dev.lastError() == TCP0465::ERROR_SHORT_RESPONSE);
}

static void test_mode_switch_reject() {
  ScriptedBus bus;

  bus.expectWrite({
    START_BYTE, PROTOCOL_ADDRESS, kCommandSetMode, kModePassive, 0,0,0,0,0
  });

  bus.expectRead({
    START_BYTE, kCommandSetMode, MODE_SWITCH_REJECTED, 0,0,0,0,0, 0x00
  });

  TCP0465 dev(bus);
  dev.begin(bus, 0x50);

  bool ok = dev.setPassiveMode();
  assert(!ok);
  assert(dev.lastError() == TCP0465::ERROR_MODE_SWITCH_REJECTED);
}

int main() {
  test_write_failure();
  test_read_failure();
  test_short_read();
  test_mode_switch_reject();
  return 0;
}