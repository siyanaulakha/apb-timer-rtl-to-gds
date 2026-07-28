#include "apb_master.hpp"

#include <sstream>

ApbMaster::ApbMaster(SimulationContext& simulation,
                     TimerReferenceModel& model,
                     Scoreboard& scoreboard,
                     FunctionalCoverage& coverage)
    : simulation_(simulation), model_(model), scoreboard_(scoreboard), coverage_(coverage) {}

void ApbMaster::advance_cycle() {
    const auto inputs = simulation_.inputs();
    model_.clock(inputs);
    simulation_.tick();
    coverage_.observe_cycle(model_);
    scoreboard_.check_bool("irq_o", simulation_.irq(), model_.irq(), simulation_.cycles());
}

void ApbMaster::reset_dut(unsigned asserted_cycles, unsigned post_reset_cycles) {
    simulation_.drive_idle();
    simulation_.set_reset(false);
    model_.reset();
    simulation_.evaluate();
    scoreboard_.check_bool("irq_o during asynchronous reset",
                           simulation_.irq(), false, simulation_.cycles());
    for (unsigned cycle = 0; cycle < asserted_cycles; ++cycle) {
        advance_cycle();
    }
    simulation_.set_reset(true);
    simulation_.evaluate();
    for (unsigned cycle = 0; cycle < post_reset_cycles; ++cycle) {
        advance_cycle();
    }
}

void ApbMaster::idle_cycles(unsigned cycles) {
    simulation_.drive_idle();
    simulation_.evaluate();
    for (unsigned cycle = 0; cycle < cycles; ++cycle) {
        advance_cycle();
    }
}

void ApbMaster::check_setup_phase() {
    simulation_.evaluate();
    scoreboard_.check_bool("PREADY during setup", simulation_.ready(), true, simulation_.cycles());
    scoreboard_.check_bool("PSLVERR during setup", simulation_.slave_error(), false, simulation_.cycles());
}

void ApbMaster::check_access_phase(const ApbTransaction& transaction,
                                   std::uint32_t expected_read_data,
                                   bool expected_error) {
    simulation_.evaluate();
    scoreboard_.check_bool("PREADY during access", simulation_.ready(), true, simulation_.cycles());
    scoreboard_.check_bool("PSLVERR during access",
                           simulation_.slave_error(), expected_error, simulation_.cycles());
    if (transaction.direction == ApbDirection::Read) {
        scoreboard_.check_u32("PRDATA", simulation_.read_data(),
                              expected_error ? 0u : expected_read_data,
                              simulation_.cycles());
    }
}

ApbTransaction ApbMaster::write(std::uint32_t address,
                                std::uint32_t data,
                                std::uint8_t strobes,
                                std::uint8_t protection) {
    return write_expect(address, data, strobes, model_.expected_error(address), protection);
}

ApbTransaction ApbMaster::write_expect(std::uint32_t address,
                                       std::uint32_t data,
                                       std::uint8_t strobes,
                                       bool expected_error,
                                       std::uint8_t protection) {
    ApbTransaction transaction;
    transaction.direction = ApbDirection::Write;
    transaction.address = address & 0xFFFu;
    transaction.write_data = data;
    transaction.strobe = strobes & 0xFu;
    transaction.protection = protection & 0x7u;
    transaction.expected_error = expected_error;
    transaction.setup_cycle = simulation_.cycles();

    simulation_.drive_bus(true, false, true, transaction.address, data,
                          transaction.strobe, transaction.protection);
    check_setup_phase();
    advance_cycle();

    simulation_.drive_bus(true, true, true, transaction.address, data,
                          transaction.strobe, transaction.protection);
    check_access_phase(transaction, 0u, expected_error);
    transaction.observed_ready = simulation_.ready();
    transaction.observed_error = simulation_.slave_error();
    advance_cycle();
    transaction.completion_cycle = simulation_.cycles();

    simulation_.drive_idle();
    simulation_.evaluate();
    coverage_.observe_transaction(transaction);
    return transaction;
}

ApbTransaction ApbMaster::read(std::uint32_t address, std::uint8_t protection) {
    ApbTransaction transaction;
    transaction.direction = ApbDirection::Read;
    transaction.address = address & 0xFFFu;
    transaction.protection = protection & 0x7u;
    transaction.expected_error = model_.expected_error(address);
    transaction.setup_cycle = simulation_.cycles();

    simulation_.drive_bus(true, false, false, transaction.address, 0u, 0u,
                          transaction.protection);
    check_setup_phase();
    advance_cycle();

    simulation_.drive_bus(true, true, false, transaction.address, 0u, 0u,
                          transaction.protection);
    const auto expected_read_data = model_.read(transaction.address);
    check_access_phase(transaction, expected_read_data, transaction.expected_error);
    transaction.read_data = simulation_.read_data();
    transaction.observed_ready = simulation_.ready();
    transaction.observed_error = simulation_.slave_error();
    advance_cycle();
    transaction.completion_cycle = simulation_.cycles();

    simulation_.drive_idle();
    simulation_.evaluate();
    coverage_.observe_transaction(transaction);
    return transaction;
}

ApbTransaction ApbMaster::read_expect(std::uint32_t address,
                                      std::uint32_t expected_data,
                                      bool expected_error) {
    auto transaction = read(address);
    scoreboard_.check_bool("explicit expected PSLVERR", transaction.observed_error,
                           expected_error, transaction.completion_cycle);
    scoreboard_.check_u32("explicit expected PRDATA", transaction.read_data,
                          expected_data, transaction.completion_cycle);
    return transaction;
}

void ApbMaster::check_irq(const std::string& label) {
    simulation_.evaluate();
    scoreboard_.check_bool(label, simulation_.irq(), model_.irq(), simulation_.cycles());
}
