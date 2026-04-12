#include <ButtonPoll.h>

/**
 * COMPLETE BUTTONPOLL USAGE EXAMPLE
 * By: Ulywae
 *
 * This library uses the "Event Contract" system:
 * 1 - 100: Button pressed (Momentary)
 * 101 - 200: Button released (Momentary)
 * 200: All selectors are in the OFF position (Neutral)
 * 201 - 232: Selector has just been turned ON
 * 301 - 332: Selector has just been turned OFF
 */

// 1. List of pins used
const int PINS[] = {2, 3, 4, 5};
const int TOTAL_PINS = 4;

// 2. Initialization: Pins, Count, Active Level, Debounce, RepeatSpeed, PollInterval
ButtonPoll buttons(PINS, TOTAL_PINS, LOW, 50, 300, 10);

// 3. Callback function to handle all events
void onButtonEvent(uint16_t eventId)
{
    Serial.print(F(">>> Event Detected: "));
    Serial.println(eventId);

    switch (eventId)
    {
    // --- Regular Push Button (Momentary) ---
    case 1:
        Serial.println(F("Button 1: PRESSED"));
        break;
    case 101:
        Serial.println(F("Button 1: RELEASED"));
        break;

    // --- Buttons with Repeat (Auto-Fire) Feature ---
    case 2:
        Serial.println(F("Button 2: IS BEING HELD (Repeat)"));
        break;

    // --- Switch System (Selector) ---
    case 200:
        Serial.println(F("SYSTEM NEUTRAL: All switches are OFF"));
        break;

    case 203:
        Serial.println(F("Switch 3: JUST BEEN TURNED ON"));
        break;
    case 303:
        Serial.println(F("Switch 3: JUST BEEN TURNED OFF"));
        break;

    case 204:
        Serial.println(F("Switch 4: JUST BEEN TURNED ON"));
        break;
    case 304:
        Serial.println(F("Switch 4: JUST BEEN TURNED OFF"));
        break;
    }
}

void setup()
{
    Serial.begin(115200);
    while (!Serial)
        ; // Wait for Serial to be ready (Leonardo/ESP32 only)

    // 4. Start the Driver
    buttons.begin();
    buttons.setCallback(onButtonEvent);

    // 5. Configure the Pin Type (If not set, the default is Push Button)
    buttons.setAsRepeat(2);   // Pin 3 (Index 2) will continuously send events when held
    buttons.setAsSelector(3); // Pin 4 (Index 3) is set as a Switch/Toggle
    buttons.setAsSelector(4); // Pin 5 (Index 4) is set as a Switch/Toggle

    // 6. Physical Synchronization (Very Pro!)
    // Ensures the physical switch status when the device is first turned on is immediately read in the program
    Serial.println(F("Synchronizing physical switch status..."));
    buttons.refresh();

    Serial.println(F("ButtonPoll Ready! Please press the button."));
}

void loop()
{
    // 7. Continuously monitor input without blocking other processes (Non-blocking)
    buttons.update();

    // Here you can do other things (e.g., read sensors or blink LEDs)
    // without interrupting the button reading process.
}