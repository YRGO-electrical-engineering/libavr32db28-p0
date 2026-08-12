/**
 * @file AVR32DB28 switch driver implementation details.
 */
#include <stdbool.h>
#include <stdint.h>

#include "arch/avr/hw_platform.h"
#include "driver/sw.h"

#define SW_PIN1 3U // Switch PIN 1 (PORTC3).
#define SW_PIN2 0U // Switch PIN 2 (PORTF0).
#define SW_PIN3 1U // Switch PIN 3 (PORTF1).

// -----------------------------------------------------------------------------
void sw_setup(void)
{
    // Configure the switches as inputs.
    PORTC.DIR &= ~(1U << SW_PIN1);
    PORTF.DIR &= ~(1U << SW_PIN2);
    PORTF.DIR &= ~(1U << SW_PIN3);

    // Enable the internal pull-up resistors, so that the inputs don't float.
    PORTC.PIN3CTRL = PORT_PULLUPEN_bm;
    PORTF.PIN0CTRL = PORT_PULLUPEN_bm;
    PORTF.PIN1CTRL = PORT_PULLUPEN_bm;
}

// -----------------------------------------------------------------------------
bool sw_read(const uint8_t sw)
{
    // The switches are connected to ground, so a pressed switch reads low.
    switch (sw)
    {
        case SW1:
            return !(bool)(PORTC.IN & (1U << SW_PIN1));
        case SW2:
            return !(bool)(PORTF.IN & (1U << SW_PIN2));
        case SW3:
            return !(bool)(PORTF.IN & (1U << SW_PIN3));
        default:
            return false;
    }
}
