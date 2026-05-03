#ifndef BUTTON_POLL_H
#define BUTTON_POLL_H

#include <Arduino.h>

/**
 * @brief Input Engine Next-Gen (Optimized)
 * Event Contract:
 * 1-N : Press Event
 * 101-10N : Release Event
 * 200 : All Selectors Off
 * 201-232 : Selector ON
 * 301-332 : Selector OFF
 */

typedef void (*BtnCallback)(uint16_t);

class ButtonPoll {
public:
    ButtonPoll(const int *pins, int count, int activeLevel = LOW, unsigned long debounceMs = 50, unsigned long repeatMs = 300, unsigned long pollInterval = 10);
    
    void begin();
    void setCallback(BtnCallback cb);
    void update();

    // --- CONFIGURATION ---
    void setAsSelector(uint8_t index); 
    void setAsRepeat(uint8_t index);
    void setInvert(uint8_t index); // Set pins to Active HIGH
    void refresh();

    // --- STATE QUERY ---
    bool isSelectorActive(uint8_t index);
    uint32_t getActiveSelectors();
    bool isMultiPressed(uint32_t mask); // Check combination (eg: 0b11 for btn 1 & 2)
    bool isLongPressed(uint8_t index, unsigned long threshold); // Check long press

private:
    const int *_pins;
    int _count;
    int _activeLevel;
    uint8_t _btnLast;
    uint8_t _btnPressed;
    uint32_t _btnLastChange;
    uint32_t _btnLastRepeat;
    uint32_t _lastPoll;
    unsigned long _debounceMs;
    unsigned long _repeatMs;
    unsigned long _pollInterval;
    BtnCallback _callback;

    uint32_t _selectorMask;
    uint32_t _repeatMask;
    uint32_t _invertMask;      
    uint32_t _lastSelectorState;

    uint8_t readActiveButton();
    bool isSelector(uint8_t index);
    bool canRepeat(uint8_t index);
    int getTargetLevel(uint8_t index); // Helper internal
};

#endif
