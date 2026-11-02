// [ru] AlashADS1115: автоподбор диапазона. Один и тот же вход показывает и 3 милливольта, и 3 вольта с наилучшей точностью.
//      Подключите к A0 потенциометр (края на питание и GND) и крутите. Печатается напряжение и использованный диапазон.
// [kk] AlashADS1115: диапазонды автоматты таңдау. Бір кіріс 3 милливольтты да, 3 вольтты да ең жақсы дәлдікпен көрсетеді.
//      A0-ге потенциометр қосыңыз (шеттері қуат пен GND-ға) және бұраңыз. Кернеу мен қолданылған диапазон басылады.
// [en] AlashADS1115: automatic range selection. The same input shows both 3 millivolts and 3 volts with the best precision.
//      Connect a potentiometer to A0 (ends to supply and GND) and turn it. The voltage and the range used are printed.
#include <AlashADS1115.h>

AlashADS1115 adc;

void setup() {
  Serial.begin(115200);
  if (!adc.begin()) {
    Serial.println(adc.errorText());
    while (true) delay(1000);
  }
  adc.setAutoGain(true);
}

void loop() {
  float v = adc.voltage(0);
  Serial.print(v, 5);
  Serial.print(F(" В, диапазон ±"));
  Serial.print(adc.fullScale(), 3);
  Serial.print(F(" В, шаг "));
  Serial.print(adc.lsb() * 1e6, 1);
  Serial.println(F(" мкВ"));
  delay(300);
}
