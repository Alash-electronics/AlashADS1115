# AlashADS1115: руководство / нұсқаулық / guide

[Русский](#русский) | [Қазақша](#қазақша) | [English](#english)

---

## Русский

Простая библиотека для 16-битного АЦП **ADS1115** (и 12-битного ADS1015) по I2C: **четыре входа в вольтах**, дифференциальные измерения, автоподбор диапазона. Точнее встроенного `analogRead()` в 64 раза (16 бит против 10) и на ESP32 не зависит от Wi-Fi. Только встроенная `Wire`. Сделана для уроков и наборов Alash Electronics.

### Установка
Arduino IDE: Скетч, Подключить библиотеку, Управлять библиотеками, найти **AlashADS1115** и нажать Установить. Или скачайте ZIP с GitHub: Скетч, Подключить библиотеку, Добавить .ZIP библиотеку.

### Платы
Arduino Uno / Nano / Mega (AVR), ESP32, ESP8266 и другие, где работает Arduino-ядро.

### Подключение
| Модуль ADS1115 | Uno / Nano | ESP32 |
|---|---|---|
| VDD | 5 В (или 3.3 В) | 3.3 В |
| GND | GND | GND |
| SDA | A4 | GPIO21 |
| SCL | A5 | GPIO22 |
| ADDR | GND (0x48), VDD (0x49), SDA (0x4A), SCL (0x4B) | то же |
| A0..A3 | измеряемые сигналы | то же |

Напряжение на входе не должно быть больше питания АЦП + 0.3 В, даже в диапазоне ±6.144 В. Для более высоких напряжений нужен резисторный делитель и `setChannelScale()`.

### Быстрый старт
```cpp
#include <AlashADS1115.h>
AlashADS1115 adc;

void setup() { Serial.begin(115200); adc.begin(); adc.setAutoGain(true); }
void loop() {
  Serial.println(adc.voltage(0), 4);   // В на входе A0 / volts on A0
  delay(500);
}
```

### Что умеет
- **Вольты сразу:** `voltage(0..3)`. Не нужно считать `raw * 0.0000625`.
- **Автодиапазон:** `setAutoGain(true)` сам выбирает самый точный из шести диапазонов (±0.256 ... ±6.144 В), поэтому и милливольты, и вольты измеряются с хорошей точностью.
- **Дифференциальные измерения:** `voltageDiff(0, 1)` (пары 0-1, 0-3, 1-3, 2-3) со знаком, для мостов, шунтов и тензодатчиков.
- **Делитель напряжения:** `setChannelScale(канал, 3.0)` сразу пересчитывает показание.
- **Скорость:** `setRate(8 ... 860)` для ADS1115, `128 ... 3300` для ADS1015; библиотека сама ждёт окончания преобразования.
- **Честные ошибки:** упор в предел диапазона это `ERR_CLIPPED`, зависание это `ERR_TIMEOUT`, а не неверное число.
- **Находит АЦП** на 0x48-0x4B и проверяет, что это не INA219 на том же адресе.
- **Понятные подсказки** на трёх языках: `errorText()`, `diagnose(Serial)`.
- Работает на **Arduino AVR, ESP32, ESP8266**.

### Справочник по методам
| Метод | Что делает |
|---|---|
| `begin()` | Найти АЦП на 0x48-0x4B и проверить |
| `voltage(ch)` | Вольты на входе 0..3 |
| `voltageDiff(p, n)` | Разность двух входов, В (со знаком) |
| `readRaw(ch) / readRawDiff(p, n)` | Сырое значение со знаком |
| `setAutoGain(on) / setGain(g)` | Автодиапазон / фиксированный диапазон |
| `setRate(sps)` | Скорость измерений |
| `setChannelScale(ch, k)` | Делитель напряжения |
| `fullScale() / lsb()` | Полная шкала, В / вольт на единицу |
| `diagnose(Serial)` | Самопроверка |

### Если что-то не работает
**Значения прыгают или не меняются**

Вход «висит в воздухе». Подключите его к источнику сигнала или к GND.

**ERR_CLIPPED**

Напряжение выше выбранного диапазона. Включите `setAutoGain(true)` или выберите диапазон шире. Выше напряжения питания АЦП подавать нельзя.

**АЦП не найден**

Проверьте SDA/SCL, питание и ADDR. Адрес 0x48 при ADDR на GND. Запустите пример Diagnose.

### Самопроверка
В библиотеке есть `diagnose(Serial)`: она печатает, что видит плата, и подсказывает, что проверить. Язык подсказок: `setLanguage(LANG_RU / LANG_KK / LANG_EN)`.

### Примеры
`Basic`, `Differential`, `AutoGain`, `Diagnose`, `ESP32_CustomPins`

---

## Қазақша

I2C арқылы 16-биттік **ADS1115** (және 12-биттік ADS1015) АЦП-ға арналған қарапайым кітапхана: **вольтпен төрт кіріс**, дифференциал өлшеу, ауқымды авто таңдау. Кірістірілген `analogRead()`-тен 64 есе дәлірек. Тек кірістірілген `Wire`.

### Орнату
Arduino IDE: Sketch, Include Library, Manage Libraries, **AlashADS1115** табып, Install басыңыз. Немесе GitHub-тан ZIP жүктеп, Sketch, Include Library, Add .ZIP Library.

### Тақталар
Arduino Uno / Nano / Mega (AVR), ESP32, ESP8266 және Arduino ядросы бар басқалар.

### Қосу
| ADS1115 модулі | Uno / Nano | ESP32 |
|---|---|---|
| VDD | 5 В (или 3.3 В) | 3.3 В |
| GND | GND | GND |
| SDA | A4 | GPIO21 |
| SCL | A5 | GPIO22 |
| ADDR | GND (0x48), VDD (0x49), SDA (0x4A), SCL (0x4B) | то же |
| A0..A3 | измеряемые сигналы | то же |

Кіріс кернеуі АЦП қуатынан + 0.3 В-тан аспауы керек. Жоғары кернеулер үшін резисторлы бөлгіш қажет.

### Жылдам бастау
```cpp
#include <AlashADS1115.h>
AlashADS1115 adc;

void setup() { Serial.begin(115200); adc.begin(); adc.setAutoGain(true); }
void loop() {
  Serial.println(adc.voltage(0), 4);   // В на входе A0 / volts on A0
  delay(500);
}
```

### Мүмкіндіктері
- **Бірден вольт:** `voltage(0..3)`.
- **Авто-ауқым:** `setAutoGain(true)` алты ауқымның ең дәлін таңдайды.
- **Дифференциал өлшеу:** `voltageDiff(0, 1)` таңбамен.
- **Кернеу бөлгіш:** `setChannelScale(арна, 3.0)`.
- **Жылдамдық:** `setRate(...)`; кітапхана өзі күтеді.
- **Адал қателер:** `ERR_CLIPPED`, `ERR_TIMEOUT`.
- **АЦП-ды табады** 0x48-0x4B мекенжайында.
- **Түсінікті кеңестер** үш тілде.
- **Arduino AVR, ESP32, ESP8266**.

### Әдістер анықтамасы
| Әдіс | Не істейді |
|---|---|
| `begin()` | АЦП-ды табу |
| `voltage(ch)` | Кіріс кернеуі |
| `voltageDiff(p, n)` | Екі кіріс айырмасы |
| `readRaw(ch) / readRawDiff(p, n)` | Шикі мән |
| `setAutoGain(on) / setGain(g)` | Авто / тұрақты ауқым |
| `setRate(sps)` | Өлшеу жылдамдығы |
| `setChannelScale(ch, k)` | Кернеу бөлгіш |
| `fullScale() / lsb()` | Толық шкала / бірлікке вольт |
| `diagnose(Serial)` | Өзін тексеру |

### Бірдеңе жұмыс істемесе
**Мәндер секіреді**

Кіріс ауада ілулі. Оны көзге не GND-ға қосыңыз.

**ERR_CLIPPED**

Кернеу таңдалған ауқымнан жоғары. `setAutoGain(true)` қосыңыз.

**АЦП табылмады**

SDA/SCL, қуат және ADDR тексеріңіз. Diagnose мысалын іске қосыңыз.

### Өзін-өзі тексеру
Кітапханада `diagnose(Serial)` бар: тақта не көргенін басып шығарады және нені тексеру керегін айтады. Кеңес тілі: `setLanguage(LANG_RU / LANG_KK / LANG_EN)`.

### Мысалдар
`Basic`, `Differential`, `AutoGain`, `Diagnose`, `ESP32_CustomPins`

---

## English

A simple library for the 16-bit **ADS1115** (and 12-bit ADS1015) ADC over I2C: **four inputs in volts**, differential reads, automatic range. 64 times finer than the built-in `analogRead()` (16 bits vs 10) and on ESP32 it does not depend on Wi-Fi. Only the built-in `Wire`. Made for classes and Alash Electronics kits.

### Installation
Arduino IDE: Sketch, Include Library, Manage Libraries, search **AlashADS1115** and click Install. Or download the ZIP from GitHub: Sketch, Include Library, Add .ZIP Library.

### Boards
Arduino Uno / Nano / Mega (AVR), ESP32, ESP8266 and others with an Arduino core.

### Wiring
| ADS1115 module | Uno / Nano | ESP32 |
|---|---|---|
| VDD | 5 В (или 3.3 В) | 3.3 В |
| GND | GND | GND |
| SDA | A4 | GPIO21 |
| SCL | A5 | GPIO22 |
| ADDR | GND (0x48), VDD (0x49), SDA (0x4A), SCL (0x4B) | то же |
| A0..A3 | измеряемые сигналы | то же |

The input voltage must not exceed the ADC supply + 0.3 V, even in the ±6.144 V range. For higher voltages use a resistor divider and `setChannelScale()`.

### Quick start
```cpp
#include <AlashADS1115.h>
AlashADS1115 adc;

void setup() { Serial.begin(115200); adc.begin(); adc.setAutoGain(true); }
void loop() {
  Serial.println(adc.voltage(0), 4);   // В на входе A0 / volts on A0
  delay(500);
}
```

### Features
- **Volts right away:** `voltage(0..3)`. No need to compute `raw * 0.0000625`.
- **Auto range:** `setAutoGain(true)` picks the most accurate of six ranges (±0.256 ... ±6.144 V), so millivolts and volts are both measured well.
- **Differential reads:** `voltageDiff(0, 1)` (pairs 0-1, 0-3, 1-3, 2-3), signed, for bridges, shunts and load cells.
- **Voltage divider:** `setChannelScale(channel, 3.0)` rescales the reading.
- **Speed:** `setRate(8 ... 860)` for the ADS1115, `128 ... 3300` for the ADS1015; the library waits for the conversion itself.
- **Honest errors:** hitting the range limit is `ERR_CLIPPED`, a hang is `ERR_TIMEOUT`, not a wrong number.
- **Finds the ADC** at 0x48-0x4B and checks that it is not an INA219 on the same address.
- **Clear tips** in three languages: `errorText()`, `diagnose(Serial)`.
- Works on **Arduino AVR, ESP32, ESP8266**.

### Method reference
| Method | What it does |
|---|---|
| `begin()` | Find the ADC at 0x48-0x4B and verify |
| `voltage(ch)` | Volts on input 0..3 |
| `voltageDiff(p, n)` | Difference of two inputs, V (signed) |
| `readRaw(ch) / readRawDiff(p, n)` | Raw signed value |
| `setAutoGain(on) / setGain(g)` | Auto range / fixed range |
| `setRate(sps)` | Measurement rate |
| `setChannelScale(ch, k)` | Voltage divider factor |
| `fullScale() / lsb()` | Full scale, V / volts per count |
| `diagnose(Serial)` | Self-check |

### If something does not work
**Values jump or do not change**

The input is floating. Connect it to a source or GND.

**ERR_CLIPPED**

The voltage is above the selected range. Use `setAutoGain(true)` or a wider range. Never exceed the ADC supply.

**The ADC is not found**

Check SDA/SCL, power and ADDR. The address is 0x48 with ADDR on GND. Run the Diagnose example.

### Self-check
The library has `diagnose(Serial)`: it prints what the board sees and tells what to check. Tips language: `setLanguage(LANG_RU / LANG_KK / LANG_EN)`.

### Examples
`Basic`, `Differential`, `AutoGain`, `Diagnose`, `ESP32_CustomPins`

---

MIT License, Alash Electronics. https://github.com/Alash-electronics/AlashADS1115
