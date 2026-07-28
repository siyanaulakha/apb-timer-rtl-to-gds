`timescale 1ns/1ps
`default_nettype none

module apb_timer_smoke_tb;
    localparam logic [11:0] CONTROL = 12'h000;
    localparam logic [11:0] COUNTER = 12'h004;
    localparam logic [11:0] COMPARE = 12'h008;
    localparam logic [11:0] STATUS  = 12'h00C;

    logic PCLK = 1'b0;
    logic PRESETn = 1'b0;
    logic PSEL = 1'b0;
    logic PENABLE = 1'b0;
    logic PWRITE = 1'b0;
    logic [11:0] PADDR = '0;
    logic [31:0] PWDATA = '0;
    logic [3:0] PSTRB = '0;
    logic [2:0] PPROT = '0;
    logic [31:0] PRDATA;
    logic PREADY;
    logic PSLVERR;
    logic irq_o;

    int checks = 0;
    int failures = 0;

    apb_timer dut (.*);

    always #5 PCLK = ~PCLK;

    task automatic check_equal32(
        input logic [31:0] actual,
        input logic [31:0] expected,
        input string label
    );
        checks++;
        if (actual !== expected) begin
            failures++;
            $error("%s: expected 0x%08h, observed 0x%08h", label, expected, actual);
        end
    endtask

    task automatic check_equal1(
        input logic actual,
        input logic expected,
        input string label
    );
        checks++;
        if (actual !== expected) begin
            failures++;
            $error("%s: expected %0b, observed %0b", label, expected, actual);
        end
    endtask

    task automatic apb_write(
        input logic [11:0] address,
        input logic [31:0] data,
        input logic [3:0] strobes,
        input logic expected_error
    );
        @(negedge PCLK);
        PSEL = 1'b1;
        PENABLE = 1'b0;
        PWRITE = 1'b1;
        PADDR = address;
        PWDATA = data;
        PSTRB = strobes;
        @(posedge PCLK);
        @(negedge PCLK);
        PENABLE = 1'b1;
        #1;
        check_equal1(PREADY, 1'b1, "PREADY on write");
        check_equal1(PSLVERR, expected_error, "PSLVERR on write");
        @(posedge PCLK);
        @(negedge PCLK);
        PSEL = 1'b0;
        PENABLE = 1'b0;
        PWRITE = 1'b0;
        PADDR = '0;
        PWDATA = '0;
        PSTRB = '0;
    endtask

    task automatic apb_read(
        input logic [11:0] address,
        input logic [31:0] expected_data,
        input logic expected_error
    );
        @(negedge PCLK);
        PSEL = 1'b1;
        PENABLE = 1'b0;
        PWRITE = 1'b0;
        PADDR = address;
        PSTRB = '0;
        @(posedge PCLK);
        @(negedge PCLK);
        PENABLE = 1'b1;
        #1;
        check_equal1(PREADY, 1'b1, "PREADY on read");
        check_equal1(PSLVERR, expected_error, "PSLVERR on read");
        check_equal32(PRDATA, expected_data, "PRDATA");
        @(posedge PCLK);
        @(negedge PCLK);
        PSEL = 1'b0;
        PENABLE = 1'b0;
        PADDR = '0;
    endtask

    initial begin
        $dumpfile("build/apb_timer_smoke.vcd");
        $dumpvars(0, apb_timer_smoke_tb);

        repeat (3) @(posedge PCLK);
        @(negedge PCLK);
        PRESETn = 1'b1;
        @(posedge PCLK);
        #1;

        apb_read(CONTROL, 32'h0000_0000, 1'b0);
        apb_read(COUNTER, 32'h0000_0000, 1'b0);
        apb_read(COMPARE, 32'hFFFF_FFFF, 1'b0);
        apb_read(STATUS,  32'h0000_0000, 1'b0);

        apb_write(COMPARE, 32'd5, 4'b1111, 1'b0);
        apb_write(COUNTER, 32'd2, 4'b1111, 1'b0);
        apb_write(CONTROL, 32'b11, 4'b0001, 1'b0);

        repeat (3) @(posedge PCLK);
        // Sample after the nonblocking updates from the third count edge have settled.
        // A falling-edge observation is portable across Icarus scheduling versions.
        @(negedge PCLK);
        check_equal1(irq_o, 1'b1, "interrupt after compare");
        apb_read(STATUS, 32'h0000_0003, 1'b0);

        apb_write(STATUS, 32'h0000_0001, 4'b0001, 1'b0);
        check_equal1(irq_o, 1'b0, "interrupt clear");

        apb_write(12'h010, 32'hDEAD_BEEF, 4'b1111, 1'b1);
        apb_read(12'h006, 32'h0000_0000, 1'b1);

        apb_write(CONTROL, 32'h0000_0000, 4'b0001, 1'b0);

        if (failures == 0) begin
            $display("SMOKE PASS: %0d checks", checks);
            $finish;
        end else begin
            $fatal(1, "SMOKE FAIL: %0d of %0d checks failed", failures, checks);
        end
    end
endmodule

`default_nettype wire
