/**
 * @brief Application entry point.
 */
#include "driver/led.h"
#include "driver/sw.h"

/**
 * @brief Set up hardware for P0.
 */
static void setup_p0(void)
{
    // Initialize the LEDs and the switches.
    led_setup();
    sw_setup();
}

/**
 * @brief Run the application.
 *
 * @return This function never returns.
 */
int main(void)
{
    setup_p0();

    while (1)
    {
        // Continuously read SW1, enable LED1 when pressed.
        if (sw_read(SW1)) { led_write(LED1, true); }
        else { led_write(LED1, false); }
    }
    return 0;
}
