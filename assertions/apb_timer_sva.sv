`default_nettype none

module apb_timer_sva (
    input logic                  PCLK,
    input logic                  PRESETn,
    input logic                  PSEL,
    input logic                  PENABLE,
    input logic                  PWRITE,
    input logic [11:0] PADDR,
    input logic [31:0]           PWDATA,
    input logic [3:0]            PSTRB,
    input logic [2:0]            PPROT,
    input logic [31:0]           PRDATA,
    input logic                  PREADY,
    input logic                  PSLVERR,
    input logic                  irq_o
);

`ifdef APB_TIMER_ENABLE_SVA
    default clocking cb @(posedge PCLK); endclocking

    apb_access_requires_select: assert property (disable iff (!PRESETn) PENABLE |-> PSEL);
    ready_is_constant:          assert property (disable iff (!PRESETn) PREADY == 1'b1);
    error_only_in_access:       assert property (disable iff (!PRESETn) PSLVERR |-> (PSEL && PENABLE));

    apb_control_stable_during_wait: assert property (disable iff (!PRESETn) 
        PSEL && PENABLE && !PREADY |=>
        $stable({PADDR, PWRITE, PWDATA, PSTRB, PPROT})
    );

    irq_known: assert property (disable iff (!PRESETn) !$isunknown(irq_o));
    read_data_known_on_valid_read: assert property (disable iff (!PRESETn) 
        PSEL && PENABLE && !PWRITE && PREADY && !PSLVERR |-> !$isunknown(PRDATA)
    );
`endif

endmodule

`default_nettype wire

`ifdef APB_TIMER_ENABLE_SVA
bind apb_timer apb_timer_sva apb_timer_sva_i (.*);
`endif
