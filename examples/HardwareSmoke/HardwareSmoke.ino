// examples/HardwareSmoke/HardwareSmoke.ino V2
#include <TCP1819.h>
#include <TCP0465.h>

static constexpr uint8_t OXYGEN_SDA_PIN = 5;
static constexpr uint8_t OXYGEN_SCL_PIN = 6;
static constexpr uint32_t OXYGEN_I2C_HZ = 100000;

BBI2C oxygenBus;
TCP0465 oxygen;

bool autoRead = false;
unsigned long nextReadAtMs = 0;

void printHelp() {
  Serial.println();
  Serial.println("HardwareSmoke commands:");
  Serial.println(" ? help");
  Serial.println(" r single oxygen read");
  Serial.println(" a toggle auto-read once per second");
  Serial.println(" b re-run begin()");
  Serial.println();
}

void performRead() {
  float percentVol = 0.0f;
  if (oxygen.readOxygenPercent(percentVol)) {
    Serial.print("O2: ");
    Serial.print(percentVol, 1);
    Serial.println(" %Vol");
  } else {
    Serial.print("read failed: ");
    Serial.println(oxygen.errorString());
  }
}

void setupOxygenBus() {
  memset(&oxygenBus, 0, sizeof(oxygenBus));
  oxygenBus.bWire = 0;              // software/bit-bang I2C
  oxygenBus.iSDA = OXYGEN_SDA_PIN;
  oxygenBus.iSCL = OXYGEN_SCL_PIN;
  I2CInit(&oxygenBus, OXYGEN_I2C_HZ);
}

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 5000) {
    delay(10);
  }

  setupOxygenBus();

  Serial.println();
  Serial.println("TCP0465 HardwareSmoke");
  Serial.println("Important: first power-up needs >5 minutes warm-up.");
  Serial.println("Important: long-unused modules are recommended to preheat for >24 hours.");
  Serial.println();

  if (!oxygen.begin(oxygenBus)) {
    Serial.print("begin() failed: ");
    Serial.println(oxygen.errorString());
  } else {
    Serial.println("begin() ok");
  }

  printHelp();
}

void loop() {
  while (Serial.available() > 0) {
    const char c = static_cast<char>(Serial.read());
    switch (c) {
      case '?':
      case 'h':
      case 'H':
        printHelp();
        break;

      case 'r':
      case 'R':
        performRead();
        break;

      case 'a':
      case 'A':
        autoRead = !autoRead;
        Serial.print("autoRead = ");
        Serial.println(autoRead ? "on" : "off");
        break;

      case 'b':
      case 'B':
        if (!oxygen.begin(oxygenBus)) {
          Serial.print("begin() failed: ");
          Serial.println(oxygen.errorString());
        } else {
          Serial.println("begin() ok");
        }
        break;

      case '\r':
      case '\n':
      case ' ':
      case '\t':
        break;

      default:
        Serial.print("unknown command: ");
        Serial.println(c);
        printHelp();
        break;
    }
  }

  if (autoRead && millis() >= nextReadAtMs) {
    nextReadAtMs = millis() + 1000UL;
    performRead();
  }
}

// examples/HardwareSmoke/HardwareSmoke.ino V2