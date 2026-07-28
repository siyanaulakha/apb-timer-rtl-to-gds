#include "apb_master.hpp"
#include "functional_coverage.hpp"
#include "scoreboard.hpp"
#include "simulation_context.hpp"
#include "test_runner.hpp"
#include "timer_reference_model.hpp"

#include "verilated_cov.h"

#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {

struct Options {
    std::string selected_test;
    std::uint64_t seed{107u};
    unsigned random_operations{500u};
    bool trace{false};
    std::string trace_path{"build/verilator/apb_timer.vcd"};
    std::string coverage_path{"results/verification/functional-coverage.md"};
};

Options parse_options(int argc, char** argv) {
    Options options;
    for (int index = 1; index < argc; ++index) {
        const std::string argument = argv[index];
        auto require_value = [&](const char* name) -> std::string {
            if (index + 1 >= argc) {
                throw std::invalid_argument(std::string("Missing value for ") + name);
            }
            return argv[++index];
        };

        if (argument == "--test") {
            options.selected_test = require_value("--test");
        } else if (argument == "--seed") {
            options.seed = std::stoull(require_value("--seed"));
        } else if (argument == "--random-operations") {
            options.random_operations = static_cast<unsigned>(
                std::stoul(require_value("--random-operations")));
        } else if (argument == "--trace") {
            options.trace = true;
        } else if (argument == "--trace-path") {
            options.trace_path = require_value("--trace-path");
        } else if (argument == "--coverage-path") {
            options.coverage_path = require_value("--coverage-path");
        } else if (argument == "--help") {
            std::cout << "Usage: apb_timer_verification [options]\n"
                      << "  --test NAME              Run one named test\n"
                      << "  --seed N                 Deterministic random seed\n"
                      << "  --random-operations N    Number of random operations\n"
                      << "  --trace                   Enable VCD waveform\n"
                      << "  --trace-path PATH         VCD output path\n"
                      << "  --coverage-path PATH      Markdown coverage report\n";
            std::exit(0);
        }
    }
    return options;
}

void create_parent_directory(const std::string& path) {
    const auto parent = std::filesystem::path(path).parent_path();
    if (!parent.empty()) {
        std::filesystem::create_directories(parent);
    }
}

}  // namespace

int main(int argc, char** argv) {
    try {
        const auto options = parse_options(argc, argv);
        create_parent_directory(options.trace_path);
        create_parent_directory(options.coverage_path);

        std::cout << "APB Timer C++ Verification\n"
                  << "seed=" << options.seed
                  << " random_operations=" << options.random_operations
                  << " trace=" << options.trace << "\n\n";

        SimulationContext simulation(argc, argv, options.trace, options.trace_path);
        TimerReferenceModel model;
        Scoreboard scoreboard;
        FunctionalCoverage coverage;
        ApbMaster apb(simulation, model, scoreboard, coverage);
        TestContext context{simulation, apb, model, scoreboard, coverage, options.seed};
        TestRunner runner(context, options.selected_test);

        register_directed_tests(runner);
        register_random_tests(runner, options.random_operations);
        const int test_result = runner.run();

#if VM_COVERAGE
        VerilatedCov::write("coverage.dat");
#endif

        coverage.write_report(options.coverage_path);
        coverage.print_summary();
        scoreboard.print_summary();

        if (test_result == 2) {
            return 2;
        }
        return scoreboard.failures() == 0u ? 0 : 1;
    } catch (const std::exception& error) {
        std::cerr << "Fatal verification error: " << error.what() << '\n';
        return 2;
    }
}
