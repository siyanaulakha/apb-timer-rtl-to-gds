#pragma once

#include <cstdint>
#include <memory>
#include <string>

class Vapb_timer;
class VerilatedContext;
class VerilatedVcdC;

struct DutInputs {
    bool reset_n{false};
    bool select{false};
    bool enable{false};
    bool write{false};
    std::uint32_t address{0};
    std::uint32_t write_data{0};
    std::uint8_t strobe{0};
    std::uint8_t protection{0};
};

class SimulationContext {
public:
    SimulationContext(int argc, char** argv, bool enable_trace, const std::string& trace_path);
    ~SimulationContext();

    SimulationContext(const SimulationContext&) = delete;
    SimulationContext& operator=(const SimulationContext&) = delete;

    void set_reset(bool reset_n);
    void drive_idle();
    void drive_bus(bool select,
                   bool enable,
                   bool write,
                   std::uint32_t address,
                   std::uint32_t write_data,
                   std::uint8_t strobe,
                   std::uint8_t protection = 0);
    void evaluate();
    void tick();

    [[nodiscard]] DutInputs inputs() const;
    [[nodiscard]] std::uint32_t read_data() const;
    [[nodiscard]] bool ready() const;
    [[nodiscard]] bool slave_error() const;
    [[nodiscard]] bool irq() const;
    [[nodiscard]] std::uint64_t cycles() const noexcept { return cycles_; }
    [[nodiscard]] std::uint64_t time() const;

private:
    void dump_trace();

    std::unique_ptr<VerilatedContext> context_;
    std::unique_ptr<Vapb_timer> dut_;
    std::unique_ptr<VerilatedVcdC> trace_;
    std::uint64_t cycles_{0};
};
