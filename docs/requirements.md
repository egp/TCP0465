# requirements.md

# TCP0465 Requirements

## 1. Purpose

TCP0465 is an Arduino library for the DFRobot **SEN0465** factory-calibrated electrochemical oxygen sensor.

The initial goal is a **small, clean, reliable library** for reading **oxygen concentration only** over **I2C** in **passive mode**.

This library is intentionally narrower than the vendor offering:

- **sensor**: SEN0465 only
- **transport**: I2C only
- **mode**: passive/query mode only
- **measurement**: gas concentration only via command `0x86`
- **single device**: one sensor on the bus, expected at the default address unless configured otherwise by the caller

---

## 2. Scope

### 2.1 In Scope for MVP

The MVP shall:

1. Initialize communication with a SEN0465 over `TwoWire`.
2. Read oxygen concentration from the sensor using the documented passive-mode gas command.
3. Validate and parse the returned frame.
4. Return oxygen concentration as a numeric value in `%Vol`.
5. Report errors for communication failure, malformed frames, unexpected gas type, and checksum failure.
6. Document the required warm-up / preheat guidance for correct use.

### 2.2 Explicitly Out of Scope for MVP

The MVP shall **not** implement:

- UART support
- active/initiative reporting mode
- temperature reads (`0x87`)
- combined gas+temperature reads (`0x88`)
- threshold alarm configuration or alarm pin support
- analog output support
- dynamic address reconfiguration
- multi-sensor discovery / scanning
- asynchronous polling, interrupts, or buffering
- dependency on the vendor library

These may be added later, but are not part of the initial acceptance criteria.

---

## 3. External Behavior

### 3.1 Primary Use Case

A user shall be able to:

1. construct a TCP0465 object,
2. call `begin()` with a `TwoWire` instance and optional I2C address,
3. call `readOxygenPercent()` or equivalent,
4. receive either:
   - a valid oxygen concentration in `%Vol`, or
   - a failure indication with an actionable error code.

### 3.2 Sensor Assumptions

For MVP, the library may assume:

- the hardware is a genuine **SEN0465**
- the module communication selector is configured for **I2C**
- the sensor is used as a **single device**
- the default I2C address is normally used
- the sensor has been allowed adequate warm-up time before trusting readings

The library shall not try to enforce warm-up timing internally, but the documentation shall make the requirement explicit.

---

## 4. Functional Requirements

### FR-1 Library Identity

The library shall be named **TCP0465**.

### FR-2 Supported Hardware

The library shall target the DFRobot **SEN0465** oxygen sensor and no other gas variant in the initial release.

### FR-3 Communication Method

The library shall support **I2C** communication only in the initial release.

### FR-4 Passive Read Command

The library shall obtain gas concentration using the documented passive-mode gas-read command `0x86`.

### FR-5 Frame Length

The library shall transmit and receive the documented fixed-width command/response frame for passive gas reads.

### FR-6 Concentration Decoding

The library shall decode gas concentration from the returned high-byte and low-byte fields and apply the returned decimal-place field to compute the concentration in `%Vol`.

### FR-7 Gas Type Validation

The library shall verify that the returned gas type corresponds to **oxygen**.  
If the returned gas type is not oxygen, the read shall fail with a distinct error.

### FR-8 Checksum Validation

The library shall validate the response checksum before accepting a reading.  
A checksum mismatch shall cause the read to fail with a distinct error.

### FR-9 Start/Command Byte Validation

The library shall validate key response bytes required to establish that the frame is a valid reply to the gas-read command.  
An invalid response frame shall cause the read to fail with a distinct error.

### FR-10 I2C Address Handling

The library shall default to the sensor’s standard/default I2C address, while allowing an alternate address to be supplied by the caller.

### FR-11 No Hidden Vendor Dependency

The library shall not require the DFRobot vendor library to build or run.

### FR-12 Deterministic API

The library shall expose a small, synchronous API with deterministic behavior suitable for typical Arduino sketches.

---

## 5. API Requirements

The exact names may change, but the MVP API shall provide equivalents of the following.

### 5.1 Initialization

A function equivalent to `begin()` that accepts a `TwoWire` instance and optional I2C address.

Requirements:

- stores the selected bus and address
- performs any minimal setup required for subsequent reads
- does not depend on temperature/alarm/active mode support
- returns success/failure

### 5.2 Read Oxygen Concentration

A function equivalent to `readOxygenPercent()`.

Requirements:

- performs one passive read transaction
- validates the response
- writes `%Vol` concentration on success
- returns failure on any communication or protocol error

### 5.3 Error Reporting

The library shall provide a way to retrieve the last error.

The exact shape is open, but callers shall be able to distinguish at least:

- no error
- I2C write failure
- I2C read failure
- short response
- invalid start byte
- unexpected command byte
- checksum mismatch
- unexpected gas type
- invalid decimal field / unparseable payload

---

## 6. Documentation Requirements

### DR-1 Warm-Up Guidance

The documentation shall explicitly state:

- **first power-up needs >5 minutes warm-up**
- **long-unused modules are recommended to preheat for >24 hours**

### DR-2 Operating Expectations

The documentation shall state that:

- the library reads the **factory-calibrated digital output**
- the MVP uses **I2C passive mode**
- the module must be configured for **I2C**
- the library does not support alarm, temperature, UART, or analog output in the initial release

### DR-3 Basic Example

The documentation shall include a minimal example sketch showing:

- object construction
- `begin()`
- periodic oxygen reads
- serial printing of `%Vol`
- failure handling

### DR-4 Wiring Notes

The documentation shall include a concise wiring section for:

- VCC
- GND
- SDA
- SCL
- note that the module must be set to I2C mode

---

## 7. Error Handling Requirements

### ER-1 No Silent Acceptance of Bad Frames

The library shall not silently accept malformed or partially valid response frames.

### ER-2 No Silent Gas-Type Mismatch

The library shall not silently treat non-oxygen gas type responses as oxygen measurements.

### ER-3 No Exceptions

The library shall not depend on C++ exceptions.

### ER-4 Arduino-Friendly Failure Model

Failures shall be represented using boolean return values, status codes, or similarly lightweight embedded-friendly mechanisms.

---

## 8. Design Constraints

### DC-1 Clean-Room Preference

The implementation should be a clean-room implementation derived from official protocol documentation.

### DC-2 Vendor Code Policy

Vendor code may be consulted only to resolve ambiguity or confirm edge-case behavior when the documentation is unclear.  
The implementation shall not become a thin wrapper around the vendor library.

### DC-3 Small Surface Area

The library should favor a minimal public API and small codebase over broad feature coverage in the MVP.

### DC-4 Resource Awareness

The implementation should be reasonable for small microcontroller targets:

- low RAM footprint
- no dynamic allocation unless clearly justified
- no background tasks
- no large buffers beyond what the protocol requires

---

## 9. Non-Goals

The following are non-goals for the first release:

- generic multi-gas abstraction
- support for all sensors in the DFRobot electrochemical family
- aggressive auto-retry / recovery policies
- calibration or correction logic beyond decoding documented factory-calibrated values
- sensor health estimation
- averaging, filtering, or smoothing beyond raw single-read acquisition

---

## 10. Acceptance Criteria for MVP

The MVP is acceptable when all of the following are true:

1. A user can connect one SEN0465 in I2C mode and read oxygen concentration in `%Vol`.
2. The library uses passive mode gas reads only.
3. Response frames are validated for structure, checksum, and gas type.
4. Bad frames fail cleanly and predictably.
5. The README or equivalent documentation includes the warm-up/preheat guidance.
6. The build does not depend on the vendor library.
7. At least one example sketch compiles and demonstrates successful periodic reading.

---

## 11. Open Questions

These items are intentionally left open for later refinement:

1. Should the public API return `float`, fixed-point integer, or both?
2. Should `begin()` perform a probe/read test, or only store configuration?
3. Should the library expose raw frame bytes for diagnostics?
4. Should the library include optional retry-once behavior for transient I2C failures?
5. Should unit tests include captured sample frames from hardware?
6. Should the API expose the configured I2C address and bus for diagnostics?

---

## 12. Initial Implementation Bias

Unless later evidence suggests otherwise, the first implementation should bias toward:

- correctness over feature breadth
- explicit validation over permissive parsing
- synchronous single-shot reads
- one obvious path for the user
- small and readable code suitable for audit and maintenance