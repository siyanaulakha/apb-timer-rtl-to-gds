# Illustrative standalone-STA constraints.
# The generic `synthesis/synth.ys` flow does not read this file and therefore
# does not make timing-closure claims. Physical implementation uses
# `openlane/pnr.sdc` and `openlane/signoff.sdc` instead.
create_clock -name PCLK -period 10.000 [get_ports PCLK]
set_clock_uncertainty 0.200 [get_clocks PCLK]
set_input_delay  1.000 -clock PCLK [remove_from_collection [all_inputs] [get_ports {PCLK PRESETn}]]
set_output_delay 1.000 -clock PCLK [all_outputs]
set_false_path -from [get_ports PRESETn]
