#ifndef BUTTON_POLL_H
#define BUTTON_POLL_H

#include <Arduino.h>

/**
 * @brief Input Engine
 *
 * Event Contract:
 * 1-N      : Press Event (Momentary)
 * 101-10N  : Release Event (Momentary)
 * 200      : All Selectors Off (Netral)
 * 201-232  : Selector ON (200 + Index)
 * 301-332  : Selector OFF (300 + Index)
 */

typedef void (*BtnCallback)(uint16_t);

class ButtonPoll
{
public:
    ButtonPoll(const int *pins, int count,
               int activeLevel = LOW,
               unsigned long debounceMs = 50,
               unsigned long repeatMs = 300,
               unsigned long pollInterval = 10);

    void begin();
    void setCallback(BtnCallback cb);
    void update();

    // --- UNIVERSAL CONFIGURATION ---
    void setAsSelector(uint8_t index); // Register index (1..N) as Latched/Switch
    void setAsRepeat(uint8_t index);   // Register index (1..N) as Repeatable (Hold)
    void refresh();                    // Force physical synchronization to memory (Cold Boot)

    // --- STATE QUERY ---
    bool isSelectorActive(uint8_t index);
    uint32_t getActiveSelectors();

private:
    const int *_pins;
    int _count;
    int _activeLevel;
    uint16_t _btnLast;
    uint8_t _btnPressed;
    uint32_t _btnLastChange;
    uint32_t _btnLastRepeat;
    uint32_t _lastPoll;
    unsigned long _debounceMs;
    unsigned long _repeatMs;
    unsigned long _pollInterval;
    BtnCallback _callback;

    // Masking & State Management
    uint32_t _selectorMask;      // Bitmask for Selector type pins
    uint32_t _repeatMask;        // Bitmask for Repeat type pins
    uint32_t _lastSelectorState; // Cache the last selector state

    uint8_t readActiveButton();
    bool isSelector(uint8_t index);
    bool canRepeat(uint8_t index);
};

#endif
