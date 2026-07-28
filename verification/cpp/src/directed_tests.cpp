#include "test_runner.hpp"

#include "generated/timer_registers.hpp"

#include <cstdint>

namespace {

void expect_irq(TestContext& context, bool expected, const char* label) {
    context.simulation.evaluate();
    context.scoreboard.check_bool(label, context.simulation.irq(), expected,
                                  context.simulation.cycles());
}

}  // namespace

void register_directed_tests(TestRunner& runner) {
    runner.add("T01_reset_values", [](TestContext& c) {
        c.apb.read_expect(timer_regs::CONTROL, 0x00000000u);
        c.apb.read_expect(timer_regs::COUNTER, 0x00000000u);
        c.apb.read_expect(timer_regs::COMPARE, 0xFFFFFFFFu);
        c.apb.read_expect(timer_regs::STATUS, 0x00000000u);
        expect_irq(c, false, "IRQ reset value");
    });

    runner.add("T02_control_write_readback", [](TestContext& c) {
        c.apb.write(timer_regs::CONTROL, 0x00000003u, 0x1u);
        c.apb.read_expect(timer_regs::CONTROL, 0x00000003u);
    });

    runner.add("T03_control_byte_strobe", [](TestContext& c) {
        c.apb.write(timer_regs::CONTROL, 0x00000003u, 0x2u);
        c.apb.read_expect(timer_regs::CONTROL, 0x00000000u);
        c.apb.write(timer_regs::CONTROL, 0x00000003u, 0x1u);
        c.apb.read_expect(timer_regs::CONTROL, 0x00000003u);
    });

    runner.add("T04_compare_full_write", [](TestContext& c) {
        c.apb.write(timer_regs::COMPARE, 0x12345678u);
        c.apb.read_expect(timer_regs::COMPARE, 0x12345678u);
    });

    runner.add("T05_compare_partial_write", [](TestContext& c) {
        c.apb.write(timer_regs::COMPARE, 0xA1B2C3D4u, 0x5u);
        c.apb.read_expect(timer_regs::COMPARE, 0xFFB2FFD4u);
    });

    runner.add("T06_counter_full_write", [](TestContext& c) {
        c.apb.write(timer_regs::COUNTER, 0x89ABCDEFu);
        c.apb.read_expect(timer_regs::COUNTER, 0x89ABCDEFu);
    });

    runner.add("T07_counter_partial_write", [](TestContext& c) {
        c.apb.write(timer_regs::COUNTER, 0xA1B2C3D4u, 0xAu);
        c.apb.read_expect(timer_regs::COUNTER, 0xA100C300u);
    });

    runner.add("T08_disabled_counter_holds", [](TestContext& c) {
        c.apb.write(timer_regs::COUNTER, 5u);
        c.apb.idle_cycles(8);
        c.apb.read_expect(timer_regs::COUNTER, 5u);
    });

    runner.add("T09_enabled_counter_increments", [](TestContext& c) {
        c.apb.write(timer_regs::COUNTER, 0u);
        c.apb.write(timer_regs::CONTROL, 1u, 0x1u);
        c.apb.idle_cycles(3);
        c.apb.read_expect(timer_regs::COUNTER, 4u);
    });

    runner.add("T10_control_write_suppresses_increment", [](TestContext& c) {
        c.apb.write(timer_regs::COUNTER, 0u);
        c.apb.write(timer_regs::CONTROL, 1u, 0x1u);
        c.apb.write(timer_regs::CONTROL, 1u, 0x1u);
        c.apb.read_expect(timer_regs::COUNTER, 2u);
    });

    runner.add("T11_counter_write_suppresses_increment", [](TestContext& c) {
        c.apb.write(timer_regs::CONTROL, 1u, 0x1u);
        c.apb.write(timer_regs::COUNTER, 100u);
        c.apb.read_expect(timer_regs::COUNTER, 101u);
    });

    runner.add("T12_compare_sets_pending", [](TestContext& c) {
        c.apb.write(timer_regs::COMPARE, 3u);
        c.apb.write(timer_regs::COUNTER, 0u);
        c.apb.write(timer_regs::CONTROL, 1u, 0x1u);
        c.apb.idle_cycles(3);
        c.apb.read_expect(timer_regs::STATUS, 0x00000001u);
    });

    runner.add("T13_match_irq_disabled", [](TestContext& c) {
        c.apb.write(timer_regs::COMPARE, 2u);
        c.apb.write(timer_regs::CONTROL, 1u, 0x1u);
        c.apb.idle_cycles(2);
        expect_irq(c, false, "IRQ remains masked");
        c.apb.read_expect(timer_regs::STATUS, 0x00000001u);
    });

    runner.add("T14_match_irq_enabled", [](TestContext& c) {
        c.apb.write(timer_regs::COMPARE, 2u);
        c.apb.write(timer_regs::CONTROL, 3u, 0x1u);
        c.apb.idle_cycles(2);
        expect_irq(c, true, "IRQ asserted at compare");
        c.apb.read_expect(timer_regs::STATUS, 0x00000003u);
    });

    runner.add("T15_enable_irq_with_pending_match", [](TestContext& c) {
        c.apb.write(timer_regs::COMPARE, 2u);
        c.apb.write(timer_regs::CONTROL, 1u, 0x1u);
        c.apb.idle_cycles(2);
        expect_irq(c, false, "IRQ initially masked");
        c.apb.write(timer_regs::CONTROL, 3u, 0x1u);
        expect_irq(c, true, "IRQ asserted when mask enabled");
        c.apb.read_expect(timer_regs::STATUS, 0x00000003u);
    });

    runner.add("T16_disable_irq_preserves_pending", [](TestContext& c) {
        c.apb.write(timer_regs::COMPARE, 2u);
        c.apb.write(timer_regs::CONTROL, 3u, 0x1u);
        c.apb.idle_cycles(2);
        c.apb.write(timer_regs::CONTROL, 1u, 0x1u);
        expect_irq(c, false, "IRQ deasserted by mask");
        c.apb.read_expect(timer_regs::STATUS, 0x00000001u);
    });

    runner.add("T17_status_w1c", [](TestContext& c) {
        c.apb.write(timer_regs::COMPARE, 2u);
        c.apb.write(timer_regs::CONTROL, 3u, 0x1u);
        c.apb.idle_cycles(2);
        c.apb.write(timer_regs::STATUS, 1u, 0x1u);
        expect_irq(c, false, "IRQ cleared through W1C");
        c.apb.read_expect(timer_regs::STATUS, 0u);
    });

    runner.add("T18_clear_match_collision_set_dominant", [](TestContext& c) {
        c.apb.write(timer_regs::COMPARE, 1u);
        c.apb.write(timer_regs::COUNTER, 0u);
        c.apb.write(timer_regs::CONTROL, 3u, 0x1u);
        c.apb.idle_cycles(1);
        expect_irq(c, true, "Initial pending interrupt");

        c.apb.write(timer_regs::CONTROL, 2u, 0x1u);
        c.apb.write(timer_regs::COMPARE, 20u);
        c.apb.write(timer_regs::COUNTER, 18u);
        c.apb.write(timer_regs::CONTROL, 3u, 0x1u);
        c.apb.write(timer_regs::STATUS, 1u, 0x1u);

        expect_irq(c, true, "Match dominates simultaneous clear");
        c.apb.read_expect(timer_regs::STATUS, 3u);
        c.scoreboard.check(c.coverage.count("clear_match_collision") > 0u,
                           "clear/match collision coverage bin was not hit",
                           c.simulation.cycles());
    });

    runner.add("T19_disable_restart", [](TestContext& c) {
        c.apb.write(timer_regs::COUNTER, 0u);
        c.apb.write(timer_regs::CONTROL, 1u, 0x1u);
        c.apb.idle_cycles(2);
        c.apb.write(timer_regs::CONTROL, 0u, 0x1u);
        c.apb.idle_cycles(3);
        c.apb.read_expect(timer_regs::COUNTER, 3u);
        c.apb.write(timer_regs::CONTROL, 1u, 0x1u);
        c.apb.idle_cycles(2);
        c.apb.write(timer_regs::CONTROL, 0u, 0x1u);
        c.apb.read_expect(timer_regs::COUNTER, 6u);
    });

    runner.add("T20_counter_rollover", [](TestContext& c) {
        c.apb.write(timer_regs::COUNTER, 0xFFFFFFFEu);
        c.apb.write(timer_regs::CONTROL, 1u, 0x1u);
        c.apb.idle_cycles(1);
        c.apb.write(timer_regs::CONTROL, 0u, 0x1u);
        c.apb.read_expect(timer_regs::COUNTER, 0u);
        c.scoreboard.check(c.coverage.count("counter_rollover") > 0u,
                           "rollover coverage bin was not hit", c.simulation.cycles());
    });

    runner.add("T21_compare_zero_rollover_match", [](TestContext& c) {
        c.apb.write(timer_regs::COMPARE, 0u);
        c.apb.write(timer_regs::COUNTER, 0xFFFFFFFEu);
        c.apb.write(timer_regs::CONTROL, 3u, 0x1u);
        c.apb.idle_cycles(2);
        expect_irq(c, true, "COMPARE=0 match on rollover");
        c.apb.read_expect(timer_regs::STATUS, 3u);
    });

    runner.add("T22_back_to_back_transfers", [](TestContext& c) {
        const auto first = c.apb.write(timer_regs::COMPARE, 0x11223344u);
        const auto second = c.apb.write(timer_regs::COUNTER, 0x55667788u);
        const auto third = c.apb.write(timer_regs::CONTROL, 3u, 0x1u);
        c.scoreboard.check(second.setup_cycle == first.completion_cycle,
                           "second transfer did not start immediately after first",
                           c.simulation.cycles());
        c.scoreboard.check(third.setup_cycle == second.completion_cycle,
                           "third transfer did not start immediately after second",
                           c.simulation.cycles());
        c.apb.read_expect(timer_regs::CONTROL, 3u);
    });

    runner.add("T23_invalid_aligned_address", [](TestContext& c) {
        c.apb.write(timer_regs::COUNTER, 0x12345678u);
        c.apb.write_expect(0x010u, 0xDEADBEEFu, 0xFu, true);
        c.apb.read_expect(0x010u, 0u, true);
        c.apb.read_expect(timer_regs::COUNTER, 0x12345678u);
    });

    runner.add("T24_misaligned_address", [](TestContext& c) {
        c.apb.write_expect(0x006u, 0xCAFEBABEu, 0xFu, true);
        c.apb.read_expect(0x00Du, 0u, true);
        c.apb.read_expect(timer_regs::COMPARE, 0xFFFFFFFFu);
    });

    runner.add("T25_zero_strobe_noop", [](TestContext& c) {
        c.apb.write(timer_regs::COUNTER, 0xAA55AA55u, 0x0u);
        c.apb.read_expect(timer_regs::COUNTER, 0u);

        c.apb.write(timer_regs::CONTROL, 1u, 0x1u);
        c.apb.write(timer_regs::COUNTER, 0xDEADBEEFu, 0x0u);
        c.apb.read_expect(timer_regs::COUNTER, 3u);
    });

    runner.add("T26_compare_change_while_running", [](TestContext& c) {
        c.apb.write(timer_regs::COUNTER, 0u);
        c.apb.write(timer_regs::COMPARE, 10u);
        c.apb.write(timer_regs::CONTROL, 3u, 0x1u);
        c.apb.write(timer_regs::COMPARE, 20u);
        expect_irq(c, false, "No spurious event from compare write");
        c.apb.idle_cycles(17);
        expect_irq(c, false, "No early event before new compare");
        c.apb.idle_cycles(1);
        expect_irq(c, true, "New compare active after write edge");
    });

    runner.add("T27_old_compare_used_on_write_edge", [](TestContext& c) {
        c.apb.write(timer_regs::COUNTER, 10u);
        c.apb.write(timer_regs::COMPARE, 12u);
        c.apb.write(timer_regs::CONTROL, 3u, 0x1u);
        c.apb.write(timer_regs::COMPARE, 100u);
        expect_irq(c, true, "Old compare value used on compare-write edge");
        c.apb.read_expect(timer_regs::COMPARE, 100u);
        c.apb.read_expect(timer_regs::STATUS, 3u);
    });

    runner.add("T28_reset_during_active_counting", [](TestContext& c) {
        c.apb.write(timer_regs::COMPARE, 3u);
        c.apb.write(timer_regs::CONTROL, 3u, 0x1u);
        c.apb.idle_cycles(3);
        expect_irq(c, true, "IRQ before reset");
        c.apb.reset_dut();
        c.apb.read_expect(timer_regs::CONTROL, 0u);
        c.apb.read_expect(timer_regs::COUNTER, 0u);
        c.apb.read_expect(timer_regs::COMPARE, 0xFFFFFFFFu);
        c.apb.read_expect(timer_regs::STATUS, 0u);
    });

    runner.add("T29_repeated_match_while_pending", [](TestContext& c) {
        c.apb.write(timer_regs::COMPARE, 2u);
        c.apb.write(timer_regs::COUNTER, 0u);
        c.apb.write(timer_regs::CONTROL, 3u, 0x1u);
        c.apb.idle_cycles(2);
        expect_irq(c, true, "First compare match");
        const auto matches_before = c.coverage.count("compare_match");
        c.apb.write(timer_regs::COUNTER, 1u);
        c.apb.idle_cycles(1);
        expect_irq(c, true, "Pending remains asserted after repeated match");
        c.scoreboard.check(c.coverage.count("compare_match") > matches_before,
                           "second compare event was not observed",
                           c.simulation.cycles());
        c.apb.read_expect(timer_regs::STATUS, 3u);
    });
}
