#include "ButtonPoll.h"

ButtonPoll::ButtonPoll(const int *pins, int count, int activeLevel,
                       unsigned long debounceMs, unsigned long repeatMs,
                       unsigned long pollInterval)
    : _pins(pins), _activeLevel(activeLevel),
      _debounceMs(debounceMs), _repeatMs(repeatMs), _pollInterval(pollInterval),
      _callback(nullptr),
      _btnLastStable(0), _btnPressed(0), _btnDebouncing(0), _debounceTarget(0),
      _selectorMask(0), _repeatMask(0), _invertMask(0), _lastSelectorState(0),
      _lastPoll(0)
{

    if (count > BUTTONPOLL_MAX_BUTTONS)
        count = BUTTONPOLL_MAX_BUTTONS;
    _count = count;

    for (int i = 0; i < BUTTONPOLL_MAX_BUTTONS; i++)
    {
        _lastDebounceTime[i] = 0;
        _pressStartTime[i] = 0;
        _lastRepeatTime[i] = 0;
    }
}

void ButtonPoll::begin()
{
    for (int i = 0; i < _count; i++)
    {
        int level = getTargetLevel(i + 1);
        pinMode(_pins[i], (level == LOW) ? INPUT_PULLUP : INPUT);
    }
    refresh();
}

int ButtonPoll::getTargetLevel(uint8_t index)
{
    bool inverted = (_invertMask & (1UL << (index - 1))) != 0;
    if (inverted)
        return (_activeLevel == LOW) ? HIGH : LOW;
    return _activeLevel;
}

void ButtonPoll::setCallback(BtnCallback cb) { _callback = cb; }

void ButtonPoll::setAsSelector(uint8_t index)
{
    if (index > 0 && index <= _count)
        _selectorMask |= (1UL << (index - 1));
}

void ButtonPoll::setAsRepeat(uint8_t index)
{
    if (index > 0 && index <= _count)
        _repeatMask |= (1UL << (index - 1));
}

void ButtonPoll::setInvert(uint8_t index)
{
    if (index > 0 && index <= _count)
        _invertMask |= (1UL << (index - 1));
}

bool ButtonPoll::isSelector(uint8_t index)
{
    return (index > 0 && (_selectorMask & (1UL << (index - 1))) != 0);
}

bool ButtonPoll::canRepeat(uint8_t index)
{
    return (index > 0 && (_repeatMask & (1UL << (index - 1))) != 0);
}

uint32_t ButtonPoll::readCurrentPhysical()
{
    uint32_t result = 0;
    for (int i = 0; i < _count; i++)
    {
        if (digitalRead(_pins[i]) == getTargetLevel(i + 1))
        {
            result |= (1UL << i);
        }
    }
    return result;
}

void ButtonPoll::refresh()
{
    uint32_t phys = readCurrentPhysical();
    _lastSelectorState = phys & _selectorMask;
}

void ButtonPoll::update()
{
    uint32_t now = millis();
    if (now - _lastPoll < _pollInterval)
        return;
    _lastPoll = now;

    uint32_t raw = readCurrentPhysical();

    handleDebounceAndState(raw, now);
    handlePressEvents(now);
    handleReleaseEvents(now);
    handleRepeatEvents(now);
    handleSelectorChanges(_btnLastStable, now);
}

void ButtonPoll::handleDebounceAndState(uint32_t raw, uint32_t now)
{
    uint32_t changed = raw ^ _btnLastStable;

    // Catch the target on the first change
    for (int i = 0; i < _count; i++)
    {
        if (changed & (1UL << i))
        {
            // Save target state (raw value when changed)
            if (raw & (1UL << i))
                _debounceTarget |= (1UL << i);
            else
                _debounceTarget &= ~(1UL << i);

            _lastDebounceTime[i] = now;
            _btnDebouncing |= (1UL << i);
        }
    }

    // Commit target after debounce is complete
    for (int i = 0; i < _count; i++)
    {
        if ((_btnDebouncing & (1UL << i)) && (now - _lastDebounceTime[i] >= _debounceMs))
        {
            _btnDebouncing &= ~(1UL << i);
            if (_debounceTarget & (1UL << i))
                _btnLastStable |= (1UL << i);
            else
                _btnLastStable &= ~(1UL << i);
        }
    }
}

void ButtonPoll::handlePressEvents(uint32_t now)
{
    uint32_t justPressed = _btnLastStable & ~_btnPressed;
    for (int i = 0; i < _count; i++)
    {
        if (justPressed & (1UL << i))
        {
            uint8_t idx = i + 1;
            if (!isSelector(idx))
            {
                _btnPressed |= (1UL << i);
                _pressStartTime[i] = now;
                _lastRepeatTime[i] = now;
                if (_callback)
                    _callback((uint16_t)idx);
            }
        }
    }
}

void ButtonPoll::handleReleaseEvents(uint32_t now)
{
    uint32_t justReleased = _btnPressed & ~_btnLastStable;
    for (int i = 0; i < _count; i++)
    {
        if (justReleased & (1UL << i))
        {
            uint8_t idx = i + 1;
            if (!isSelector(idx))
            {
                _btnPressed &= ~(1UL << i);
                _pressStartTime[i] = 0;
                _lastRepeatTime[i] = 0;
                if (_callback)
                    _callback((uint16_t)(idx + 100));
            }
        }
    }
    (void)now;
}

void ButtonPoll::handleRepeatEvents(uint32_t now)
{
    uint32_t pressedNow = _btnPressed & _btnLastStable;
    for (int i = 0; i < _count; i++)
    {
        if (pressedNow & (1UL << i))
        {
            uint8_t idx = i + 1;
            if (canRepeat(idx))
            {
                if (now - _lastRepeatTime[i] >= _repeatMs)
                {
                    _lastRepeatTime[i] = now;
                    if (_callback)
                        _callback((uint16_t)idx);
                }
            }
        }
    }
}

void ButtonPoll::handleSelectorChanges(uint32_t stableState, uint32_t now)
{
    uint32_t currentSelector = stableState & _selectorMask;
    if (currentSelector != _lastSelectorState)
    {
        uint32_t changes = currentSelector ^ _lastSelectorState;
        for (int i = 0; i < _count; i++)
        {
            if (changes & (1UL << i))
            {
                uint8_t idx = i + 1;
                if (_callback)
                {
                    if (currentSelector & (1UL << i))
                        _callback(idx + 200); // selector ON
                    else
                        _callback(idx + 300); // selector OFF
                }
            }
        }
        if (currentSelector == 0 && _callback)
            _callback(200); // all off
        _lastSelectorState = currentSelector;
    }
    (void)now;
}

bool ButtonPoll::isSelectorActive(uint8_t index)
{
    if (index == 0 || index > _count)
        return false;
    return (_lastSelectorState & (1UL << (index - 1))) != 0;
}

uint32_t ButtonPoll::getActiveSelectors()
{
    return _lastSelectorState;
}

bool ButtonPoll::isMultiPressed(uint32_t mask)
{
    return (_btnLastStable & mask) == mask;
}

bool ButtonPoll::isLongPressed(uint8_t index, unsigned long threshold)
{
    if (index == 0 || index > _count)
        return false;
    uint8_t i = index - 1;
    if ((_btnPressed & (1UL << i)) && (_btnLastStable & (1UL << i)))
    {
        return (millis() - _pressStartTime[i]) >= threshold;
    }
    return false;
}
