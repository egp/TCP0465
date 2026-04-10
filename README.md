# TCP0465

TCP0465 is an Arduino library for the DFRobot SEN0465 factory-calibrated oxygen sensor.

This project is intentionally narrow in scope:

- sensor: SEN0465 only
- bus: I2C only
- mode: passive/query mode only
- measurement: oxygen concentration only
- one sensor on the bus
- default address: `0x74` unless overridden

The implementation is intended to be a clean-room implementation based on the published protocol documentation, not a wrapper around the vendor library.

## Current status

Current development status:

- host-side protocol parsing tests are green
- host-side pure-core API tests are green
- the host-testable core API has been implemented and tested
- Arduino-library discovery / integration is still being diagnosed
- end-to-end hardware validation is still pending

In other words:

- the parsing and core behavior are reasonably far along
- the Arduino-facing wrapper and normal Arduino library import path are not yet fully signed off

## Sensor notes

Important warm-up / preheat guidance from the SEN0465 documentation:

- first power-up needs more than 5 minutes of warm-up
- long-unused modules are recommended to preheat for more than 24 hours

Do not trust measurements until the sensor has had adequate warm-up time.

The SEN0465 reports oxygen concentration over a calibrated digital interface.
The expected oxygen range is 0 to 25 `%Vol`.

## Intended public API

The intended Arduino-facing API is:

- `bool begin(TwoWire& wire = Wire, uint8_t address = DEFAULT_ADDRESS);`
- `bool readOxygenPercent(float& percentVol);`
- `ErrorCode lastError() const;`
- `const char* errorString() const;`
- `uint8_t address() const;`

### `begin(...)`

Purpose:

- bind the library to an I2C bus
- remember the configured device address
- prepare the sensor for passive/query reads

Nominal usage:

- call once from `setup()`
- use the default address unless your module has been configured differently

### `readOxygenPercent(...)`

Purpose:

- perform one oxygen read in passive/query mode
- return the oxygen concentration in `%Vol`

Nominal usage:

- call after a successful `begin()`
- on success, `percentVol` receives the decoded oxygen concentration
- on failure, inspect `lastError()` or `errorString()`

### Error reporting

The library exposes a small embedded-friendly error model.

Current error enum:

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

The exact behavior of each code is still subject to refinement while the Arduino-facing transport layer is being finished.

## Wiring

Use I2C mode on the sensor board.

Typical connections:

- VCC -> 3.3V or 5V
- GND -> GND
- SDA -> controller SDA
- SCL -> controller SCL

The module must be configured for I2C mode.

## Current restrictions

This library is not a full-featured multi-gas framework.

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
- no calibration workflow beyond decoding the factory-calibrated digital output

Current project restriction:

- host-side tests are the source of truth right now
- normal Arduino IDE / Arduino CLI import and compile flow is not yet fully validated

## What has been tested so far

The host-side test suite currently covers:

- protocol checksum logic
- validation of passive oxygen response frames
- rejection of malformed or invalid frames
- pure-core API behavior for:
  - `address()`
  - `lastError()`
  - `errorString()`
  - `begin()`
  - `readOxygenPercent()`

This means the low-level protocol and pure-core behavior are being developed with TDD even though hardware access is currently limited.

## Example sketch shape

The intended sketch pattern is:

1. construct `TCP0465`
2. call `begin()`
3. repeatedly call `readOxygenPercent(percentVol)`
4. print `%Vol` on success
5. print `errorString()` on failure

A minimal sketch looks like this conceptually:

    #include <TCP0465.h>

    TCP0465 oxygen;

    void setup() {
      Serial.begin(115200);

      if (!oxygen.begin()) {
        Serial.print("begin() failed: ");
        Serial.println(oxygen.errorString());
        while (true) {
          delay(1000);
        }
      }
    }

    void loop() {
      float percentVol = 0.0f;

      if (oxygen.readOxygenPercent(percentVol)) {
        Serial.print("O2: ");
        Serial.print(percentVol, 1);
        Serial.println(" %Vol");
      } else {
        Serial.print("read failed: ");
        Serial.println(oxygen.errorString());
      }

      delay(1000);
    }

## Development

Requirements:

- Arduino CLI
- a supported Arduino core installed locally
- a host C++ compiler such as `c++` or `g++`

Common commands:

- `make host-test`
- `make test`
- `make compile`
- `make compile-all`
- `make ci`
- `make clean`

At the moment, `make test` is the most trustworthy development signal because host-side tests are currently green while Arduino library discovery / integration is still under investigation.

## Repository layout

- `src/` library source
- `examples/` Arduino examples
- `tests/host/` host-side tests
- `.github/workflows/ci.yml` CI workflow
- `library.properties` Arduino library metadata
- `keywords.txt` Arduino IDE keyword highlighting

## Release / publish note

The library source and host tests are far enough along to continue polishing and publishing the repository itself.

However, before treating the library as fully released for ordinary Arduino IDE / Arduino CLI installation, the remaining Arduino-side integration issue should be resolved and re-tested through the normal library import path.

## License

MIT