#include "functional_coverage.hpp"

#include "generated/timer_registers.hpp"

#include <fstream>
#include <iostream>
#include <stdexcept>

void FunctionalCoverage::hit(const std::string& name) {
    ++bins_[name];
}

std::uint64_t FunctionalCoverage::count(const std::string& name) const {
    const auto iterator = bins_.find(name);
    return iterator == bins_.end() ? 0u : iterator->second;
}

void FunctionalCoverage::observe_transaction(const ApbTransaction& transaction) {
    hit(transaction.direction == ApbDirection::Read ? "apb_read" : "apb_write");
    if (transaction.observed_error) {
        hit((transaction.address & 0x3u) != 0u ? "misaligned_access" : "invalid_access");
        return;
    }

    switch (transaction.address & 0xFFFu) {
        case timer_regs::CONTROL: hit("access_control"); break;
        case timer_regs::COUNTER: hit("access_counter"); break;
        case timer_regs::COMPARE: hit("access_compare"); break;
        case timer_regs::STATUS:  hit("access_status"); break;
        default:                  hit("unexpected_address_bin"); break;
    }

    if (transaction.direction == ApbDirection::Write) {
        if (transaction.strobe == 0u) {
            hit("pstrb_zero");
        } else if (transaction.strobe == 0xFu) {
            hit("pstrb_full");
        } else {
            hit("pstrb_partial");
        }
    }
}

void FunctionalCoverage::observe_cycle(const TimerReferenceModel& model) {
    if (model.last_match_event()) {
        hit("compare_match");
    }
    if (model.last_clear_request()) {
        hit("status_clear");
    }
    if (model.last_match_event() && model.last_clear_request()) {
        hit("clear_match_collision");
    }
    if (model.last_rollover()) {
        hit("counter_rollover");
    }
    if (model.irq()) {
        hit("irq_active_cycle");
    }
}

void FunctionalCoverage::write_report(const std::string& path) const {
    std::ofstream output(path);
    if (!output) {
        throw std::runtime_error("Unable to write coverage report: " + path);
    }
    output << "# Functional Coverage\n\n";
    output << "| Bin | Hits |\n|---|---:|\n";
    for (const auto& [name, hits] : bins_) {
        output << "| `" << name << "` | " << hits << " |\n";
    }
}

void FunctionalCoverage::print_summary() const {
    std::cout << "\nFunctional coverage bins:\n";
    for (const auto& [name, hits] : bins_) {
        std::cout << "  " << name << ": " << hits << '\n';
    }
}
