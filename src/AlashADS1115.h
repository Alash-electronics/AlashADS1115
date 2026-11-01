// AlashADS1115: 16-битный АЦП ADS1115 / ADS1015 (4 входа, I2C): вольты на каждом входе, дифференциальные измерения, автоподбор диапазона.
// Без зависимостей, только встроенная Wire. MIT License. Alash Electronics, https://github.com/Alash-electronics/AlashADS1115
#ifndef ALASH_ADS1115_H
#define ALASH_ADS1115_H

#include <Arduino.h>
#include <Wire.h>

class AlashADS1115 {
 public:
  enum Chip : uint8_t { ADS1115 = 0, ADS1015 = 1 };    // ADS1015 12-битный и быстрее
  // Диапазон входа (полная шкала, В). Напряжение на входе не должно превышать питание датчика + 0.3 В, даже в диапазоне 6.144.
  enum Gain : uint8_t { RANGE_6V = 0, RANGE_4V = 1, RANGE_2V = 2, RANGE_1V = 3, RANGE_0_5V = 4, RANGE_0_25V = 5 };
  enum Error : uint8_t {
    ERR_OK = 0,
    ERR_NO_DEVICE,    // на шине I2C нет ответа по адресам 0x48-0x4B
    ERR_WRONG_CHIP,   // отвечает другое устройство (например, INA219 тоже на 0x48-0x4B)
    ERR_I2C,          // ошибка чтения или записи по I2C
    ERR_TIMEOUT,      // преобразование не закончилось
    ERR_BAD_CHANNEL,  // вход должен быть от 0 до 3
    ERR_CLIPPED       // напряжение у предела диапазона: выберите диапазон больше
  };
  enum Language : uint8_t { LANG_RU = 0, LANG_KK = 1, LANG_EN = 2 };

  explicit AlashADS1115(Chip chip = ADS1115) : _chip(chip) {}

  // ---- Запуск --------------------------------------------------------------
  // begin() находит датчик на адресах 0x48-0x4B (ADDR: GND = 0x48, VDD = 0x49, SDA = 0x4A, SCL = 0x4B) и проверяет, что это АЦП.
  bool begin(TwoWire &wire = Wire, bool startWire = true);
  bool begin(uint8_t address, TwoWire &wire = Wire, bool startWire = true);
#if defined(ESP32) || defined(ESP8266)
  bool begin(int sdaPin, int sclPin);
#endif

  // ---- Измерения ---------------------------------------------------------------
  float voltage(uint8_t channel);           // вольты на входе A0..A3 относительно GND (с учётом setChannelScale)
  int16_t readRaw(uint8_t channel);         // «сырое» значение со знаком (-32768...32767)
  float voltageDiff(uint8_t positive, uint8_t negative);   // разность между двумя входами: пары 0-1, 0-3, 1-3, 2-3
  int16_t readRawDiff(uint8_t positive, uint8_t negative);

  // ---- Настройки -----------------------------------------------------------------
  void setGain(Gain g) { _gain = g; _auto = false; }   // фиксированный диапазон (отключает автоподбор)
  void setAutoGain(bool on) { _auto = on; }            // подбирать диапазон под сигнал (самый точный из подходящих)
  Gain gain() const { return _gain; }
  void setRate(uint16_t sps);                          // скорость, измерений в секунду: ADS1115 8...860, ADS1015 128...3300
  void setChannelScale(uint8_t channel, float factor); // например, делитель напряжения 1:3 даёт factor = 3
  float fullScale() const;                             // полная шкала текущего диапазона, В
  float lsb() const;                                   // вольт на единицу «сырого» значения

  // ---- Состояние и диагностика --------------------------------------------------------
  uint8_t address() const { return _addr; }
  bool isConnected();
  Error error() const { return _err; }
  void setLanguage(Language lang) { _lang = lang; }
  const __FlashStringHelper *errorText();
  void diagnose(Print &out);

 private:
  TwoWire *_wire = &Wire;
  uint8_t _addr = 0;
  Chip _chip;
  Gain _gain = RANGE_2V;
  bool _auto = false, _started = false;
  uint8_t _rate = 4;                         // код скорости (ADS1115: 128 в секунду, ADS1015: 1600)
  float _scale[4] = {1, 1, 1, 1};
  Error _err = ERR_OK;
  Language _lang = LANG_RU;

  bool probe(uint8_t address);
  bool convert(uint8_t mux, Gain g, int16_t &raw);
  int16_t readMux(uint8_t mux, float &fs, float &volts);
  bool readReg(uint8_t reg, uint16_t &v);
  bool writeReg(uint8_t reg, uint16_t v);
  uint16_t conversionMs() const;
  static float fullScaleOf(Gain g);
};

#endif  // ALASH_ADS1115_H
