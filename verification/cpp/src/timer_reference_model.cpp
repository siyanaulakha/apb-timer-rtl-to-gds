#include "timer_reference_model.hpp"

#include "generated/timer_registers.hpp"

void TimerReferenceModel::reset() {
    timer_enable_ = false;
    irq_enable_ = false;
    counter_ = timer_regs::COUNTER_RESET;
    compare_ = timer_regs::COMPARE_RESET;
    match_pending_ = false;
    last_match_event_ = false;
    last_clear_request_ = false;
    last_rollover_ = false;
}

bool TimerReferenceModel::address_aligned(std::uint32_t address) {
    return (address & 0x3u) == 0u;
}

bool TimerReferenceModel::address_valid(std::uint32_t address) {
    if (!address_aligned(address)) {
        return false;
    }
    const auto offset = address & 0xFFFu;
    return offset == timer_regs::CONTROL || offset == timer_regs::COUNTER ||
           offset == timer_regs::COMPARE || offset == timer_regs::STATUS;
}

bool TimerReferenceModel::expected_error(std::uint32_t address) const {
    return !address_valid(address & 0xFFFu);
}

std::uint32_t TimerReferenceModel::merge_bytes(std::uint32_t old_value,
                                                std::uint32_t new_value,
                                                std::uint8_t strobes) {
    auto result = old_value;
    for (unsigned byte = 0; byte < 4; ++byte) {
        if ((strobes & (1u << byte)) != 0u) {
            const std::uint32_t mask = 0xFFu << (byte * 8u);
            result = (result & ~mask) | (new_value & mask);
        }
    }
    return result;
}

void TimerReferenceModel::clock(const DutInputs& inputs) {
    last_match_event_ = false;
    last_clear_request_ = false;
    last_rollover_ = false;

    if (!inputs.reset_n) {
        reset();
        return;
    }

    const bool transfer = inputs.select && inputs.enable;
    const bool valid_write = transfer && inputs.write && address_valid(inputs.address);
    const auto address = inputs.address & 0xFFFu;

    const bool control_write = valid_write && address == timer_regs::CONTROL &&
                               (inputs.strobe & 0x1u) != 0u;
    const bool counter_write = valid_write && address == timer_regs::COUNTER &&
                               (inputs.strobe & 0xFu) != 0u;
    const bool compare_write = valid_write && address == timer_regs::COMPARE &&
                               (inputs.strobe & 0xFu) != 0u;
    const bool clear_request = valid_write && address == timer_regs::STATUS &&
                               (inputs.strobe & 0x1u) != 0u &&
                               (inputs.write_data & 0x1u) != 0u;

    const bool automatic_increment = timer_enable_ && !control_write && !counter_write;
    const std::uint32_t incremented_counter = counter_ + 1u;
    const bool match_event = automatic_increment && incremented_counter == compare_;

    const bool next_timer_enable = control_write
        ? (inputs.write_data & timer_regs::CONTROL_TIMER_ENABLE_MASK) != 0u
        : timer_enable_;
    const bool next_irq_enable = control_write
        ? (inputs.write_data & timer_regs::CONTROL_IRQ_ENABLE_MASK) != 0u
        : irq_enable_;
    const std::uint32_t next_counter = counter_write
        ? merge_bytes(counter_, inputs.write_data, inputs.strobe)
        : (automatic_increment ? incremented_counter : counter_);
    const std::uint32_t next_compare = compare_write
        ? merge_bytes(compare_, inputs.write_data, inputs.strobe)
        : compare_;
    const bool next_pending = (match_pending_ && !clear_request) || match_event;

    last_match_event_ = match_event;
    last_clear_request_ = clear_request;
    last_rollover_ = automatic_increment && counter_ == 0xFFFFFFFFu;

    timer_enable_ = next_timer_enable;
    irq_enable_ = next_irq_enable;
    counter_ = next_counter;
    compare_ = next_compare;
    match_pending_ = next_pending;
}

std::uint32_t TimerReferenceModel::read(std::uint32_t address) const {
    const auto offset = address & 0xFFFu;
    if (!address_valid(offset)) {
        return 0u;
    }
    switch (offset) {
        case timer_regs::CONTROL:
            return (timer_enable_ ? timer_regs::CONTROL_TIMER_ENABLE_MASK : 0u) |
                   (irq_enable_ ? timer_regs::CONTROL_IRQ_ENABLE_MASK : 0u);
        case timer_regs::COUNTER:
            return counter_;
        case timer_regs::COMPARE:
            return compare_;
        case timer_regs::STATUS:
            return (match_pending_ ? timer_regs::STATUS_MATCH_PENDING_MASK : 0u) |
                   (irq() ? timer_regs::STATUS_IRQ_ACTIVE_MASK : 0u);
        default:
            return 0u;
    }
}
