// AlashADS1115 на ESP32 со своими пинами I2C: begin(SDA, SCL). АЦП ADS1115 точнее встроенного АЦП ESP32 и не зависит от Wi-Fi.
#include <AlashADS1115.h>

AlashADS1115 adc;

void setup() {
  Serial.begin(115200);
  if (!adc.begin(21, 22)) {               // SDA = GPIO21, SCL = GPIO22
    Serial.println(adc.errorText());
    while (true) delay(1000);
  }
  adc.setAutoGain(true);
}

void loop() {
  Serial.println(adc.voltage(0), 4);
  delay(500);
}
