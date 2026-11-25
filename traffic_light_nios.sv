
module traffic_light_nios (
    input  logic       clk_50,
    input  logic       reset_n,
    input  logic [7:0] switches,

    output logic [7:0] green_leds,
    output logic [7:0] red_leds
);

    // Процессорная система Nios II из Qsys.
    nios_system u0 (
        // Тактовая частота 50 МГц.
        .clk_clk           (clk_50),

        // KEY0: нажатие даёт 0.
        // Qsys ожидает активный высокий уровень сброса.
        .reset_reset_n       (reset_n),

        // Переключатели платы DE1.
        .switches_export   (switches),

        // Красные и зелёные светодиоды.
        .red_leds_export   (red_leds),
        .green_leds_export (green_leds)
    );

endmodule
