#include "ButtonPoll.h"

// List of pins for 3 buttons
const int pinBtn[] = {2, 3, 4};
const int numberBtn = sizeof(pinBtn) / sizeof(pinBtn[0]);

// Initialization: pin, number, active level (LOW), debounce, repeat, poll interval
ButtonPoll bp(pinBtn, numberTombol, LOW, 50, 300, 10);

// Callback function to handle click events
void handleButton(uint16_t eventCode)
{
    if (eventCode <= 100)
    {
        Serial.print("Event: Button ");
        Serial.print(eventCode);
        Serial.println("PRESSED");
    }
    else if (eventCode > 100 && eventCode <= 200)
    {
        Serial.print("Event: Button ");
        Serial.print(eventCode - 100);
        Serial.println(" RELEASED");
    }
}

void setup()
{
    Serial.begin(9600);

    // Feature 1: Invert Logic (For example, the button on pin 3 is Active HIGH)
    bp.setInvert(2); // Index 2 refers to the second pin (pin 3)

    bp.begin();
    bp.setCallback(handleButton);

    Serial.println("ButtonPoll System Ready...");
}

void loop()
{
    bp.update();

    // Feature 2: Long Press (Checks if Button 3 is held for 2 seconds)
    if (bp.isLongPressed(3, 2000))
    {
        static unsigned long lastMsg = 0;
        if (millis() - lastMsg > 500)
        { // To prevent serial overflow
            Serial.println("---> Feature: Button 3 held for 2 seconds! (Long Press)");
            lastMsg = millis();
        }
    }

    // Feature 3: Multi-Press (Checks if Buttons 1 & 2 are pressed simultaneously)
    // Mask: Button 1 (bit 0) and Button 2 (bit 1) -> 0b01 | 0b10 = 0b11 (Decimal: 3)
    if (bp.isMultiPressed(0b011))
    {
        static unsigned long lastCombo = 0;
        if (millis() - lastCombo > 500)
        {
            Serial.println("---> Feature: Key Combination 1 & 2 detected!");
            lastCombo = millis();
        }
    }
}