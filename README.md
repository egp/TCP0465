# TCP0465

TCP0465 is an Arduino library for the DFRobot SEN0465 factory-calibrated oxygen sensor.

This library is intentionally narrow in scope:

- sensor: SEN0465 only
- bus: I2C only
- mode: passive/query mode only
- measurement: oxygen concentration only
- one sensor per `TCP0465` object
- default device address: `0x74`

The implementation is a clean-room implementation of the documented protocol. It is not a wrapper around the vendor library.

## Companion dependency

TCP0465 currently uses the `TCP1819` companion library for the Arduino-facing I2C transport types and helpers.

The public API uses:

- `BBI2C`
- `I2CInit(...)`
- `I2CWrite(...)`
- `I2CRead(...)`

Install both libraries before building the examples.

## Features

- initialize the sensor in passive/query mode
- read oxygen concentration as `%Vol`
- compact embedded-friendly error model
- basic read example
- hardware smoke-test example

## Public API

class TCP0465 {
public:
  static const uint8_t DEFAULT_ADDRESS = 0x74;
  static const uint8_t EXPECTED_GAS_TYPE_OXYGEN = 0x05;

  enum ErrorCode : uint8_t {
    ERROR_NONE = 0,
    ERROR_NOT_INITIALIZED,
    ERROR_I2C_WRITE,
    ERROR_I2C_READ,
    ERROR_SHORT_RESPONSE,
    ERROR_INVALID_START_BYTE,
    ERROR_UNEXPECTED_COMMAND,
    ERROR_CHECKSUM_MISMATCH,
    ERROR_UNEXPECTED_GAS_TYPE,
    ERROR_INVALID_DECIMALS,
    ERROR_MODE_SWITCH_FAILED,
    ERROR_MODE_SWITCH_REJECTED
  };

  TCP0465();
  bool begin(BBI2C& i2c, uint8_t address = DEFAULT_ADDRESS);
  bool readOxygenPercent(float& percentVol);
  ErrorCode lastError() const;
  const char* errorString() const;
  uint8_t address() const;
};

## Installation

Install these libraries side by side in your Arduino libraries directory:

- `TCP0465`
- `TCP1819`

Example layout:

- `~/Documents/Arduino/libraries/TCP0465`
- `~/Documents/Arduino/libraries/TCP1819`

## Wiring

Use the SEN0465 in I2C mode.

Typical connections:

- VCC -> 3.3V or 5V
- GND -> GND
- SDA -> chosen controller GPIO for the TCP1819 I2C bus
- SCL -> chosen controller GPIO for the TCP1819 I2C bus

The current examples configure the bus pins in the sketch, so they are not limited to the board’s default hardware SDA/SCL pins.

## Sensor warm-up notes

Important guidance from the SEN0465 documentation:

- first power-up needs more than 5 minutes of warm-up
- long-unused modules are recommended to preheat for more than 24 hours

Do not trust measurements until the sensor has had adequate warm-up time.

## Minimal example

#include <TCP1819.h>
#include <TCP0465.h>

namespace {
constexpr uint8_t kOxygenSdaPin = 10;
constexpr uint8_t kOxygenSclPin = 11;
constexpr uint32_t kOxygenI2cHz = 100000;
}

BBI2C oxygenBus;
TCP0465 oxygen;

static void setupOxygenBus() {
  memset(&oxygenBus, 0, sizeof(oxygenBus));
  oxygenBus.bWire = 0;
  oxygenBus.iSDA = kOxygenSdaPin;
  oxygenBus.iSCL = kOxygenSclPin;
  I2CInit(&oxygenBus, kOxygenI2cHz);
}

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 5000) {
    delay(10);
  }

  setupOxygenBus();

  if (!oxygen.begin(oxygenBus)) {
    Serial.print("begin() failed: ");
    Serial.println(oxygen.errorString());
    while (true) {
      delay(1000);
    }
  }

  Serial.println("Sensor initialized.");
}

void loop() {
  float percentVol = 0.0f;

  if (oxygen.readOxygenPercent(percentVol)) {
    Serial.print("O2 concentration: ");
    Serial.print(percentVol, 1);
    Serial.println(" %Vol");
  } else {
    Serial.print("readOxygenPercent() failed: ");
    Serial.println(oxygen.errorString());
  }

  delay(1000);
}

## API notes

### begin(...)

Purpose:

- bind the library to an initialized `BBI2C` bus
- remember the configured sensor address
- switch the sensor into passive/query mode

Nominal usage:

- configure and initialize the `BBI2C` bus first
- call `begin(...)` once from `setup()`
- use the default address unless your module has been reconfigured

### readOxygenPercent(...)

Purpose:

- perform one passive/query read
- decode and return oxygen concentration in `%Vol`

Nominal usage:

- call after a successful `begin(...)`
- on success, `percentVol` receives the decoded oxygen concentration
- on failure, inspect `lastError()` or `errorString()`

### Error reporting

The library exposes this error model:

- `ERROR_NONE`
- `ERROR_NOT_INITIALIZED`
- `ERROR_I2C_WRITE`
- `ERROR_I2C_READ`
- `ERROR_SHORT_RESPONSE`
- `ERROR_INVALID_START_BYTE`
- `ERROR_UNEXPECTED_COMMAND`
- `ERROR_CHECKSUM_MISMATCH`
- `ERROR_UNEXPECTED_GAS_TYPE`
- `ERROR_INVALID_DECIMALS`
- `ERROR_MODE_SWITCH_FAILED`
- `ERROR_MODE_SWITCH_REJECTED`

Use `errorString()` for a human-readable summary.

## Examples

### BasicRead

Simple once-per-second oxygen reading example.

### HardwareSmoke

Interactive serial smoke test for bring-up and manual debugging.

Commands:

- `?` help
- `r` single oxygen read
- `a` toggle auto-read once per second
- `b` re-run `begin()`

## Restrictions

This is not a full-featured multi-gas framework.

Current restrictions:

- SEN0465 only
- I2C only
- passive/query mode only
- oxygen concentration only
- no UART support
- no active/streaming mode support
- no temperature reads
- no alarm support
- no analog-output support
- no multi-sensor discovery
- no dynamic address management

## Development

Repository layout:

- `src/` library source
- `examples/` Arduino examples
- `tests/host/` host-side tests
- `.github/workflows/ci.yml` CI workflow
- `library.properties` Arduino library metadata
- `keywords.txt` Arduino IDE keyword highlighting

Common local commands:

make test
make compile TCP1819_DIR=../TCP1819
make ci TCP1819_DIR=../TCP1819
make clean

## License

MIT License. See `LICENSE`.