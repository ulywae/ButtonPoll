#include "ButtonPoll.h"

ButtonPoll::ButtonPoll(const int *pins, int count,
                       int activeLevel,
                       unsigned long debounceMs,
                       unsigned long repeatMs,
                       unsigned long pollInterval)
    : _pins(pins), _count(count), _activeLevel(activeLevel),
      _debounceMs(debounceMs), _repeatMs(repeatMs),
      _pollInterval(pollInterval),
      _btnLast(0), _btnPressed(0),
      _btnLastChange(0), _btnLastRepeat(0),
      _lastPoll(0), _callback(nullptr),
      _selectorMask(0), _repeatMask(0), _lastSelectorState(0) {}

void ButtonPoll::begin()
{
    for (int i = 0; i < _count; i++)
    {
        pinMode(_pins[i], _activeLevel == LOW ? INPUT_PULLUP : INPUT);
    }
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

bool ButtonPoll::isSelector(uint8_t index)
{
    return (index > 0 && (_selectorMask & (1UL << (index - 1))));
}

bool ButtonPoll::canRepeat(uint8_t index)
{
    return (index > 0 && (_repeatMask & (1UL << (index - 1))));
}

bool ButtonPoll::isSelectorActive(uint8_t index)
{
    if (index == 0 || index > _count)
        return false;
    return (_lastSelectorState & (1UL << (index - 1)));
}

uint32_t ButtonPoll::getActiveSelectors() { return _lastSelectorState; }

void ButtonPoll::refresh()
{
    uint32_t currentPhys = 0;
    for (int i = 0; i < _count; i++)
    {
        if (digitalRead(_pins[i]) == _activeLevel && isSelector(i + 1))
        {
            currentPhys |= (1UL << i);
            if (_callback)
                _callback((i + 1) + 200);
        }
    }
    _lastSelectorState = currentPhys;
    _btnLast = 0;
    _btnPressed = 0;
    _lastPoll = millis();
}

uint8_t ButtonPoll::readActiveButton()
{
    uint8_t firstSelector = 0;
    for (int i = 0; i < _count; i++)
    {
        if (digitalRead(_pins[i]) == _activeLevel)
        {
            uint8_t idx = i + 1;
            if (!isSelector(idx))
                return idx; // Push Button Priority
            if (firstSelector == 0)
                firstSelector = idx;
        }
    }

    return firstSelector;
}

void ButtonPoll::update()
{
    if (millis() - _lastPoll < _pollInterval)
        return;
    _lastPoll = millis();

    uint32_t now = millis();

    // --- 1. HANDLING MULTI-SELECTOR (LATCHED) ---
    uint32_t currentSelectorState = 0;
    for (int i = 0; i < _count; i++)
    {
        if (isSelector(i + 1) && digitalRead(_pins[i]) == _activeLevel)
            currentSelectorState |= (1UL << i);
    }

    if (currentSelectorState != _lastSelectorState)
    {
        for (uint8_t i = 0; i < _count; i++)
        {
            bool wasOn = (_lastSelectorState & (1UL << i));
            bool isOn = (currentSelectorState & (1UL << i));

            if (isOn != wasOn)
            {
                if (_callback)
                    // ON: 201-232 | OFF: 301-332
                    _callback(isOn ? (i + 1 + 200) : (i + 1 + 300));
            }
        }

        // Send 200 only if ALL selectors are truly OFF (Global Neutral)
        if (currentSelectorState == 0 && _callback)
            _callback(200);

        _lastSelectorState = currentSelectorState;
    }

    // --- 2. HANDLING PUSH BUTTON (MOMENTARY) ---
    uint8_t curr = readActiveButton();
    uint8_t pushBtn = isSelector(curr) ? 0 : curr;

    if (pushBtn != _btnLast)
    {
        _btnLast = pushBtn;
        _btnLastChange = now;
    }

    if ((now - _btnLastChange) < _debounceMs)
        return;

    if (pushBtn != 0 && !_btnPressed)
    {
        _btnPressed = 1;
        _btnLastRepeat = now;
        if (_callback)
            _callback((uint16_t)pushBtn); // Press: 1-100
        return;
    }

    if (pushBtn != 0 && _btnPressed)
    {
        if (canRepeat(pushBtn) && (now - _btnLastRepeat >= _repeatMs))
        {
            _btnLastRepeat = now;
            if (_callback)
                _callback((uint16_t)pushBtn);
        }
        return;
    }

    if (pushBtn == 0 && _btnPressed)
    {
        if (!canRepeat(_btnLast))
        {
            if (_callback)
                _callback((uint16_t)_btnLast + 100); // Release: 101-200
        }
        _btnPressed = 0;
    }
}
