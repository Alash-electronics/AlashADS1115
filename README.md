# AlashADS1115

[Русский](#русский) | [Қазақша](#қазақша) | [English](#english)

---

## Русский

Простая библиотека для 16-битного АЦП **ADS1115** (и 12-битного ADS1015) по I2C: **четыре входа в вольтах**, дифференциальные измерения, автоподбор диапазона. Точнее встроенного `analogRead()` в 64 раза (16 бит против 10) и на ESP32 не зависит от Wi-Fi. Только встроенная `Wire`. Сделана для уроков и наборов Alash Electronics.

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

### Чем отличается от других библиотек
Мы посмотрели Adafruit ADS1X15, ADS1X15 (Rob Tillaart) и ADS1115_WE:
- Adafruit требует ещё BusIO и сама не выбирает диапазон; у нас `setAutoGain(true)`.
- Приходится самому пересчитывать «сырое» значение через `computeVolts` и помнить про усиление; у нас `voltage()`.
- При упоре в предел они молча отдают максимум; у нас `ERR_CLIPPED`.

### Примеры
`Basic`, `Differential`, `AutoGain`, `Diagnose`, `ESP32_CustomPins`

### Тесты
Масштабирование, четыре входа, дифференциальные пары и знак, упор в предел, автоподбор диапазона на уровнях от 0.05 до 5 В, коды скорости и время ожидания, ADS1015 (12 бит), делитель, ошибки (чужая микросхема, зависание) проверены без железа: `extras/test`.

Подробное руководство: [docs/WIKI.md](docs/WIKI.md).

---

## Қазақша

I2C арқылы 16-биттік **ADS1115** (және 12-биттік ADS1015) АЦП-ға арналған қарапайым кітапхана: **вольтпен төрт кіріс**, дифференциал өлшеу, ауқымды авто таңдау. Кірістірілген `analogRead()`-тен 64 есе дәлірек. Тек кірістірілген `Wire`.

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

### Басқа кітапханалардан айырмашылығы
Adafruit ADS1X15, ADS1X15 (Rob Tillaart) және ADS1115_WE қарадық:
- Adafruit-қа BusIO керек, ауқымды өзі таңдамайды; бізде `setAutoGain(true)`.
- Шикі мәнді өзің есептейсің; бізде `voltage()`.
- Шекке тірелгенде үнсіз максимум береді; бізде `ERR_CLIPPED`.

### Мысалдар
`Basic`, `Differential`, `AutoGain`, `Diagnose`, `ESP32_CustomPins`

### Тесттер
Масштабтау, төрт кіріс, дифференциал жұптар, шекке тірелу, авто-ауқым, жылдамдық, ADS1015, бөлгіш, қателер тексерілген: `extras/test`.

Толық нұсқаулық: [docs/WIKI.md](docs/WIKI.md).

---

## English

A simple library for the 16-bit **ADS1115** (and 12-bit ADS1015) ADC over I2C: **four inputs in volts**, differential reads, automatic range. 64 times finer than the built-in `analogRead()` (16 bits vs 10) and on ESP32 it does not depend on Wi-Fi. Only the built-in `Wire`. Made for classes and Alash Electronics kits.

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

### How it differs from other libraries
We looked at Adafruit ADS1X15, ADS1X15 (Rob Tillaart) and ADS1115_WE:
- Adafruit also needs BusIO and does not pick the range; we have `setAutoGain(true)`.
- You have to convert raw values yourself with `computeVolts` and remember the gain; we have `voltage()`.
- At the range limit they silently return the max; we return `ERR_CLIPPED`.

### Examples
`Basic`, `Differential`, `AutoGain`, `Diagnose`, `ESP32_CustomPins`

### Tests
Scaling, four inputs, differential pairs and sign, range limit, auto range from 0.05 to 5 V, rate codes and waiting, ADS1015 (12-bit), divider and errors (foreign chip, hang) are checked without hardware: `extras/test`.

Full guide: [docs/WIKI.md](docs/WIKI.md).

---

MIT License, Alash Electronics.
