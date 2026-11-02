// [ru] AlashADS1115: АЦП не работает? Скетч покажет устройства на шине I2C, напряжения на входах и подскажет, что проверить.
// [kk] AlashADS1115: АЦП жұмыс істемей ме? Скетч I2C шинасындағы құрылғыларды, кіріс кернеулерін көрсетеді және нені тексеру керегін айтады.
// [en] AlashADS1115: the ADC does not work? The sketch shows the devices on the I2C bus, the input voltages and tells what to check.
#include <AlashADS1115.h>

AlashADS1115 adc;

void setup() {
  Serial.begin(115200);
  delay(500);
  adc.setLanguage(AlashADS1115::LANG_RU);   // LANG_KK: қазақша, LANG_EN: English
  adc.begin();
  adc.diagnose(Serial);
}

void loop() {}
