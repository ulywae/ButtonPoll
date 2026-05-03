# ButtonPoll (Input Engine Next-Gen)

A lightweight, non-blocking input handler for buttons and selectors with event-based callbacks. Optimized for extremely low RAM usage using 32-bit masking.

## Why Use This?
* **Deterministic Memory:** Uses the same amount of RAM for 1 or 32 buttons (Single Instance).
* **Hybrid System:** Handles momentary buttons, latched selectors, and multi-press combinations.
* **Pro-Grade Logic:** Built-in Debounce, Auto-Repeat, and Per-pin Inversion (Active LOW/HIGH).
* **Self-Healing:** The `refresh()` function ensures physical switch synchronization during system reset.

---

## Event Contract (The Holy Protocol)
This library communicates via non-overlapping IDs:


| ID Range  | Event Type   | Description                                     |
| --------- | ------------ | ----------------------------------------------- |
| 1 - N     | Press        | Momentary buttons index 1..N are pressed.       |
| 101 - 10N | Release      | Momentary buttons index 1..N are released.      |
| 200       | Neutral      | All selectors are in the OFF / Middle position. |
| 201 - 232 | Selector ON  | Selector switches index 1..32 are active (ON).  |
| 301 - 332 | Selector OFF | The Nth switch has just been turned off.        |

---

## Powerful Features

1. **setAsRepeat(index):** Enable Auto-Repeat (Hold to scroll). Ideal for UP/DOWN menus.
2. **setAsSelector(index):** Handles Latched switches without blocking the polling engine.
3. **setInvert(index):** Support mixed wiring. Set specific pins to **Active HIGH** while others stay **Active LOW**.
4. **isLongPressed(index, ms):** Precision hold detection without extra RAM overhead.
5. **isMultiPressed(mask):** Detect button combinations (e.g., Btn 1 + Btn 2) using bitmasks.

---

## How to Use (Quick Start)

```cpp
#include <ButtonPoll.h>

const int pins[] = {2, 3, 4}; // Enter, Up, SecretToggle
ButtonPoll btn(pins, 3, LOW);

void myHandler(uint16_t id) {
    if (id == 1) Serial.println("ENTER Pressed");
    if (id == 203) Serial.println("SECRET MODE: ON");
}

void setup() {
    btn.setCallback(myHandler);
    btn.setAsRepeat(2);    // Enable hold-to-scroll for button 2
    btn.setAsSelector(3);  // Set button 3 as a toggle switch
    btn.setInvert(3);      // Button 3 is wired as Active HIGH
    
    btn.begin();
    btn.refresh();
}

void loop() {
    btn.update();

    // Custom Logic: Long Press on Button 1 (2 seconds)
    if (btn.isLongPressed(1, 2000)) {
        Serial.println("System Reset initiated...");
    }

    // Custom Logic: Multi-Press (Button 1 & 2 together)
    if (btn.isMultiPressed(0b011)) { 
        Serial.println("Combo Detected!");
    }
}
```

---

## License
Licensed under the **MIT License**. Free for hobby and commercial use.

## Made by NEUFA
Handcrafted with Pure Logic
Code by **Ulywae** (@neufa).
*Part of the NEU Ecosystem — Building professional-grade tools for everyone.*
