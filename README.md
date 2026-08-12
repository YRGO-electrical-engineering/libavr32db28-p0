# AVR32DB28 Drivers for P0

## About
A small, deliberately plain set of drivers for the AVR32DB28 microcontroller, written for students
on an embedded engineering course. The set is scoped to project 0 (P0), so it covers the LEDs and
switches that P0 uses and nothing else.

The drivers assume no previous experience with microcontrollers. Everything a student needs is in
the header files: each one exposes a handful of ordinary C functions and no hardware detail at all.
There are no registers, no bit masks and no pointers to learn before switching on an LED. The
register work lives in the `.c` files, commented throughout for anyone curious enough to open them.

That simplicity is a deliberate constraint rather than a limitation of the hardware:

* **No interrupts.** Everything is polled, so the flow of a program is the order of its statements.
* **Blocking where blocking is simpler.** Non-blocking variants are provided where they are genuinely
  useful, not everywhere.
* **No configuration a beginner doesn't need.** Alternate pin mappings and clock tuning are left out
  until a course exercise calls for them.

Experienced readers will find features missing on purpose. The
[AVR32DB28 data sheet](https://www.microchip.com/en-us/product/AVR32DB28) documents what the
hardware can do beyond what the drivers expose.

---

## Drivers

| Peripheral | Header                                         | Status    |
| ---------- | ---------------------------------------------- | --------- |
| LED        | [include/driver/led.h](./include/driver/led.h) | Available |
| Switch     | [include/driver/sw.h](./include/driver/sw.h)   | Available |

These are the only two peripherals P0 uses, so the rest of the device is left alone.

---

## Usage
Each driver is initialized once, and then used from the main loop. Lighting every LED while its
switch is held down is a complete program:

```c
#include "driver/led.h"
#include "driver/sw.h"

int main(void)
{
    led_setup();
    sw_setup();

    while (1)
    {
        led_write(LED1, sw_read(SW1));
        led_write(LED2, sw_read(SW2));
        led_write(LED3, sw_read(SW3));
    }
}
```

The LEDs are called `LED1` - `LED3` and the switches `SW1` - `SW3`. Those names are the only
hardware a program needs to know about: `sw_read()` reports whether a switch is pressed, 
`led_write()` switches an LED on or off, `led_toggle()` inverts one, and `led_read()` reports whether it is lit.

`led_pwm()` dims a LED rather than switching it fully on. One call runs a single PWM period, so the
LED stays dimmed only for as long as the calls keep coming:

```c
while (1)
{
    // Run LED1 at 30 %, or at full brightness while SW1 is held down.
    if (sw_read(SW1)) { led_pwm(LED1, 100U); }
    else { led_pwm(LED1, 30U); }
}
```

A program like this goes in `main.c`, which is built with `make build` and flashed as described
below.

---

## Structure

```text
ci/          Scripts for compilation, testing, and code formatting
include/     Driver headers, i.e. the API students use
libs/        The yrgo::test framework, checked out as a git submodule
source/      Driver implementations and the mocked hardware platform
test/        Unit tests, run on the host against the mocked hardware
main.c       Application entry point
```

`include/arch/` and `source/arch/` hold the hardware platform. It selects the real AVR registers
when building firmware, and a mocked set of registers in RAM when building the tests, so the same
driver code compiles for both. It also provides `delay_ms`, which busy-waits on the target.

---

## Toolchain
Building the firmware needs `avr-gcc` and the AVR-Dx device family pack (DFP). `avr-libc` does not
ship the device specs or the `<avr/io.h>` header for this part, so the pack supplies them.

`avr-gcc` is installed via `apt` on WSL:

```bash
sudo apt -y update
sudo apt -y install gcc-avr binutils-avr avr-libc avrdude
```

The device family pack is downloaded and unpacked once:

```bash
wget http://packs.download.atmel.com/Atmel.AVR-Dx_DFP.1.10.114.atpack
unzip -q -d dfp Atmel.AVR-Dx_DFP.1.10.114.atpack
```

The build looks for the pack in `dfp/` by default. Set `DFP_DIR` to use a pack from somewhere else,
such as a local Microchip Studio installation.

---

## Compilation
The root `Makefile` cross-compiles the firmware for the AVR32DB28:

```bash
make build
```

The result is written to `build/main.elf`, along with a flashable `build/main.hex`. Flashing over
UPDI:

```bash
avrdude -c serialupdi -p avr32db28 -P /dev/ttyUSB0 -U flash:w:build/main.hex:i
```

---

## Tests
The drivers are unit tested against a mocked AVR32DB28, so **the tests run on the host and no board
is needed**:

```bash
make test
```

The mock provides the peripheral registers as ordinary variables in RAM. A test calls a driver
function and then checks which register it wrote and with what value, which is how the tests catch a
driver reaching for the wrong register without anyone plugging in hardware. Delays are recorded
instead of waited out, so testing a driver that waits costs the suite no time at all.

The suite is built against the [yrgo::test](https://github.com/yrgo-libs/yrgo-test) framework, which
lives in `libs/test` as a git submodule. Check it out once before running the tests:

```bash
git submodule update --init
```

A C++17-capable compiler (e.g. `g++`) needs to be installed and available on `PATH`. The drivers
themselves are compiled as C; only the tests are C++.

---

## Code Formatting
The root `Makefile` formats all C/C++ code with `clang-format`:

```bash
make format        # Format all files.
make format-check  # Check formatting without modifying files.
```

`clang-format` needs to be installed and available on `PATH`:

```bash
sudo apt -y update
sudo apt -y install clang-format
```

---

## Continuous Integration
Every push and pull request to `main` runs three jobs: a firmware cross-compile, the test suite, and
the formatting check. See [.github/workflows/ci.yml](./.github/workflows/ci.yml).

---
