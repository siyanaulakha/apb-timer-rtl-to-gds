#pragma once

#include <cstdint>
#include <string>

enum class ApbDirection { Read, Write };

struct ApbTransaction {
    ApbDirection direction{ApbDirection::Read};
    std::uint32_t address{0};
    std::uint32_t write_data{0};
    std::uint8_t strobe{0};
    std::uint8_t protection{0};
    bool expected_error{false};

    std::uint32_t read_data{0};
    bool observed_ready{false};
    bool observed_error{false};
    std::uint64_t setup_cycle{0};
    std::uint64_t completion_cycle{0};

    [[nodiscard]] std::string describe() const;
};
