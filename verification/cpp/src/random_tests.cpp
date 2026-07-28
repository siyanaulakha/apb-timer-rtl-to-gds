#include "test_runner.hpp"

#include "generated/timer_registers.hpp"

#include <array>
#include <cstdint>
#include <random>

void register_random_tests(TestRunner& runner, unsigned operations) {
    runner.add("T30_randomized_apb_sequences", [operations](TestContext& c) {
        std::mt19937_64 random(c.seed);
        const std::array<std::uint32_t, 10> addresses = {
            timer_regs::CONTROL,
            timer_regs::COUNTER,
            timer_regs::COMPARE,
            timer_regs::STATUS,
            0x010u,
            0x100u,
            0x001u,
            0x006u,
            0x00Du,
            0xFFFu,
        };

        for (unsigned operation = 0; operation < operations; ++operation) {
            const auto choice = static_cast<unsigned>(random() % 100u);
            if (choice < 8u) {
                c.apb.idle_cycles(static_cast<unsigned>(random() % 5u));
            } else if (choice < 11u) {
                c.coverage.hit("random_reset");
                c.apb.reset_dut(2, 1);
            } else {
                const auto address = addresses[random() % addresses.size()];
                if ((random() & 1u) == 0u) {
                    c.apb.read(address, static_cast<std::uint8_t>(random() & 0x7u));
                } else {
                    c.apb.write(address,
                                static_cast<std::uint32_t>(random()),
                                static_cast<std::uint8_t>(random() & 0xFu),
                                static_cast<std::uint8_t>(random() & 0x7u));
                }
            }
        }

        c.apb.read(timer_regs::CONTROL);
        c.apb.read(timer_regs::COUNTER);
        c.apb.read(timer_regs::COMPARE);
        c.apb.read(timer_regs::STATUS);
    });
}
