module traffic_light_nios (
    input  logic       clk_50,
    input  logic       reset_n,
    input  logic [7:0] switches,

    output logic [7:0] green_leds,
    output logic [7:0] red_leds
);

    // Сюда добавим экземпляр nios_system.
    // Названия портов возьмём из сгенерированного Qsys.

endmodule