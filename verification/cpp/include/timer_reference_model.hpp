#pragma once

#include "simulation_context.hpp"

#include <cstdint>

class TimerReferenceModel {
public:
    TimerReferenceModel() { reset(); }

    void reset();
    void clock(const DutInputs& inputs);

    [[nodiscard]] std::uint32_t read(std::uint32_t address) const;
    [[nodiscard]] bool expected_error(std::uint32_t address) const;
    [[nodiscard]] bool irq() const noexcept { return match_pending_ && irq_enable_; }
    [[nodiscard]] bool timer_enabled() const noexcept { return timer_enable_; }
    [[nodiscard]] bool irq_enabled() const noexcept { return irq_enable_; }
    [[nodiscard]] std::uint32_t counter() const noexcept { return counter_; }
    [[nodiscard]] std::uint32_t compare() const noexcept { return compare_; }
    [[nodiscard]] bool match_pending() const noexcept { return match_pending_; }

    [[nodiscard]] bool last_match_event() const noexcept { return last_match_event_; }
    [[nodiscard]] bool last_clear_request() const noexcept { return last_clear_request_; }
    [[nodiscard]] bool last_rollover() const noexcept { return last_rollover_; }

    static bool address_aligned(std::uint32_t address);
    static bool address_valid(std::uint32_t address);
    static std::uint32_t merge_bytes(std::uint32_t old_value,
                                     std::uint32_t new_value,
                                     std::uint8_t strobes);

private:
    bool timer_enable_{false};
    bool irq_enable_{false};
    std::uint32_t counter_{0};
    std::uint32_t compare_{0xFFFFFFFFu};
    bool match_pending_{false};

    bool last_match_event_{false};
    bool last_clear_request_{false};
    bool last_rollover_{false};
};
