#pragma once

#include <cassert>
#include <functional>
#include <stdint.h>

namespace thermocam {

// enums mirror Arduino's values -> test with Arduino.h to ensure compatibility

enum class PinState
{
    LOW_LEVEL = 0,
    HIGH_LEVEL = 1
};

enum class EdgeType
{
    RISING_EDGE = 0x1,
    FALLING_EDGE = 0x2,
    ANY_EDGE = 0x3,
};

enum class PinMode
{
    IN_FLOATING = 0x01,
    IN_PULLUP = 0x05,
    IN_PULLDOWN = 0x09,
    OUT = 0x03,
    OUT_OPEN_DRAIN = 0x13
};

static void noop_pin_mode_func(uint8_t, PinMode) {}

template <typename PinType, PinType Min, PinType Max,
          typename ReadoutFunction, typename TimestampFunction, typename PinModeFunction = decltype(noop_pin_mode_func)>
class DebouncedPin
{
public:
    // using ReadoutFunction = std::function<PinState(PinType)>;
    // using PinModeFunction = std::function<void(PinType, PinMode)>;
    // using TimestampFunction = std::function<unsigned long()>;

    DebouncedPin() = delete;
    DebouncedPin(PinType pin,
                 PinMode pin_mode,
                 ReadoutFunction readout_func,
                 TimestampFunction timestamp_func,
                 PinModeFunction pin_setup_func = noop_pin_mode_func,
                 long debounce_delay_ms = 40)
        : _pin(pin),
          _debounce_delay_ms(debounce_delay_ms),
          _readout_func(readout_func),
          _pin_setup_func(pin_setup_func),
          _timestamp_func(timestamp_func)
    {
        assert(_pin >= Min && _pin <= Max);
        _pin_setup_func(_pin, pin_mode);
    }

    // get state of pin by reading it out
    PinState current_state()
    {
        auto current_state = _readout();
        _last_read_state = current_state;
        return current_state;
    }

    bool was_edge_detected(EdgeType edge_type)
    {
        auto current_state = _readout();
        bool is_edge = false;
        switch (edge_type) {
        case EdgeType::RISING_EDGE:
            is_edge = current_state == PinState::HIGH_LEVEL && _last_read_state == PinState::LOW_LEVEL;
            break;
        case EdgeType::FALLING_EDGE:
            is_edge = current_state == PinState::LOW_LEVEL && _last_read_state == PinState::HIGH_LEVEL;
            break;
        case EdgeType::ANY_EDGE:
            is_edge = current_state != _last_read_state;
            break;
        default:
            break;
        }
        _last_read_state = current_state;
        return is_edge;
    }

    // Get last read pin state. Does not readout pin state!
    PinState last_state() const noexcept
    {
        return _last_read_state;
    }

    void set_last_state(PinState state) noexcept
    {
        _last_read_state = state;
    }

    void toggle_last_state() noexcept
    {
        _last_read_state = _last_read_state == PinState::LOW_LEVEL ? PinState::HIGH_LEVEL : PinState::LOW_LEVEL;
    }

    void change_pin_mode(PinMode pin_mode)
    {
        _pin_setup_func(_pin, pin_mode);
    }

private:
    PinState _readout()
    {
        auto current_time = _timestamp_func();
        if (current_time - _last_update_time > _debounce_delay_ms) {
            _last_update_time = current_time;
            _read_state_buffer = _readout_func(_pin);
        }
        return _read_state_buffer;
    }

    PinType _pin;
    unsigned long _debounce_delay_ms = 40;
    unsigned long _last_update_time = 0;
    PinState _read_state_buffer = PinState::LOW_LEVEL;
    PinState _last_read_state = PinState::LOW_LEVEL;
    PinState(*_readout_func)(PinType);
    void(*_pin_setup_func)(PinType, PinMode);
    unsigned long(*_timestamp_func)();
};

} // namespace thermocam
