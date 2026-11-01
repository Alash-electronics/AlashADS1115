// Имитация I2C и ADS1115 / ADS1015: регистры, мультиплексор входов, диапазон, время преобразования.
#pragma once
#include "Arduino.h"
struct FakeAds {
  bool present = true; uint8_t addr = 0x48; bool isAds = true; bool is1015 = false;
  uint16_t config = 0x8583;
  float inV[4] = {0, 0, 0, 0};      // напряжения на входах A0..A3, В
  uint32_t doneAt = 0; bool converting = false; bool stuck = false;
  int16_t result = 0; int conversions = 0;
  uint16_t lastCfg = 0;
};
inline FakeAds g_ads;
class TwoWire {
 public:
  void begin() {}
  void begin(int, int) {}
  void beginTransmission(uint8_t a) { _a = a; _n = 0; _hi = 0; }
  size_t write(uint8_t v) {
    if (_n == 0) _reg = v; else if (_n == 1) _hi = v;
    else if (_n == 2 && g_ads.isAds && _reg == 1) {
      uint16_t w = (_hi << 8) | v;
      g_ads.lastCfg = w;
      if (w & 0x8000) start(w);
      g_ads.config = (w & 0x7FFF) | (g_ads.converting ? 0 : 0x8000);
    }
    _n++; return 1;
  }
  uint8_t endTransmission(bool = true) { return (g_ads.present && _a == g_ads.addr) ? 0 : 2; }
  uint8_t requestFrom(uint8_t a, uint8_t len) {
    if (!(g_ads.present && a == g_ads.addr) || len != 2) return 0;
    uint16_t v = 0;
    if (!g_ads.isAds) v = 0x0000;                                   // чужое устройство: регистр настройки не принимает записи
    else if (_reg == 1) {
      if (g_ads.converting && !g_ads.stuck && g_ms >= g_ads.doneAt) g_ads.converting = false;
      v = (g_ads.config & 0x7FFF) | (g_ads.converting ? 0 : 0x8000);
    } else if (_reg == 0) v = (uint16_t)g_ads.result;
    _buf[0] = v >> 8; _buf[1] = v & 0xFF; _pos = 0; _len = 2; return 2;
  }
  int read() { return _pos < _len ? _buf[_pos++] : -1; }
 private:
  uint8_t _a = 0, _reg = 0, _hi = 0, _n = 0, _buf[4], _len = 0, _pos = 0;
  void start(uint16_t w) {
    static const float fs[8] = {6.144f, 4.096f, 2.048f, 1.024f, 0.512f, 0.256f, 0.256f, 0.256f};
    static const int r11[8] = {8, 16, 32, 64, 128, 250, 475, 860}, r15[8] = {128, 250, 490, 920, 1600, 2400, 3300, 3300};
    uint8_t mux = (w >> 12) & 7, pga = (w >> 9) & 7, dr = (w >> 5) & 7;
    float v;
    if (mux >= 4) v = g_ads.inV[mux - 4];
    else { static const int p[4] = {0, 0, 1, 2}, n[4] = {1, 3, 3, 3}; v = g_ads.inV[p[mux]] - g_ads.inV[n[mux]]; }
    float div = g_ads.is1015 ? 2048.0f : 32768.0f;
    float raw = v / fs[pga] * div;
    float hi = div - 1, lo = -div;
    if (raw > hi) raw = hi; if (raw < lo) raw = lo;
    int16_t r = (int16_t)lroundf(raw);
    g_ads.result = g_ads.is1015 ? (int16_t)(r << 4) : r;           // ADS1015 хранит 12 бит в старших разрядах
    int sps = g_ads.is1015 ? r15[dr] : r11[dr];
    g_ads.doneAt = g_ms + 1000 / sps; g_ads.converting = true; g_ads.conversions++;
  }
};
inline TwoWire Wire;
