# Driver reference
Every function the P0 driver exposes, what the hardware does when you call it, and the few things
worth knowing before writing a program around it. The header in `include/driver/` carries a short
summary of each function; this document is the long version.

---

## Conventions
There is one driver here, but it follows the same shape as the P1 drivers that come after it:
* **Initialize once, then use.** `digital_out_setup()` takes no arguments and prepares every pin.
  Calling it twice is harmless.
* **Pins are named constants.** `PC0`, `PD4`, `PA7`. The compiler catches a wrong name, and there
  is nothing to look up.
* **An unknown pin does nothing.** `digital_out_read()` returns false for one, and
  `digital_out_write()` does nothing at all. A program never crashes because it passed the wrong
  pin.
* **Reads report what the driver last wrote**, not what a voltmeter would measure on the pin.
* **Nothing uses interrupts.** The driver is a handful of register writes, so a program does
  exactly what its statements say, in order.

---

## The hardware
The AVR32DB28 comes in a 28-pin package, and the driver covers every general purpose pin on it:

| Port  | Pins        | Notes                                                                  |
| ----- | ----------- | ---------------------------------------------------------------------- |
| PORTA | PA0 - PA7   | All eight available. PA0 is marked `D13` on the board                   |
| PORTC | PC0 - PC3   | Only four pins on this package                                          |
| PORTD | PD1 - PD7   | PD0 has no pin on the 28-pin package, so the list starts at PD1          |
| PORTF | PF0 - PF1   | PF6 is the reset pin and is left alone, so the list stops at PF1         |

That is 21 pins in total. The device runs at 5 V on its internal 4 MHz oscillator. No external
crystal is fitted, and none is needed for anything the driver does.

---

## Things worth knowing
Three things are worth understanding before writing a program that uses more than one pin.

**Setup claims all 21 pins.** `digital_out_setup()` makes every pin in the table above an output,
not only the ones a program intends to drive. Anything wired to one of those pins is then being
driven by the device rather than reading a voltage into it, so a button or a sensor connected to
one will not work while the driver is set up.

**The pin IDs are not the port's bit numbers.** They run from 0 to 20 in the order the table lists,
so `PD1` is 4 rather than 1. The driver translates between the two; a program should always use the
names, never a raw number.

**Blocking delays stop everything.** `delay_ms()`, declared in `include/arch/avr/hw_platform.h`,
busy-waits, and nothing else happens meanwhile. That is fine for blinking an LED, and it is the
reason a program built this way cannot do two things at once.

---

## Digital output
Every pin sits on one of the four I/O ports and is driven high or low by the device. High is 5 V
and low is ground, so an LED with its cathode to ground lights when its pin is written true.

The driver offers the same thing two ways: a pair of functions per pin, which take no arguments,
and a pair that take the pin as an argument. They do exactly the same work — the per-pin functions
call the general ones — so the choice is only about which reads better.

```c
void digital_out_setup(void);
```

Configures all 21 pins as outputs. The output values are left as they are, which after a reset
means every pin starts low, so nothing switches on by itself. Call this once, before anything else
in the driver.

```c
bool digital_out_read(uint8_t pin);
```

Returns whether the pin is on, i.e. what the driver was last asked to write. Nothing is measured on
the pin itself, so a shorted or unconnected output still reads back whatever was written to it. An
unknown pin reads as false.

```c
void digital_out_write(uint8_t pin, bool state);
```

Switches the pin on when true and off when false. Only that pin changes; the others on the same
port keep whatever they had. An unknown pin is ignored.

Because the pin IDs run without gaps from `PC0` to `PA7`, a loop covers every pin at once:

```c
for (uint8_t pin = PC0; pin <= PA7; ++pin)
{
    digital_out_write(pin, true);
}
```

```c
void digital_out_PC0_on(void);
void digital_out_PC0_off(void);
```

One such pair exists for each of the 21 pins: `digital_out_PD4_on()`, `digital_out_PA7_off()` and
so on, named after the pin they drive. Each one is the same as `digital_out_write()` with that pin
and true or false, and is there so that a program driving one known pin has nothing to pass and
nothing to get wrong:

```c
digital_out_PA0_on();
delay_ms(100U);
digital_out_PA0_off();
```

There is no toggle. Inverting a pin means reading it and writing back the opposite:

```c
digital_out_write(PA0, !digital_out_read(PA0));
```

---

## Delays
`delay_ms()` is declared in `include/arch/avr/hw_platform.h` rather than in the driver, since it
belongs to the hardware platform underneath rather than to any one peripheral.

```c
void delay_ms(uint16_t ms);
```

Waits the given number of milliseconds and returns. It busy-waits, one millisecond at a time, so
the processor does nothing else until the wait is over, and a delay cannot be interrupted or cut
short. Durations from 0 to 65535 ms are accepted; 0 returns immediately.

The same function exists in both builds. On the board it waits; in the host test suite it records
the duration it was asked for instead, which is how a test can check that a driver intended to wait
without waiting itself.

---
