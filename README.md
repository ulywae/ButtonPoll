---

## ButtonPoll (Input Engine)

ButtonPoll is an input management library (buttons & switches) designed specifically for mission-critical and high-reliability industrial automation systems.

## Why Use This?

* Hybrid System: Handles both momentary buttons and latched/selector switches in a single wiring harness.
* Noise-Resistant: Uses non-blocking polling and debounce logic to combat electromagnetic interference (EMI) in factory environments.
* Event-Driven: Uses callback patterns and data contracts.
* Self-Healing: The refresh() function ensures physical switch synchronization during a cold boot or system reset.

---

## Event Contract (The Holy Protocol)

This library communicates via non-overlapping IDs:

| ID Range  | Event Type   | Description                                    |
| --------- | ------------ | ---------------------------------------------- |
| 1 - N     | Press        | Momentation buttons index 1..N are pressed.    |
| 101 - 10N | Release      | Momentation buttons index 1..N are released.   |
| 200       | Netral       | All selectors are in the OFF / Middle position.|
| 201 - 232 | Selector ON  | Selector switches index 1..32 are active (ON). |
| 301 - 332 | Selector OFF | The Nth switch has just been turned off (Disengaged).|

---

## Other Features

1. setAsRepeat(index): Registers a button to have the Auto-Repeat feature (Hold to scroll). Suitable for UP/DOWN navigation menus.
2. setAsSelector(index): Registers a pin as a Latched switch. The polling logic won't get stuck even if the switch is continuously NC (ON).
3. isSelectorActive(index): Checks the switch status in real-time (Direct Query) without waiting for a callback.
4. getActiveSelectors(): Retrieves all active switch states as a Bitmask (uint32_t).

---

## How to Use (Quick Start)

```cpp
#include <ButtonPoll.h>
const int pins[] = {32, 33, 25, 26, 27}; // Enter, Up, Down, Manual, Auto
ButtonPoll btn(pins, 5, LOW);            // Active LOW

void myHandler(uint16_t id) {
   if (id == 204) Serial.println("MODE: MANUAL ACTIVE");
   if (id == 2) Serial.println("MENU: UP PRESSED");
}

void setup() {
   btn.setCallback(myHandler);
   btn.setAsRepeat(2);   // The UP button can be held
   btn.setAsRepeat(3);   // The DOWN button can be held
   btn.setAsSelector(4); // Pin 27 as the Manual Selector
   btn.setAsSelector(5); // Pin 14 as the Auto Selector

   btn.begin();
   btn.refresh(); // Physically synchronize the switch at startup
}

void loop() {
   btn.update();
}
```

---

## License

## Made by NEUFA

---
