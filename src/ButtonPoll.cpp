#include "ButtonPoll.h"

ButtonPoll::ButtonPoll(const int *pins, int count, int activeLevel, unsigned long debounceMs, unsigned long repeatMs, unsigned long pollInterval)
    : _pins(pins), _count(count), _activeLevel(activeLevel), _debounceMs(debounceMs), _repeatMs(repeatMs), _pollInterval(pollInterval),
      _btnLast(0), _btnPressed(0), _btnLastChange(0), _btnLastRepeat(0), _lastPoll(0), _callback(nullptr),
      _selectorMask(0), _repeatMask(0), _invertMask(0), _lastSelectorState(0) {}

void ButtonPoll::begin() {
    for (int i = 0; i < _count; i++) {
        int level = getTargetLevel(i + 1);
        pinMode(_pins[i], level == LOW ? INPUT_PULLUP : INPUT);
    }
}

int ButtonPoll::getTargetLevel(uint8_t index) {
    // If bit in invertMask is active, use the inverse of _activeLevel
    bool inverted = (_invertMask & (1UL << (index - 1)));
    if (inverted) return (_activeLevel == LOW) ? HIGH : LOW;
    return _activeLevel;
}

void ButtonPoll::setCallback(BtnCallback cb) { _callback = cb; }

void ButtonPoll::setAsSelector(uint8_t index) {
    if (index > 0 && index <= _count) _selectorMask |= (1UL << (index - 1));
}

void ButtonPoll::setAsRepeat(uint8_t index) {
    if (index > 0 && index <= _count) _repeatMask |= (1UL << (index - 1));
}

void ButtonPoll::setInvert(uint8_t index) {
    if (index > 0 && index <= _count) _invertMask |= (1UL << (index - 1));
}

bool ButtonPoll::isSelector(uint8_t index) {
    return (index > 0 && (_selectorMask & (1UL << (index - 1))));
}

bool ButtonPoll::canRepeat(uint8_t index) {
    return (index > 0 && (_repeatMask & (1UL << (index - 1))));
}

bool ButtonPoll::isSelectorActive(uint8_t index) {
    if (index == 0 || index > _count) return false;
    return (_lastSelectorState & (1UL << (index - 1)));
}

uint32_t ButtonPoll::getActiveSelectors() { return _lastSelectorState; }

bool ButtonPoll::isMultiPressed(uint32_t mask) {
    uint32_t currentPhys = 0;
    for (int i = 0; i < _count; i++) {
        if (digitalRead(_pins[i]) == getTargetLevel(i + 1)) currentPhys |= (1UL << i);
    }
    return (currentPhys & mask) == mask;
}

bool ButtonPoll::isLongPressed(uint8_t index, unsigned long threshold) {
    if (index == 0 || index > _count) return false;
    if (digitalRead(_pins[index - 1]) == getTargetLevel(index)) {
        if (_btnPressed && _btnLast == index) {
            return (millis() - _btnLastChange >= threshold);
        }
    }
    return false;
}

void ButtonPoll::refresh() {
    uint32_t currentPhys = 0;
    for (int i = 0; i < _count; i++) {
        if (digitalRead(_pins[i]) == getTargetLevel(i + 1) && isSelector(i + 1)) {
            currentPhys |= (1UL << i);
            if (_callback) _callback((i + 1) + 200);
        }
    }
    _lastSelectorState = currentPhys;
    _btnLast = 0; _btnPressed = 0;
    _lastPoll = millis();
}

uint8_t ButtonPoll::readActiveButton() {
    uint8_t firstSelector = 0;
    for (int i = 0; i < _count; i++) {
        if (digitalRead(_pins[i]) == getTargetLevel(i + 1)) {
            uint8_t idx = i + 1;
            if (!isSelector(idx)) return idx;
            if (firstSelector == 0) firstSelector = idx;
        }
    }
    return firstSelector;
}

void ButtonPoll::update() {
    uint32_t now = millis();
    if (now - _lastPoll < _pollInterval) return;
    _lastPoll = now;

    // 1. SELECTORS
    uint32_t currentSelectorState = 0;
    for (int i = 0; i < _count; i++) {
        if (isSelector(i + 1) && digitalRead(_pins[i]) == getTargetLevel(i + 1)) 
            currentSelectorState |= (1UL << i);
    }

    if (currentSelectorState != _lastSelectorState) {
        for (uint8_t i = 0; i < _count; i++) {
            bool wasOn = (_lastSelectorState & (1UL << i));
            bool isOn = (currentSelectorState & (1UL << i));
            if (isOn != wasOn) {
                if (_callback) _callback(isOn ? (i + 1 + 200) : (i + 1 + 300));
            }
        }
        if (currentSelectorState == 0 && _callback) _callback(200);
        _lastSelectorState = currentSelectorState;
    }

    // 2. PUSH BUTTONS
    uint8_t curr = readActiveButton();
    uint8_t pushBtn = isSelector(curr) ? 0 : curr;

    if (pushBtn != _btnLast) {
        _btnLast = pushBtn;
        _btnLastChange = now;
    }

    if ((now - _btnLastChange) < _debounceMs) return;

    if (pushBtn != 0 && !_btnPressed) {
        _btnPressed = 1;
        _btnLastRepeat = now;
        if (_callback) _callback((uint16_t)pushBtn);
        return;
    }

    if (pushBtn != 0 && _btnPressed) {
        if (canRepeat(pushBtn) && (now - _btnLastRepeat >= _repeatMs)) {
            _btnLastRepeat = now;
            if (_callback) _callback((uint16_t)pushBtn);
        }
        return;
    }

    if (pushBtn == 0 && _btnPressed) {
        if (!canRepeat(_btnLast)) {
            if (_callback) _callback((uint16_t)_btnLast + 100);
        }
        _btnPressed = 0;
    }
}
