# Interview Notes

## Two-minute explanation

I designed a synthesizable 32-bit APB4 timer peripheral with a writable counter, programmable compare value, sticky match status, maskable level interrupt, partial byte writes, deterministic error handling, and explicitly defined simultaneous-event priority.

I verified it with a reusable C++17 environment around Verilator. An APB master drives legal setup and access phases, an independent cycle-accurate reference model predicts state, and a scoreboard checks read data, errors, ready, and interrupt behavior. The canonical regression contains 29 directed tests plus one deterministic randomized test, assertions, semantic coverage, failure seeds, and non-zero process exits.

I synthesized the RTL with Yosys and completed a Sky130 OpenLane 2 implementation targeting 100 MHz. The frozen result has zero setup, hold, slew, fanout, route DRC, Magic DRC, KLayout DRC, LVS, and antenna violations, with 710 standard cells and 5,526.55 µm² standard-cell area.

## Likely questions

### Why APB rather than AXI?

The timer is a low-bandwidth control peripheral. APB exercises realistic SoC register timing without adding unrelated AXI channel, burst, and ordering complexity.

### Why is COUNTER writable?

It supports firmware initialization and makes rollover and boundary verification deterministic instead of requiring billions of cycles.

### Why does compare use `counter + 1`?

The event is defined when the counter enters the programmed value. This removes ambiguity about whether status appears one cycle before or after the visible threshold.

### What happens when clear and match coincide?

Set dominates clear: `(pending && !clear) || match`. This avoids losing a real hardware event.

### Is this UVM?

No. It is a reusable object-oriented C++ verification environment using transaction, driver, reference-model, scoreboard, assertion, and coverage concepts.

### What does the 0.723 mW value mean?

It is an OpenLane estimate under the flow's activity assumptions. It is useful as a flow metric but is not workload-characterized silicon power.

### What would you extend next?

Add a prescaler and periodic auto-reload, connect the block through an APB interconnect to a RISC-V core, and demonstrate firmware-driven interrupts on FPGA.
