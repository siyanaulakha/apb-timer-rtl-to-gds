#pragma once

#include <cstdint>
#include <string>
#include <vector>

class Scoreboard {
public:
    struct Failure {
        std::string test;
        std::string message;
        std::uint64_t cycle{0};
    };

    void begin_test(const std::string& name);
    void check(bool condition, const std::string& message, std::uint64_t cycle);
    void check_bool(const std::string& label,
                    bool actual,
                    bool expected,
                    std::uint64_t cycle);
    void check_u32(const std::string& label,
                   std::uint32_t actual,
                   std::uint32_t expected,
                   std::uint64_t cycle);

    [[nodiscard]] std::size_t checks() const noexcept { return checks_; }
    [[nodiscard]] std::size_t failures() const noexcept { return failures_.size(); }
    [[nodiscard]] std::size_t failures_since(std::size_t baseline) const noexcept;
    [[nodiscard]] const std::vector<Failure>& failure_records() const noexcept { return failures_; }
    void print_summary() const;

private:
    std::string current_test_{"unscoped"};
    std::size_t checks_{0};
    std::vector<Failure> failures_;
};
