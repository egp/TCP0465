// examples/BasicRead/BasicRead.ino V3

#include <TCP1819.h>
#include <TCP0465.h>

namespace {
constexpr uint8_t kOxygenSdaPin = 10;
constexpr uint8_t kOxygenSclPin = 11;
constexpr uint32_t kOxygenI2cHz = 100000;
}

BBI2C oxygenBus;
TCP0465 oxygen;

void setupOxygenBus() {
  memset(&oxygenBus, 0, sizeof(oxygenBus));
  oxygenBus.bWire = 0;  // bit-banged I2C
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

  Serial.println();
  Serial.println("TCP0465 BasicRead");
  Serial.println("Note: first power-up needs >5 minutes warm-up.");
  Serial.println("Note: long-unused modules are recommended to preheat for >24 hours.");

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

// examples/BasicRead/BasicRead.ino V3