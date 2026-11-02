// [ru] AlashADS1115: самый простой пример. Напряжение на четырёх входах точнее, чем analogRead() (16 бит вместо 10).
//
//      Подключение: VDD к 3.3 В или 5 В, GND к GND, SDA к A4 (ESP32: GPIO21), SCL к A5 (ESP32: GPIO22),
//      ADDR к GND (адрес 0x48). Измеряемые сигналы на A0..A3. Напряжение на входе не больше питания АЦП + 0.3 В.
// [kk] AlashADS1115: ең қарапайым мысал. Төрт кірістегі кернеу analogRead()-тен дәлірек (10 орнына 16 бит).
//
//      Қосу: VDD 3.3 В немесе 5 В-қа, GND GND-ға, SDA A4-ке (ESP32: GPIO21), SCL A5-ке (ESP32: GPIO22),
//      ADDR GND-ға (мекенжай 0x48). Өлшенетін сигналдар A0..A3-те. Кіріс кернеуі АЦП қуаты + 0.3 В-тан аспауы керек.
// [en] AlashADS1115: the simplest example. The voltage on four inputs, more precise than analogRead() (16 bits instead of 10).
//
//      Wiring: VDD to 3.3 V or 5 V, GND to GND, SDA to A4 (ESP32: GPIO21), SCL to A5 (ESP32: GPIO22),
//      ADDR to GND (address 0x48). Measured signals on A0..A3. The input voltage must not exceed the ADC supply + 0.3 V.
#include <AlashADS1115.h>

AlashADS1115 adc;

void setup() {
  Serial.begin(115200);
  if (!adc.begin()) {                     // сам находит АЦП на адресах 0x48-0x4B
    Serial.println(adc.errorText());      // объяснит, что проверить
    while (true) delay(1000);
  }
  adc.setAutoGain(true);                  // сам выбирает самый точный диапазон под сигнал
}

void loop() {
  for (uint8_t ch = 0; ch < 4; ch++) {
    Serial.print(F("A"));
    Serial.print(ch);
    Serial.print(F(" = "));
    Serial.print(adc.voltage(ch), 4);
    Serial.print(F(" В   "));
  }
  Serial.println();
  delay(500);
}
