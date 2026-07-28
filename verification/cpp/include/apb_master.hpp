#pragma once

#include "apb_transaction.hpp"
#include "functional_coverage.hpp"
#include "scoreboard.hpp"
#include "simulation_context.hpp"
#include "timer_reference_model.hpp"

#include <cstdint>

class ApbMaster {
public:
    ApbMaster(SimulationContext& simulation,
              TimerReferenceModel& model,
              Scoreboard& scoreboard,
              FunctionalCoverage& coverage);

    void reset_dut(unsigned asserted_cycles = 3, unsigned post_reset_cycles = 1);
    void idle_cycles(unsigned cycles);

    ApbTransaction write(std::uint32_t address,
                         std::uint32_t data,
                         std::uint8_t strobes = 0xFu,
                         std::uint8_t protection = 0u);
    ApbTransaction read(std::uint32_t address, std::uint8_t protection = 0u);

    ApbTransaction write_expect(std::uint32_t address,
                                std::uint32_t data,
                                std::uint8_t strobes,
                                bool expected_error,
                                std::uint8_t protection = 0u);
    ApbTransaction read_expect(std::uint32_t address,
                               std::uint32_t expected_data,
                               bool expected_error = false);

    void check_irq(const std::string& label = "irq_o");

private:
    void advance_cycle();
    void check_setup_phase();
    void check_access_phase(const ApbTransaction& transaction,
                            std::uint32_t expected_read_data,
                            bool expected_error);

    SimulationContext& simulation_;
    TimerReferenceModel& model_;
    Scoreboard& scoreboard_;
    FunctionalCoverage& coverage_;
};
