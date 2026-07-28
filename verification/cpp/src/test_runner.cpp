#include "test_runner.hpp"

#include <exception>
#include <iostream>
#include <utility>

TestRunner::TestRunner(TestContext context, std::string selected_test)
    : context_(context), selected_test_(std::move(selected_test)) {}

void TestRunner::add(std::string name, TestFunction function) {
    tests_.push_back(TestCase{std::move(name), std::move(function)});
}

int TestRunner::run() {
    unsigned executed = 0;
    unsigned passed = 0;

    for (const auto& test : tests_) {
        if (!selected_test_.empty() && selected_test_ != test.name) {
            continue;
        }
        ++executed;
        context_.scoreboard.begin_test(test.name);
        const auto failures_before = context_.scoreboard.failures();
        std::cout << "[ RUN      ] " << test.name << '\n';
        try {
            context_.apb.reset_dut();
            test.function(context_);
        } catch (const std::exception& error) {
            context_.scoreboard.check(false,
                                      std::string("uncaught exception: ") + error.what(),
                                      context_.simulation.cycles());
        } catch (...) {
            context_.scoreboard.check(false, "unknown uncaught exception",
                                      context_.simulation.cycles());
        }

        if (context_.scoreboard.failures() == failures_before) {
            ++passed;
            std::cout << "[       OK ] " << test.name << '\n';
        } else {
            std::cout << "[  FAILED  ] " << test.name << '\n';
        }
    }

    if (executed == 0) {
        std::cerr << "No test matched selection: " << selected_test_ << '\n';
        return 2;
    }

    std::cout << "\nTests: " << passed << " passed, " << (executed - passed)
              << " failed, " << executed << " executed\n";
    return passed == executed ? 0 : 1;
}
