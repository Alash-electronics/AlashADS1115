// [ru] AlashADS1115: дифференциальное измерение. Напряжение между двумя входами (например, мостовой датчик, шунт или тензодатчик).
//      Пары: 0-1, 0-3, 1-3, 2-3. Знак показывает полярность: минус, если на втором входе напряжение выше.
//      Для слабых сигналов (милливольты) выберите узкий диапазон: так шаг будет 7.8 мкВ.
// [kk] AlashADS1115: дифференциалды өлшеу. Екі кіріс арасындағы кернеу (мысалы, көпір датчигі, шунт немесе тензодатчик).
//      Жұптар: 0-1, 0-3, 1-3, 2-3. Таңба полярлықты көрсетеді: екінші кірісте кернеу жоғары болса минус.
//      Әлсіз сигналдар (милливольт) үшін тар диапазон таңдаңыз: сонда қадам 7.8 мкВ болады.
// [en] AlashADS1115: differential measurement. The voltage between two inputs (for example a bridge sensor, a shunt or a strain gauge).
//      Pairs: 0-1, 0-3, 1-3, 2-3. The sign shows the polarity: negative if the second input has the higher voltage.
//      For weak signals (millivolts) choose a narrow range: then the step is 7.8 uV.
#include <AlashADS1115.h>

AlashADS1115 adc;

void setup() {
  Serial.begin(115200);
  if (!adc.begin()) {
    Serial.println(adc.errorText());
    while (true) delay(1000);
  }
  adc.setGain(AlashADS1115::RANGE_0_25V);   // ±0.256 В
  adc.setRate(64);                          // медленнее, но меньше шума
}

void loop() {
  float mv = adc.voltageDiff(0, 1) * 1000.0;
  if (isnan(mv)) Serial.println(adc.errorText());
  else {
    Serial.print(mv, 3);
    Serial.println(F(" мВ"));
  }
  delay(200);
}
