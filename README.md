# ButtonPoll — Deterministic Input Engine for Embedded Systems

> Lightweight. Predictable. No nonsense.
> A next-generation button & selector handler for Arduino and ESP platforms.

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Platform: Arduino](https://img.shields.io/badge/Platform-Arduino-00878F?logo=arduino&logoColor=white)](https://arduino.cc)
[![Platform: ESP32](https://img.shields.io/badge/Platform-ESP32-blue?logo=espressif&logoColor=white)](https://espressif.com)
[![Platform: ESP8266](https://img.shields.io/badge/Platform-ESP8266-lightgrey?logo=espressif&logoColor=white)](https://espressif.com)
[![Language: C++11](https://img.shields.io/badge/C%2B%2B-11-blue.svg)](https://isocpp.org/)
[![Architecture: 32-bit](https://img.shields.io/badge/Architecture-32--bit-orange.svg)]()
[![Max Buttons: 32](https://img.shields.io/badge/Max%20Buttons-32-informational.svg)]()
[![Memory: Heap Free](https://img.shields.io/badge/Memory-Heap--Free-success.svg)]()
[![Design: Deterministic](https://img.shields.io/badge/Design-Deterministic-critical.svg)]()

---

## Why ButtonPoll?

Most button libraries are either:

* too simple (no features), or
* too complex (bloated, unpredictable)

**ButtonPoll sits right in the middle — engineered, not overengineered.**

### Core Advantages

* **Deterministic Memory**
  Uses constant RAM regardless of button count (up to 32).

* **Non-Blocking by Design**
  Fully polling-based. No delays. No interrupts. No surprises.

* **Hybrid Input System**
  Supports:

  * Momentary buttons
  * Latched selectors (toggle switches)
  * Multi-button combinations

* **Production-Grade Logic**

  * Per-button debounce (target-based)
  * Auto-repeat (hold-to-scroll)
  * Per-pin inversion (mixed wiring)

* **Self-Healing State**

  * `refresh()` syncs selector state after reset

---

## How It Works

```
Physical Input
      ↓
   Debounce Engine
      ↓
  Stable State (Bitmask)
      ↓
   Event Dispatch
```

✔ Single read per cycle
✔ Fully deterministic behavior
✔ No raw-state leakage

---

## Event Contract (The Protocol)

All interactions are encoded into compact numeric events:

| Range       | Event Type   | Description               |
| ----------- | ------------ | ------------------------- |
| `1 – 32`    | Press        | Button index `N` pressed  |
| `101 – 132` | Release      | Button index `N` released |
| `200`       | Neutral      | All selectors OFF         |
| `201 – 232` | Selector ON  | Selector `N` switched ON  |
| `301 – 332` | Selector OFF | Selector `N` switched OFF |

### Formula

```
Press        = idx
Release      = idx + 100
Selector ON  = idx + 200
Selector OFF = idx + 300
```

---

## ⚙️ Features

### Input Modes

* `setAsSelector(index)` → Toggle / switch behavior
* `setAsRepeat(index)` → Auto-repeat on hold
* `setInvert(index)` → Per-pin active HIGH/LOW

---

### Query System

* `isSelectorActive(index)`
* `getActiveSelectors()`
* `isMultiPressed(mask)`
* `isLongPressed(index, threshold)`

---

### Smart Behavior

* Independent debounce per button
* Target-based debounce (capture → hold → commit)
* Stable multi-button detection
* Clean separation: input → state → event

---

## Quick Start

```cpp
#include <ButtonPoll.h>

const int pins[] = {2, 3, 4}; // Enter, Up, Toggle
ButtonPoll btn(pins, 3, LOW);

void handle(uint16_t id) {
    if (id == 1) Serial.println("ENTER pressed");
    if (id == 103) Serial.println("UP released");
    if (id == 204) Serial.println("TOGGLE ON");
}

void setup() {
    Serial.begin(115200);

    btn.setCallback(handle);

    btn.setAsRepeat(2);     // Button 2: hold to repeat
    btn.setAsSelector(3);   // Button 3: toggle switch
    btn.setInvert(3);       // Active HIGH wiring

    btn.begin();
    btn.refresh();          // Sync selector state
}

void loop() {
    btn.update();

    // Long press (2 seconds)
    if (btn.isLongPressed(1, 2000)) {
        Serial.println("Long press detected");
    }

    // Multi-press (Btn1 + Btn2)
    if (btn.isMultiPressed(0b011)) {
        Serial.println("Combo detected");
    }
}
```

---

## Performance

* **Heap-free (no dynamic allocation)**
* Fixed memory footprint
* Configurable polling interval
* Optimized for:

  * Arduino (AVR)
  * ESP8266 / ESP32

---

## Limitations

* Maximum: **32 buttons**
* Polling-based only (no interrupt mode)
* Event system uses numeric encoding (by design)

---

## Design Philosophy

> “Build systems that are predictable, not magical.”

ButtonPoll is designed to:

* be simple to reason about
* behave consistently under all conditions
* scale without hidden costs

---

## Installation

### Arduino IDE

1. Download ZIP
2. `Sketch → Include Library → Add .ZIP Library`

### PlatformIO

```ini
lib_deps =
    ulywae/ButtonPoll
```

---

## License

MIT License — free for personal and commercial use.

---

## Author

**Ulywae** (@neufa)
Part of the **NEU Ecosystem**

> Handcrafted with pure logic.

---

## Notes

This is primarily a personal toolkit.

If it helps your project — great.
If not — that’s fine too.

It does exactly what it was built to do.

---

# If You Like It

Give it a star, or just use it silently in your project — both are valid 😄
