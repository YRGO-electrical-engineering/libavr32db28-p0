/**
 * @brief Application entry point.
 */
#include "arch/avr/hw_platform.h"
#include "driver/digital_out.h"

#define LED1 PA0            // PA0 -> D13.
#define BLINK_SPEED_MS 100U // Blink speed in ms.

/**
 * @brief Set up hardware for P0.
 */
static void setup_p0(void)
{
    // Initialize all digital output pins.
    digital_out_setup();
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
        // Blink LED1 continuously.
        digital_out_write(LED1, true);
        delay_ms(BLINK_SPEED_MS);
        digital_out_write(LED1, false);
        delay_ms(BLINK_SPEED_MS);
    }
    return 0;
}
