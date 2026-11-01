// Тесты AlashADS1115 без железа (имитация АЦП: входы в вольтах, диапазон, скорость, время преобразования):
//   g++ -std=c++17 -Wall -I mock -I ../../src test_ads1115.cpp ../../src/AlashADS1115.cpp && ./a.out
#include <stdio.h>
#include "Arduino.h"
#include "Wire.h"
#include "AlashADS1115.h"

static int fails = 0;
#define CHECK(cond) do { if (!(cond)) { printf("FAIL line %d: %s\n", __LINE__, #cond); fails++; } } while (0)
static bool near(float a, float b, float e) { return fabsf(a - b) <= e; }
static void reset() { g_ads = FakeAds(); g_ms += 1000; }
static int pga() { return (g_ads.lastCfg >> 9) & 7; }

int main() {
  // 1) запуск и значения на четырёх входах (диапазон ±2.048 В по умолчанию)
  reset(); g_ads.inV[0] = 1.234f; g_ads.inV[1] = 0.5f; g_ads.inV[2] = 2.0f; g_ads.inV[3] = 0.0f;
  AlashADS1115 a;
  CHECK(a.begin() && a.address() == 0x48);
  CHECK(near(a.voltage(0), 1.234f, 0.0001f) && near(a.voltage(1), 0.5f, 0.0001f) && near(a.voltage(2), 2.0f, 0.0001f) && near(a.voltage(3), 0.0f, 0.0001f));
  CHECK(a.error() == AlashADS1115::ERR_OK);
  CHECK(near(a.lsb(), 2.048f / 32768.0f, 1e-9f) && near(a.fullScale(), 2.048f, 1e-6f));
  CHECK(a.readRaw(0) == (int16_t)lroundf(1.234f / 2.048f * 32768.0f));

  // 2) упор в диапазон: 3.3 В при ±2.048 В это ERR_CLIPPED; шире диапазон решает
  g_ads.inV[0] = 3.3f;
  float v = a.voltage(0);
  CHECK(near(v, 2.048f, 0.001f) && a.error() == AlashADS1115::ERR_CLIPPED);
  a.setGain(AlashADS1115::RANGE_4V);
  CHECK(near(a.voltage(0), 3.3f, 0.0002f) && a.error() == AlashADS1115::ERR_OK && pga() == 1);

  // 3) отрицательное и дифференциальные измерения
  g_ads.inV[0] = 1.5f; g_ads.inV[1] = 0.75f; g_ads.inV[2] = 2.0f; g_ads.inV[3] = 0.5f;
  CHECK(near(a.voltageDiff(0, 1), 0.75f, 0.0002f));
  CHECK(near(a.voltageDiff(1, 3), 0.25f, 0.0002f));
  CHECK(near(a.voltageDiff(2, 3), 1.5f, 0.0002f));
  CHECK(near(a.voltageDiff(0, 3), 1.0f, 0.0002f));
  g_ads.inV[0] = 0.2f; g_ads.inV[1] = 0.9f;
  CHECK(near(a.voltageDiff(0, 1), -0.7f, 0.0002f));                   // минус при обратной полярности
  CHECK(a.readRawDiff(0, 1) < 0);
  CHECK(isnan(a.voltageDiff(1, 2)) && a.error() == AlashADS1115::ERR_BAD_CHANNEL);   // такой пары в АЦП нет
  CHECK(isnan(a.voltage(4)) && a.error() == AlashADS1115::ERR_BAD_CHANNEL);

  // 4) автоподбор диапазона: самый узкий, в который влезает сигнал
  reset(); AlashADS1115 g; g.begin(); g.setAutoGain(true);
  g_ads.inV[0] = 0.3f;   CHECK(near(g.voltage(0), 0.3f, 0.0001f) && pga() == 4);       // ±0.512
  g_ads.inV[0] = 0.05f;  CHECK(near(g.voltage(0), 0.05f, 0.00002f) && pga() == 5);     // ±0.256
  g_ads.inV[0] = 1.0f;   CHECK(near(g.voltage(0), 1.0f, 0.0001f) && pga() == 2);       // ±2.048: в ±1.024 с запасом 5 % не влезает
  g_ads.inV[0] = 2.5f;   CHECK(near(g.voltage(0), 2.5f, 0.0002f) && pga() == 1);       // ±4.096
  g_ads.inV[0] = 5.0f;   CHECK(near(g.voltage(0), 5.0f, 0.0003f) && pga() == 0);       // ±6.144
  g_ads.inV[0] = -0.3f;  CHECK(near(g.voltage(0), -0.3f, 0.0001f));                    // отрицательное тоже
  CHECK(g.error() == AlashADS1115::ERR_OK);

  // 5) скорость и время ожидания
  reset(); AlashADS1115 r; r.begin(); g_ads.inV[0] = 1.0f;
  r.setRate(860); r.voltage(0); CHECK(((g_ads.lastCfg >> 5) & 7) == 7);
  r.setRate(100); r.voltage(0); CHECK(((g_ads.lastCfg >> 5) & 7) == 3);    // ближайшая не выше: 64
  r.setRate(8);   uint32_t t0 = g_ms; r.voltage(0);
  CHECK(((g_ads.lastCfg >> 5) & 7) == 0 && g_ms - t0 >= 120);                // 8 измерений в секунду это 125 мс
  r.setRate(860); t0 = g_ms; r.voltage(0); CHECK(g_ms - t0 <= 5);

  // 6) ADS1015: 12 бит, шаг 1 мВ на ±2.048 В, свои скорости
  reset(); g_ads.is1015 = true; g_ads.inV[0] = 1.2345f; AlashADS1115 b(AlashADS1115::ADS1015);
  CHECK(b.begin());
  CHECK(near(b.voltage(0), 1.2345f, 0.0011f) && near(b.lsb(), 0.001f, 1e-6f));
  b.setRate(3300); b.voltage(0); CHECK(((g_ads.lastCfg >> 5) & 7) == 6);
  b.setRate(1600); b.voltage(0); CHECK(((g_ads.lastCfg >> 5) & 7) == 4);

  // 7) делитель напряжения: масштаб канала
  reset(); g_ads.inV[2] = 1.0f; AlashADS1115 s; s.begin(); s.setChannelScale(2, 3.0f);
  CHECK(near(s.voltage(2), 3.0f, 0.001f));
  CHECK(near(s.voltage(1), 0.0f, 0.001f));

  // 8) ошибки: нет АЦП, чужое устройство, зависшее преобразование, адрес 0x49, пропал на ходу
  reset(); g_ads.present = false; AlashADS1115 n;
  CHECK(!n.begin() && n.error() == AlashADS1115::ERR_NO_DEVICE && isnan(n.voltage(0)));
  reset(); g_ads.isAds = false; AlashADS1115 w;
  CHECK(!w.begin() && w.error() == AlashADS1115::ERR_WRONG_CHIP);
  reset(); AlashADS1115 t; t.begin(); g_ads.stuck = true;
  CHECK(isnan(t.voltage(0)) && t.error() == AlashADS1115::ERR_TIMEOUT);
  reset(); g_ads.addr = 0x49; AlashADS1115 d; CHECK(d.begin() && d.address() == 0x49);
  reset(); AlashADS1115 p; p.begin(); g_ads.present = false;
  CHECK(isnan(p.voltage(0)) && p.error() == AlashADS1115::ERR_I2C);

  // 9) тексты и diagnose
  reset(); g_ads.inV[0] = 1.0f; AlashADS1115 x; x.begin(); x.setLanguage(AlashADS1115::LANG_KK); const char *kk = (const char *)x.errorText();
  x.setLanguage(AlashADS1115::LANG_EN); x.diagnose(*(new Print));
  CHECK(kk != nullptr);

  if (fails) { printf("FAILED: %d\n", fails); return 1; }
  printf("ALL TESTS PASSED\n");
  return 0;
}
