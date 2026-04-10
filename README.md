README.md

# TCP0465

TCP0465 is a small Arduino library for the DFRobot SEN0465 factory-calibrated oxygen sensor.

Initial scope:

- SEN0465 only
- I2C only
- passive/query mode only
- oxygen concentration only
- single sensor on the bus
- default address 0x74 unless overridden

The implementation is intended to be a clean-room implementation based on the published protocol documentation, not a wrapper around the vendor library.

## Status

Initial scaffold plus a minimal compile-ready library implementation.

Current API:

- `begin(TwoWire& wire = Wire, uint8_t address = DEFAULT_ADDRESS)`
- `readOxygenPercent(float& percentVol)`
- `lastError()`
- `errorString()`

`begin()` switches the sensor into passive/query mode so later reads can use the passive gas command.

## Warm-up / preheat notes

Important:

- first power-up needs >5 minutes warm-up
- long-unused modules are recommended to preheat for >24 hours

Do not trust measurements until the sensor has had adequate warm-up time.

## Wiring

Use I2C mode on the sensor board.

Typical connections:

- VCC -> 3.3V or 5V
- GND -> GND
- SDA -> controller SDA
- SCL -> controller SCL

The module must be configured for I2C mode.

## Examples

- `examples/BasicRead/BasicRead.ino`  
  Minimal periodic read example.

- `examples/HardwareSmoke/HardwareSmoke.ino`  
  Simple interactive smoke test over Serial.

## Development

Requirements:

- Arduino CLI
- a supported Arduino core installed locally
- a host C++ compiler such as `c++` or `g++`

Common commands:

- `make setup`
- `make host-test`
- `make compile`
- `make compile-all`
- `make ci`
- `make clean`

By default the Makefile compiles the examples for:

- arduino:avr:uno
- arduino:avr:mega

The host test target exercises the pure protocol parser without Arduino dependencies.

## Repository layout

- `src/` library source
- `examples/BasicRead/` minimal example sketch
- `examples/HardwareSmoke/` interactive hardware smoke sketch
- `tests/host/` host-side protocol parser tests
- `.github/workflows/ci.yml` GitHub Actions CI
- `library.properties` Arduino library metadata
- `keywords.txt` Arduino IDE keyword highlighting
- `requirements.md` initial requirements draft

## License

MIT