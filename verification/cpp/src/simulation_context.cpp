#include "simulation_context.hpp"

#include "Vapb_timer.h"
#include "verilated.h"
#include "verilated_vcd_c.h"

#include <stdexcept>

SimulationContext::SimulationContext(int argc,
                                     char** argv,
                                     bool enable_trace,
                                     const std::string& trace_path)
    : context_(std::make_unique<VerilatedContext>()),
      dut_(nullptr),
      trace_(nullptr) {
    context_->commandArgs(argc, argv);
    context_->traceEverOn(enable_trace);
    dut_ = std::make_unique<Vapb_timer>(context_.get(), "TOP");
    dut_->PCLK = 0;
    dut_->PRESETn = 0;
    drive_idle();
    evaluate();

    if (enable_trace) {
        trace_ = std::make_unique<VerilatedVcdC>();
        dut_->trace(trace_.get(), 99);
        trace_->open(trace_path.c_str());
        dump_trace();
    }
}

SimulationContext::~SimulationContext() {
    if (dut_) {
        dut_->final();
    }
    if (trace_) {
        trace_->close();
    }
}

void SimulationContext::set_reset(bool reset_n) {
    dut_->PRESETn = reset_n ? 1 : 0;
}

void SimulationContext::drive_idle() {
    drive_bus(false, false, false, 0u, 0u, 0u, 0u);
}

void SimulationContext::drive_bus(bool select,
                                  bool enable,
                                  bool write,
                                  std::uint32_t address,
                                  std::uint32_t write_data,
                                  std::uint8_t strobe,
                                  std::uint8_t protection) {
    dut_->PSEL = select ? 1 : 0;
    dut_->PENABLE = enable ? 1 : 0;
    dut_->PWRITE = write ? 1 : 0;
    dut_->PADDR = address & 0xFFFu;
    dut_->PWDATA = write_data;
    dut_->PSTRB = strobe & 0xFu;
    dut_->PPROT = protection & 0x7u;
}

void SimulationContext::evaluate() {
    dut_->eval();
    dump_trace();
}

void SimulationContext::tick() {
    dut_->PCLK = 0;
    dut_->eval();
    dump_trace();

    context_->timeInc(5);
    dut_->PCLK = 1;
    dut_->eval();
    dump_trace();

    context_->timeInc(5);
    dut_->PCLK = 0;
    dut_->eval();
    dump_trace();
    ++cycles_;
}

DutInputs SimulationContext::inputs() const {
    DutInputs values;
    values.reset_n = dut_->PRESETn != 0;
    values.select = dut_->PSEL != 0;
    values.enable = dut_->PENABLE != 0;
    values.write = dut_->PWRITE != 0;
    values.address = dut_->PADDR;
    values.write_data = dut_->PWDATA;
    values.strobe = static_cast<std::uint8_t>(dut_->PSTRB);
    values.protection = static_cast<std::uint8_t>(dut_->PPROT);
    return values;
}

std::uint32_t SimulationContext::read_data() const {
    return dut_->PRDATA;
}

bool SimulationContext::ready() const {
    return dut_->PREADY != 0;
}

bool SimulationContext::slave_error() const {
    return dut_->PSLVERR != 0;
}

bool SimulationContext::irq() const {
    return dut_->irq_o != 0;
}

std::uint64_t SimulationContext::time() const {
    return context_->time();
}

void SimulationContext::dump_trace() {
    if (trace_) {
        trace_->dump(context_->time());
    }
}
