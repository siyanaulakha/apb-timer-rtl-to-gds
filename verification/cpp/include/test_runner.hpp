#pragma once

#include "apb_master.hpp"
#include "functional_coverage.hpp"
#include "scoreboard.hpp"
#include "simulation_context.hpp"
#include "timer_reference_model.hpp"

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

struct TestContext {
    SimulationContext& simulation;
    ApbMaster& apb;
    TimerReferenceModel& model;
    Scoreboard& scoreboard;
    FunctionalCoverage& coverage;
    std::uint64_t seed;
};

class TestRunner {
public:
    using TestFunction = std::function<void(TestContext&)>;

    TestRunner(TestContext context, std::string selected_test);
    void add(std::string name, TestFunction function);
    int run();

private:
    struct TestCase {
        std::string name;
        TestFunction function;
    };

    TestContext context_;
    std::string selected_test_;
    std::vector<TestCase> tests_;
};

void register_directed_tests(TestRunner& runner);
void register_random_tests(TestRunner& runner, unsigned operations);
