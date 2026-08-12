/**
 * @file AVR32DB28 LED driver implementation details.
 */
#include <stdbool.h>
#include <stdint.h>

#include "arch/avr/hw_platform.h"
#include "driver/led.h"

#define PORT PORTC                     // Port register for all LEDs.
#define PWM_PERIOD_MS 10U              // PWM period in ms.
#define PERCENT_MAX 100U               // Maximum duty cycle (fully on).
#define PERCENT_DIV (PERCENT_MAX / 2U) // Percent divider.

// -----------------------------------------------------------------------------
void led_setup(void)
{
    // Configure LEDs as outputs.
    PORT.DIR |= (1U << LED1) | (1U << LED2) | (1U << LED3);
}

// -----------------------------------------------------------------------------
bool led_read(const uint8_t led)
{
    // Read the LED state, return true if on, false if off.
    return (bool)(PORT.OUT & (1U << led));
}

// -----------------------------------------------------------------------------
void led_write(const uint8_t led, const bool state)
{
    // Set LED state as specified.
    if (state) { PORT.OUTSET = (1U << led); }
    else { PORT.OUTCLR = (1U << led); }
}

// -----------------------------------------------------------------------------
void led_toggle(const uint8_t led)
{
    // Toggle the LED.
    PORT.OUTTGL = (1U << led);
}

// -----------------------------------------------------------------------------
void led_pwm(const uint8_t led, const uint8_t percent)
{
    // Check the duty cycle, do nothing if invalid.
    if (PERCENT_MAX < percent) { return; }

    // Compute on- and off time, rounded to the nearest millisecond.
    const uint8_t on  = (uint8_t)((percent * PWM_PERIOD_MS + PERCENT_DIV) / PERCENT_MAX);
    const uint8_t off = PWM_PERIOD_MS - on;

    // Run PWM cycle.
    led_write(led, true);
    delay_ms(on);
    led_write(led, false);
    delay_ms(off);
}
