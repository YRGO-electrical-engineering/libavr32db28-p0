# AVR32DB28 Drivers for P0

A deliberately plain driver set for the AVR32DB28, written for students on an embedded engineering
course. Everything a student needs is in the header: ordinary C functions, no registers, no bit
masks and no pointers. The register work lives in the `.c` files, commented for anyone curious.

---

## Drivers

| Peripheral     | Header                                                         | Status    |
| -------------- | -------------------------------------------------------------- | --------- |
| Digital output | [include/driver/digital_out.h](./include/driver/digital_out.h) | Available |

The pins are named after the port they sit on: `PC0` - `PC3`, `PD1` - `PD7`, `PF0` - `PF1` and
`PA0` - `PA7`. Each one has its own pair of functions, so nothing has to be passed as an argument:

```c
#include "arch/avr/hw_platform.h"
#include "driver/digital_out.h"

#define BLINK_SPEED_MS 100U

int main(void)
{
    digital_out_setup();

    while (1)
    {
        // Blink PA0 every 100 ms.
        digital_out_PA0_on();
        delay_ms(BLINK_SPEED_MS);
        digital_out_PA0_off();
        delay_ms(BLINK_SPEED_MS);
    }
}
```

`digital_out_write()` and `digital_out_read()` take the pin as an argument instead, hence every pin can be driven at once via a loop:

```c
for (uint8_t pin = PC0; pin <= PA7; ++pin)
{
    digital_out_write(pin, true);
}
```

---

## Building and testing

```bash
make build         # Cross-compile to build/main.elf and a flashable build/main.hex.
make test          # Run the unit tests on the host, so no board is needed.
make format        # Format all C/C++ code with clang-format.
make format-check  # Check formatting without modifying files.
```

Flashing over UPDI:

```bash
avrdude -c serialupdi -p avr32db28 -P /dev/ttyUSB0 -U flash:w:build/main.hex:i
```

---

## Toolchain

```bash
sudo apt -y update
sudo apt -y install gcc-avr binutils-avr avr-libc avrdude g++ clang-format
git submodule update --init  # yrgo::test, which the test suite is built against.
```

The firmware build also needs the AVR-Dx device family pack, since `avr-libc` ships neither the
device specs nor `<avr/io.h>` for this part:

```bash
wget http://packs.download.atmel.com/Atmel.AVR-Dx_DFP.1.10.114.atpack
unzip -q -d dfp Atmel.AVR-Dx_DFP.1.10.114.atpack
```

The build looks in `dfp/` by default; set `DFP_DIR` to use a pack from elsewhere.

---

## Layout

```text
main.c                            Application entry point
Makefile                          Targets for building, testing and formatting

include/driver/digital_out.h      Driver API for digital outputs, i.e. what students use
source/driver/digital_out.c       Driver implementation details, i.e. the register access

include/arch/avr/hw_platform.h    Hardware platform, selecting real or mocked registers
include/arch/test/hw_platform.h   Mocked AVR registers, declared as plain variables in RAM
source/arch/avr/hw_platform.c     delay_ms for the target, busy-waiting
source/arch/test/hw_platform.c    delay_ms for the tests, recorded instead of waited out

test/testsuite.cpp                Test suite entry point
test/driver/digital_out_test.cpp  Tests for the digital output driver
test/arch/hw_platform_test.cpp    Tests for the mocked hardware platform
test/Makefile                     Builds and runs the host test suite

ci/build.sh                       Cross-compiles the firmware
ci/test.sh                        Builds and runs the test suite
ci/format.sh                      Formats, or checks the formatting of, all C/C++ code
libs/test/                        The yrgo::test framework, a git submodule
```

The two `hw_platform.h` files are what let one driver serve both builds: the firmware gets the real
AVR registers, the test suite gets mocked ones in RAM, and the driver code is compiled unchanged
either way.

Every push and pull request to `main` runs the build, the tests and the formatting check. See
[.github/workflows/ci.yml](./.github/workflows/ci.yml).

---
