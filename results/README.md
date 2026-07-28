# Results Policy

Only compact, inspectable, tool-generated evidence belongs here. Do not commit enormous temporary OpenLane directories.

Expected final evidence:

- Icarus smoke log
- Verilator regression and functional-coverage reports
- Yosys synthesis log and synthesized netlist
- OpenLane metrics JSON and flow summary
- Setup and hold timing reports
- Magic and KLayout DRC summaries
- Netgen LVS summary
- Final GDSII checksum
- Tool-version record

Until a flow is run, `results-summary.md` must say `Not measured` rather than contain estimated numbers.
