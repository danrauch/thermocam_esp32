#pragma once

#include <Arduino.h>

#include "debounced_pin.h"

namespace thermocam {

static PinState _arduino_readout_func(uint8_t pin)
{
    return static_cast<PinState>(digitalRead(pin));
}

static void _arduino_pin_mode_func(uint8_t pin, PinMode mode)
{
    pinMode(pin, static_cast<uint8_t>(mode));
}

class ArduinoPin : public DebouncedPin<uint8_t, 0, 50,
                                       decltype(_arduino_readout_func),
                                       decltype(millis),
                                       decltype(_arduino_pin_mode_func)>
{
public:
    ArduinoPin() = delete;
    ArduinoPin(uint8_t pin,
               PinMode pin_mode,
               long debounce_delay_ms = 40)
        : DebouncedPin(pin, pin_mode,
                       _arduino_readout_func,
                       millis,
                       _arduino_pin_mode_func,
                       debounce_delay_ms)
    {
    }
};

} // namespace thermocam