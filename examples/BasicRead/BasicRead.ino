examples/BasicRead/BasicRead.ino

#include <TCP0465.h>

TCP0465 oxygen;

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 5000) {
    delay(10);
  }

  Serial.println();
  Serial.println("TCP0465 BasicRead");
  Serial.println("Note: first power-up needs >5 minutes warm-up.");
  Serial.println("Note: long-unused modules are recommended to preheat for >24 hours.");

  if (!oxygen.begin()) {
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