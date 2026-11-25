#include <stdint.h>
#include <unistd.h>

#include "system.h"
#include "io.h"
#include "altera_avalon_pio_regs.h"

// SW0 = 0 - дневной режим/1 - ночной режим
#define MODE_SWITCH_MASK 0x01

// Светодиоды 0 и 1 всегда выключены
#define ALL_WORKING_LEDS 0xFC

// Длительность одного шага проверки 10000 микросекунд
#define CHECK_INTERVAL_US 10000

// Устанавливаем состояние двух групп LED.
static void set_leds(uint8_t green_mask, uint8_t red_mask)
{
    IOWR_ALTERA_AVALON_PIO_DATA(PIO_GREEN_BASE, green_mask);
    IOWR_ALTERA_AVALON_PIO_DATA(PIO_RED_BASE, red_mask);
}

// Проверяем положение SW0
static int get_mode(void)
{
    uint32_t switches;

    switches = IORD_ALTERA_AVALON_PIO_DATA(PIO_SWITCHES_BASE);

    return (switches & MODE_SWITCH_MASK) ? 1 : 0;
}

// Каждые 10 мс проверяем, не изменился ли режим, 1 - изменился режим
static int wait_with_mode_check(unsigned int duration_ms,
                                int expected_mode)
{
    unsigned int elapsed_ms = 0;

    while (elapsed_ms < duration_ms)
    {
        if (get_mode() != expected_mode)
        {
            return 1;
        }

        usleep(CHECK_INTERVAL_US);
        elapsed_ms += 10;
    }

    return get_mode() != expected_mode;
}

// Дневной режим
static void run_day_mode(void)
{
    // Фаза 1 — 2 секунды, все направления запрещены.
    set_leds(0x00, 0xFC);

    if (wait_with_mode_check(2000, 0))
    {
        return;
    }

    // Фаза 2 — 7 секунд, второстепенная зеленый во все стороны, главная зеленый направо
    set_leds(0xF0, 0x0C);

    if (wait_with_mode_check(7000, 0))
    {
        return;
    }

    // Фаза 3 — 12 секунд, главная зеленый налево и прямо
    set_leds(0x0C, 0xF0);

    wait_with_mode_check(12000, 0);
}

// Ночной режим
static void run_night_mode(void)
{
    // Пока Sвключен ночной режим, все 6 загараются и гаснут каждые 0,5с
    while (get_mode() == 1)
    {
        set_leds(ALL_WORKING_LEDS, ALL_WORKING_LEDS);

        if (wait_with_mode_check(500, 1))
        {
            return;
        }

        set_leds(0x00, 0x00);

        if (wait_with_mode_check(500, 1))
        {
            return;
        }
    }
}


int main(void)
{
    // Сначала все выключено
    set_leds(0x00, 0x00);

    // Бесконечная работа контроллера
    while (1)
    {
        if (get_mode() == 0)
        {
            run_day_mode();
        }
        else
        {
            run_night_mode();
        }

        // При смене режима убираем предыдущие сигналы
        set_leds(0x00, 0x00);
    }

    return 0;
}
