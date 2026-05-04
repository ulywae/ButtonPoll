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

#define BUTTONPOLL_MAX_BUTTONS 32

typedef void (*BtnCallback)(uint16_t);

class ButtonPoll
{
public:
    // Constructor: pins = pin array, count = number of buttons (max 32), activeLevel = HIGH/LOW,
    // debounceMs = anti-bounce time, repeatMs = auto-repeat interval,
    // pollInterval = update call interval (ms)
    ButtonPoll(const int *pins, int count, int activeLevel = LOW,
               unsigned long debounceMs = 50, unsigned long repeatMs = 300,
               unsigned long pollInterval = 10);

    void begin(); // set pinMode
    void setCallback(BtnCallback cb);
    void update(); // call in loop() each time

    // Per button configuration (index 1..count)
    void setAsSelector(uint8_t index); // mode toggle / selector
    void setAsRepeat(uint8_t index);   // auto-repeat mode
    void setInvert(uint8_t index);     // active HIGH (default LOW)

    // Re-synchronize state selector without triggering event
    void refresh();

    // Query state
    bool isSelectorActive(uint8_t index);
    uint32_t getActiveSelectors();
    bool isMultiPressed(uint32_t mask); // mask bits 0..(count-1)
    bool isLongPressed(uint8_t index, unsigned long threshold);

private:
    const int *_pins;
    int _count;
    int _activeLevel;
    unsigned long _debounceMs;
    unsigned long _repeatMs;
    unsigned long _pollInterval;
    BtnCallback _callback;

    // Internal state
    uint32_t _btnLastStable;  // stable state after debounce
    uint32_t _btnPressed;     // button that has been triggered pressed (non-selector)
    uint32_t _btnDebouncing;  // button that is currently in the debounce period
    uint32_t _debounceTarget; // value to be committed after debounce

    unsigned long _lastDebounceTime[BUTTONPOLL_MAX_BUTTONS];
    unsigned long _pressStartTime[BUTTONPOLL_MAX_BUTTONS];
    unsigned long _lastRepeatTime[BUTTONPOLL_MAX_BUTTONS];

    uint32_t _selectorMask;
    uint32_t _repeatMask;
    uint32_t _invertMask;
    uint32_t _lastSelectorState;

    unsigned long _lastPoll; // for interval polling

    // Helper
    int getTargetLevel(uint8_t index);
    uint32_t readCurrentPhysical();
    bool isSelector(uint8_t index);
    bool canRepeat(uint8_t index);
    void handleDebounceAndState(uint32_t raw, uint32_t now);
    void handlePressEvents(uint32_t now);
    void handleReleaseEvents(uint32_t now);
    void handleRepeatEvents(uint32_t now);
    void handleSelectorChanges(uint32_t stableState, uint32_t now);
};

#endif
