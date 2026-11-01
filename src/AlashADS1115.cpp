// MIT License. Alash Electronics.
#include "AlashADS1115.h"

#define T(ru, kk, en) (_lang == LANG_KK ? F(kk) : _lang == LANG_EN ? F(en) : F(ru))

static const uint8_t REG_CONVERSION = 0x00;
static const uint8_t REG_CONFIG = 0x01;
static const uint16_t CFG_OS = 0x8000;           // запуск преобразования / бит «готово»
static const uint16_t CFG_SINGLE = 0x0100;       // разовый режим
static const uint16_t CFG_COMP_OFF = 0x0003;     // компаратор выключен

float AlashADS1115::fullScaleOf(Gain g) {
  static const float fs[6] = {6.144f, 4.096f, 2.048f, 1.024f, 0.512f, 0.256f};
  return fs[g > 5 ? 5 : g];
}
float AlashADS1115::fullScale() const { return fullScaleOf(_gain); }
float AlashADS1115::lsb() const { return fullScaleOf(_gain) / (_chip == ADS1115 ? 32768.0f : 2048.0f); }

bool AlashADS1115::begin(TwoWire &wire, bool startWire) {
  _wire = &wire;
  if (startWire) _wire->begin();
  Error first = ERR_NO_DEVICE;
  for (uint8_t a = 0x48; a <= 0x4B; a++) {
    if (probe(a)) return true;
    if (_err == ERR_WRONG_CHIP) first = ERR_WRONG_CHIP;
  }
  _err = first;
  return false;
}

bool AlashADS1115::begin(uint8_t address, TwoWire &wire, bool startWire) {
  _wire = &wire;
  if (startWire) _wire->begin();
  return probe(address);
}

#if defined(ESP32) || defined(ESP8266)
bool AlashADS1115::begin(int sdaPin, int sclPin) {
  Wire.begin(sdaPin, sclPin);
  return begin(Wire, false);
}
#endif

// Проверка: запишем в регистр настройки известное значение и прочитаем обратно. У чужой микросхемы (INA219 и др.) так не получится.
bool AlashADS1115::probe(uint8_t address) {
  _addr = address;
  _wire->beginTransmission(address);
  if (_wire->endTransmission() != 0) {
    _err = ERR_NO_DEVICE;
    return false;
  }
  const uint16_t test = CFG_SINGLE | (2 << 9) | (4 << 5) | CFG_COMP_OFF;     // ±2.048 В, 128 SPS, разовый режим
  uint16_t back = 0;
  if (!writeReg(REG_CONFIG, test) || !readReg(REG_CONFIG, back)) {
    _err = ERR_I2C;
    return false;
  }
  if ((back & 0x7FFF) != test) {
    _err = ERR_WRONG_CHIP;
    return false;
  }
  _started = true;
  _err = ERR_OK;
  return true;
}

bool AlashADS1115::isConnected() {
  _wire->beginTransmission(_addr);
  return _wire->endTransmission() == 0;
}

bool AlashADS1115::readReg(uint8_t reg, uint16_t &v) {
  _wire->beginTransmission(_addr);
  _wire->write(reg);
  if (_wire->endTransmission(false) != 0) return false;
  if (_wire->requestFrom(_addr, (uint8_t)2) != 2) return false;
  v = (uint16_t)(_wire->read() << 8);
  v |= (uint8_t)_wire->read();
  return true;
}

bool AlashADS1115::writeReg(uint8_t reg, uint16_t v) {
  _wire->beginTransmission(_addr);
  _wire->write(reg);
  _wire->write((uint8_t)(v >> 8));
  _wire->write((uint8_t)(v & 0xFF));
  return _wire->endTransmission() == 0;
}

// Скорость: ADS1115 8, 16, 32, 64, 128, 250, 475, 860; ADS1015 128, 250, 490, 920, 1600, 2400, 3300
void AlashADS1115::setRate(uint16_t sps) {
  static const uint16_t r15[7] = {128, 250, 490, 920, 1600, 2400, 3300};
  static const uint16_t r11[8] = {8, 16, 32, 64, 128, 250, 475, 860};
  uint8_t best = 0, n = _chip == ADS1115 ? 8 : 7;
  for (uint8_t i = 0; i < n; i++) {
    uint16_t v = _chip == ADS1115 ? r11[i] : r15[i];
    if (v <= sps) best = i;                         // ближайшая скорость не выше запрошенной
  }
  _rate = best;
}

uint16_t AlashADS1115::conversionMs() const {
  static const uint16_t r15[7] = {128, 250, 490, 920, 1600, 2400, 3300};
  static const uint16_t r11[8] = {8, 16, 32, 64, 128, 250, 475, 860};
  uint16_t sps = _chip == ADS1115 ? r11[_rate > 7 ? 7 : _rate] : r15[_rate > 6 ? 6 : _rate];
  return (uint16_t)(1000UL / sps + 2);               // время преобразования плюс запас
}

void AlashADS1115::setChannelScale(uint8_t channel, float factor) {
  if (channel < 4) _scale[channel] = factor;
}

// Одно преобразование: запускаем, ждём бит готовности, читаем результат
bool AlashADS1115::convert(uint8_t mux, Gain g, int16_t &raw) {
  uint16_t cfg = CFG_OS | CFG_SINGLE | CFG_COMP_OFF | ((uint16_t)mux << 12) | ((uint16_t)g << 9) | ((uint16_t)_rate << 5);
  if (!writeReg(REG_CONFIG, cfg)) {
    _err = ERR_I2C;
    return false;
  }
  uint32_t t0 = millis();
  uint16_t limit = conversionMs() * 3 + 20;
  delay(conversionMs() > 3 ? conversionMs() - 2 : 0);
  while (true) {
    uint16_t c = 0;
    if (!readReg(REG_CONFIG, c)) {
      _err = ERR_I2C;
      return false;
    }
    if (c & CFG_OS) break;
    if ((uint32_t)(millis() - t0) > limit) {
      _err = ERR_TIMEOUT;
      return false;
    }
    delay(1);
  }
  uint16_t v = 0;
  if (!readReg(REG_CONVERSION, v)) {
    _err = ERR_I2C;
    return false;
  }
  raw = (int16_t)v;
  if (_chip == ADS1015) raw >>= 4;                  // 12 значащих бит, старшие
  return true;
}

// Измерение с автоподбором диапазона. Возвращает сырое значение, fs = полная шкала использованного диапазона, volts = напряжение
int16_t AlashADS1115::readMux(uint8_t mux, float &fs, float &volts) {
  const float div = _chip == ADS1115 ? 32768.0f : 2048.0f;
  Gain g = _auto ? RANGE_6V : _gain;
  int16_t raw = 0;
  if (!convert(mux, g, raw)) {
    volts = NAN;
    return 0;
  }
  if (_auto) {
    float v = fabsf(raw * fullScaleOf(g) / div);
    Gain best = RANGE_0_25V;                         // самый узкий диапазон, в который сигнал влезает с запасом 5 %
    for (uint8_t i = 0; i <= 5; i++)
      if (fullScaleOf((Gain)i) * 0.95f >= v) best = (Gain)i;
    if (best != g) {
      g = best;
      if (!convert(mux, g, raw)) {
        volts = NAN;
        return 0;
      }
    }
    _gain = g;
  }
  fs = fullScaleOf(g);
  volts = raw * fs / div;
  // Упёрлись в потолок диапазона: настоящее напряжение выше
  _err = (raw >= (int16_t)(div - 2) || raw <= (int16_t)(-div + 1)) ? ERR_CLIPPED : ERR_OK;
  return raw;
}

int16_t AlashADS1115::readRaw(uint8_t channel) {
  if (!_started || channel > 3) {
    _err = _started ? ERR_BAD_CHANNEL : ERR_NO_DEVICE;
    return 0;
  }
  float fs, v;
  return readMux(4 + channel, fs, v);
}

float AlashADS1115::voltage(uint8_t channel) {
  if (!_started || channel > 3) {
    _err = _started ? ERR_BAD_CHANNEL : ERR_NO_DEVICE;
    return NAN;
  }
  float fs, v;
  readMux(4 + channel, fs, v);
  return isnan(v) ? NAN : v * _scale[channel];
}

// Допустимые пары: 0-1 (код 0), 0-3 (1), 1-3 (2), 2-3 (3)
static int8_t diffMux(uint8_t p, uint8_t n) {
  if (p == 0 && n == 1) return 0;
  if (p == 0 && n == 3) return 1;
  if (p == 1 && n == 3) return 2;
  if (p == 2 && n == 3) return 3;
  return -1;
}

int16_t AlashADS1115::readRawDiff(uint8_t p, uint8_t n) {
  int8_t m = diffMux(p, n);
  if (!_started || m < 0) {
    _err = _started ? ERR_BAD_CHANNEL : ERR_NO_DEVICE;
    return 0;
  }
  float fs, v;
  return readMux(m, fs, v);
}

float AlashADS1115::voltageDiff(uint8_t p, uint8_t n) {
  int8_t m = diffMux(p, n);
  if (!_started || m < 0) {
    _err = _started ? ERR_BAD_CHANNEL : ERR_NO_DEVICE;
    return NAN;
  }
  float fs, v;
  readMux(m, fs, v);
  return v;
}

// ---- Диагностика ------------------------------------------------------------------------

const __FlashStringHelper *AlashADS1115::errorText() {
  switch (_err) {
    case ERR_OK: return F("OK");
    case ERR_NO_DEVICE:
      return T("АЦП не найден: проверьте SDA / SCL, питание (2-5.5 В) и GND. Адрес 0x48-0x4B зависит от вывода ADDR",
               "АЦП табылмады: SDA / SCL, қуатты (2-5.5 В) және GND тексеріңіз. 0x48-0x4B мекенжайы ADDR шығысына байланысты",
               "ADC not found: check SDA / SCL, power (2-5.5 V) and GND. The address 0x48-0x4B depends on the ADDR pin");
    case ERR_WRONG_CHIP:
      return T("По адресу отвечает другое устройство (например, INA219). Смените адрес выводом ADDR",
               "Мекенжайда басқа құрылғы жауап береді (мысалы, INA219). ADDR шығысымен мекенжайды өзгертіңіз",
               "Another device answers at that address (for example an INA219). Change the address with the ADDR pin");
    case ERR_I2C:
      return T("Ошибка I2C: проверьте контакты и подтяжку линий", "I2C қатесі: байланысты және тартқыш резисторларды тексеріңіз",
               "I2C error: check connections and pull-up resistors");
    case ERR_TIMEOUT:
      return T("АЦП не закончил измерение", "АЦП өлшеуді аяқтамады", "The ADC did not finish converting");
    case ERR_BAD_CHANNEL:
      return T("Вход должен быть от 0 до 3; дифференциальные пары только 0-1, 0-3, 1-3, 2-3",
               "Кіріс 0-ден 3-ке дейін болуы керек; дифференциал жұптар тек 0-1, 0-3, 1-3, 2-3",
               "The input must be 0 to 3; differential pairs are only 0-1, 0-3, 1-3, 2-3");
    case ERR_CLIPPED:
      return T("Напряжение упёрлось в предел диапазона: выберите диапазон больше (setGain) или включите setAutoGain(true). Вход не должен превышать питание АЦП",
               "Кернеу ауқым шегіне тірелді: үлкенірек ауқымды таңдаңыз (setGain) не setAutoGain(true) қосыңыз. Кіріс АЦП қуатынан аспауы керек",
               "The voltage hit the range limit: choose a larger range (setGain) or use setAutoGain(true). The input must not exceed the ADC supply");
  }
  return F("");
}

void AlashADS1115::diagnose(Print &out) {
  out.println(T("AlashADS1115: диагностика", "AlashADS1115: диагностика", "AlashADS1115: diagnostics"));
  out.println(T("Найденные устройства I2C:", "Табылған I2C құрылғылары:", "I2C devices found:"));
  for (uint8_t a = 1; a < 127; a++) {
    _wire->beginTransmission(a);
    if (_wire->endTransmission() == 0) {
      out.print(F("  0x"));
      if (a < 16) out.print('0');
      out.print(a, HEX);
      if (a >= 0x48 && a <= 0x4B) out.print(F("  ADS1115 / ADS1015 / INA219"));
      else if (a == 0x3C || a == 0x3D) out.print(F("  OLED SSD1306"));
      else if (a == 0x68) out.print(F("  DS3231 / MPU6050"));
      else if (a == 0x76 || a == 0x77) out.print(F("  BME280 / BMP280"));
      out.println();
    }
  }
  if (!_started) {
    out.println(errorText());
    return;
  }
  out.print(T("АЦП найден, адрес 0x", "АЦП табылды, мекенжайы 0x", "ADC found, address 0x"));
  out.println(_addr, HEX);
  for (uint8_t ch = 0; ch < 4; ch++) {
    out.print(F("  A"));
    out.print((int)ch);
    out.print(F(": "));
    float v = voltage(ch);
    if (isnan(v)) out.println(errorText());
    else {
      out.print(v, 4);
      out.println(F(" V"));
    }
  }
  out.println(T("Вход «висит в воздухе» показывает случайные значения: это нормально, подключите его к источнику или к GND. Не подавайте на вход больше напряжения питания АЦП.",
                "Ауада ілулі кіріс кездейсоқ мәндерді көрсетеді: бұл қалыпты, оны көзге не GND-ға қосыңыз. Кіріске АЦП қуатынан артық кернеу бермеңіз.",
                "A floating input shows random values: that is normal, connect it to a source or GND. Never feed an input more than the ADC supply."));
}
