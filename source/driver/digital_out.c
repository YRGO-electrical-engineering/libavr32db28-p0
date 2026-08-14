/**
 * @file AVR32DB28 digital output driver implementation details.
 */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "arch/avr/hw_platform.h"
#include "driver/digital_out.h"

// -----------------------------------------------------------------------------
static PORT_t* pin_port(const uint8_t pin)
{
    // Return a pointer to the associated I/O port, or NULL if invalid.
    if (PC3 >= pin) { return &PORTC; }
    else if (PD7 >= pin) { return &PORTD; }
    else if (PF1 >= pin) { return &PORTF; }
    else if (PA7 >= pin) { return &PORTA; }
    return NULL;
}

// -----------------------------------------------------------------------------
static uint8_t pin_mask(const uint8_t pin)
{
    // Return the bit mask with respect to the associated port.
    if (pin <= PC3) { return (uint8_t)(1U << (pin - PC0)); }
    else if (pin <= PD7) { return (uint8_t)(1U << (pin - PD1 + 1U)); }
    else if (pin <= PF1) { return (uint8_t)(1U << (pin - PF0)); }
    else if (pin <= PA7) { return (uint8_t)(1U << (pin - PA0)); }
    return 0U;
}

// -----------------------------------------------------------------------------
void digital_out_setup(void)
{
    // Configure all digital pins as outputs.
    for (uint8_t pin = PC0; pin <= PA7; ++pin)
    {
        pin_port(pin)->DIR |= pin_mask(pin);
    }
}

// -----------------------------------------------------------------------------
bool digital_out_read(const uint8_t pin)
{
    // Get the associated I/O port, return false if the pin does not exist.
    const PORT_t* port = pin_port(pin);
    if (port == NULL) { return false; }

    // Read the pin state, return true if on, false if off.
    return (bool)(port->OUT & pin_mask(pin));
}

// -----------------------------------------------------------------------------
void digital_out_write(const uint8_t pin, const bool state)
{
    // Get the associated I/O port, do nothing if the pin does not exist.
    PORT_t* port = pin_port(pin);
    if (port == NULL) { return; }

    // Set state as specified.
    if (state) { port->OUTSET = pin_mask(pin); }
    else { port->OUTCLR = pin_mask(pin); }
}

// -----------------------------------------------------------------------------
void digital_out_PC0_on(void) { digital_out_write(PC0, true); }

// -----------------------------------------------------------------------------
void digital_out_PC0_off(void) { digital_out_write(PC0, false); }

// -----------------------------------------------------------------------------
void digital_out_PC1_on(void) { digital_out_write(PC1, true); }

// -----------------------------------------------------------------------------
void digital_out_PC1_off(void) { digital_out_write(PC1, false); }

// -----------------------------------------------------------------------------
void digital_out_PC2_on(void) { digital_out_write(PC2, true); }

// -----------------------------------------------------------------------------
void digital_out_PC2_off(void) { digital_out_write(PC2, false); }

// -----------------------------------------------------------------------------
void digital_out_PC3_on(void) { digital_out_write(PC3, true); }

// -----------------------------------------------------------------------------
void digital_out_PC3_off(void) { digital_out_write(PC3, false); }

// -----------------------------------------------------------------------------
void digital_out_PD1_on(void) { digital_out_write(PD1, true); }

// -----------------------------------------------------------------------------
void digital_out_PD1_off(void) { digital_out_write(PD1, false); }

// -----------------------------------------------------------------------------
void digital_out_PD2_on(void) { digital_out_write(PD2, true); }

// -----------------------------------------------------------------------------
void digital_out_PD2_off(void) { digital_out_write(PD2, false); }

// -----------------------------------------------------------------------------
void digital_out_PD3_on(void) { digital_out_write(PD3, true); }

// -----------------------------------------------------------------------------
void digital_out_PD3_off(void) { digital_out_write(PD3, false); }

// -----------------------------------------------------------------------------
void digital_out_PD4_on(void) { digital_out_write(PD4, true); }

// -----------------------------------------------------------------------------
void digital_out_PD4_off(void) { digital_out_write(PD4, false); }

// -----------------------------------------------------------------------------
void digital_out_PD5_on(void) { digital_out_write(PD5, true); }

// -----------------------------------------------------------------------------
void digital_out_PD5_off(void) { digital_out_write(PD5, false); }

// -----------------------------------------------------------------------------
void digital_out_PD6_on(void) { digital_out_write(PD6, true); }

// -----------------------------------------------------------------------------
void digital_out_PD6_off(void) { digital_out_write(PD6, false); }

// -----------------------------------------------------------------------------
void digital_out_PD7_on(void) { digital_out_write(PD7, true); }

// -----------------------------------------------------------------------------
void digital_out_PD7_off(void) { digital_out_write(PD7, false); }

// -----------------------------------------------------------------------------
void digital_out_PF0_on(void) { digital_out_write(PF0, true); }

// -----------------------------------------------------------------------------
void digital_out_PF0_off(void) { digital_out_write(PF0, false); }

// -----------------------------------------------------------------------------
void digital_out_PF1_on(void) { digital_out_write(PF1, true); }

// -----------------------------------------------------------------------------
void digital_out_PF1_off(void) { digital_out_write(PF1, false); }

// -----------------------------------------------------------------------------
void digital_out_PA0_on(void) { digital_out_write(PA0, true); }

// -----------------------------------------------------------------------------
void digital_out_PA0_off(void) { digital_out_write(PA0, false); }

// -----------------------------------------------------------------------------
void digital_out_PA1_on(void) { digital_out_write(PA1, true); }

// -----------------------------------------------------------------------------
void digital_out_PA1_off(void) { digital_out_write(PA1, false); }

// -----------------------------------------------------------------------------
void digital_out_PA2_on(void) { digital_out_write(PA2, true); }

// -----------------------------------------------------------------------------
void digital_out_PA2_off(void) { digital_out_write(PA2, false); }

// -----------------------------------------------------------------------------
void digital_out_PA3_on(void) { digital_out_write(PA3, true); }

// -----------------------------------------------------------------------------
void digital_out_PA3_off(void) { digital_out_write(PA3, false); }

// -----------------------------------------------------------------------------
void digital_out_PA4_on(void) { digital_out_write(PA4, true); }

// -----------------------------------------------------------------------------
void digital_out_PA4_off(void) { digital_out_write(PA4, false); }

// -----------------------------------------------------------------------------
void digital_out_PA5_on(void) { digital_out_write(PA5, true); }

// -----------------------------------------------------------------------------
void digital_out_PA5_off(void) { digital_out_write(PA5, false); }

// -----------------------------------------------------------------------------
void digital_out_PA6_on(void) { digital_out_write(PA6, true); }

// -----------------------------------------------------------------------------
void digital_out_PA6_off(void) { digital_out_write(PA6, false); }

// -----------------------------------------------------------------------------
void digital_out_PA7_on(void) { digital_out_write(PA7, true); }

// -----------------------------------------------------------------------------
void digital_out_PA7_off(void) { digital_out_write(PA7, false); }
