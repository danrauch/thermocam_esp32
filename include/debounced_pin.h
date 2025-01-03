#pragma once

#include <Arduino.h>

namespace thermocam {

enum class PinState
{
    LOW_LEVEL = LOW,
    HIGH_LEVEL = HIGH
};

enum class PinMode
{
    IN_FLOATING = INPUT,
    IN_PULLUP = INPUT_PULLUP,
    IN_PULLDOWN = INPUT_PULLDOWN,
    OUT = OUTPUT,
    OUT_OPEN_DRAIN = OUTPUT_OPEN_DRAIN
};

class DebouncedPin
{
public:
    DebouncedPin(uint8_t pin, PinMode pin_mode, long debounce_delay_ms = 50)
        : _pin(pin), _debounce_delay_ms(debounce_delay_ms)
    {
        assert(pin >= 0 && pin <= 50);

        pinMode(_pin, static_cast<uint8_t>(pin_mode));
    }

    PinState readout()
    {
        auto current_time = millis();
        if (current_time - _last_read_time > _debounce_delay_ms) {
            _last_read_time = current_time;
            _last_read_state = static_cast<PinState>(digitalRead(_pin));
        }
        return _last_read_state;
    }

private:
    uint8_t _pin;
    unsigned long _debounce_delay_ms = 50;
    unsigned long _last_read_time = 0;
    PinState _last_read_state = PinState::LOW_LEVEL;
};

} // namespace thermocam
