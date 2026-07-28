#pragma once

#include "apb_transaction.hpp"
#include "timer_reference_model.hpp"

#include <cstdint>
#include <map>
#include <string>

class FunctionalCoverage {
public:
    void observe_transaction(const ApbTransaction& transaction);
    void observe_cycle(const TimerReferenceModel& model);
    void hit(const std::string& name);
    [[nodiscard]] std::uint64_t count(const std::string& name) const;
    void write_report(const std::string& path) const;
    void print_summary() const;

private:
    std::map<std::string, std::uint64_t> bins_;
};
