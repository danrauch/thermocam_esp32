#pragma once

#include <Arduino.h>

#include "debounced_pin.h"

namespace thermocam {

class ArduinoPin : public DebouncedPin<uint8_t, 0, 50>
{
public:
    ArduinoPin() = delete;
    ArduinoPin(uint8_t pin,
               PinMode pin_mode,
               long debounce_delay_ms = 40)
        : DebouncedPin(pin, pin_mode,
                [](uint8_t pin) { return static_cast<PinState>(digitalRead(pin)); }, 
                millis,
                [](uint8_t pin, PinMode mode) { pinMode(pin, static_cast<uint8_t>(mode)); }, 
                debounce_delay_ms)
    {
    }
};

} // namespace thermocam