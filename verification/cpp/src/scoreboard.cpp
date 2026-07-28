#include "scoreboard.hpp"

#include <iomanip>
#include <iostream>
#include <sstream>

void Scoreboard::begin_test(const std::string& name) {
    current_test_ = name;
}

void Scoreboard::check(bool condition, const std::string& message, std::uint64_t cycle) {
    ++checks_;
    if (!condition) {
        failures_.push_back(Failure{current_test_, message, cycle});
        std::cerr << "[FAIL] " << current_test_ << " cycle=" << cycle
                  << " " << message << '\n';
    }
}

void Scoreboard::check_bool(const std::string& label,
                            bool actual,
                            bool expected,
                            std::uint64_t cycle) {
    std::ostringstream message;
    message << label << " expected=" << expected << " observed=" << actual;
    check(actual == expected, message.str(), cycle);
}

void Scoreboard::check_u32(const std::string& label,
                           std::uint32_t actual,
                           std::uint32_t expected,
                           std::uint64_t cycle) {
    std::ostringstream message;
    message << label << " expected=0x" << std::hex << std::setw(8) << std::setfill('0')
            << expected << " observed=0x" << std::setw(8) << actual;
    check(actual == expected, message.str(), cycle);
}

std::size_t Scoreboard::failures_since(std::size_t baseline) const noexcept {
    return failures_.size() >= baseline ? failures_.size() - baseline : failures_.size();
}

void Scoreboard::print_summary() const {
    std::cout << "\nScoreboard: " << (failures_.empty() ? "PASS" : "FAIL")
              << " checks=" << checks_ << " failures=" << failures_.size() << '\n';
    if (!failures_.empty()) {
        std::cout << "Failure records:\n";
        for (const auto& failure : failures_) {
            std::cout << "  - " << failure.test << " @ cycle " << failure.cycle
                      << ": " << failure.message << '\n';
        }
    }
}
