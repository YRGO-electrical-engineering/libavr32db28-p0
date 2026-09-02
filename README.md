# AVR32DB28 Drivers for P0
A deliberately plain driver set for the AVR32DB28, written for the P0 hardware development hat.

Students design the hat themselves in *Tillämpad elektronik och hållbar utveckling*, and then
learn to code on it in *Programmeringsmetodik*, where these drivers are what they build on. That
is what the drivers are for: while learning to code, the interesting part is the program rather
than the hardware, so everything a student needs is in the header:
* Ordinary C functions.
* No registers.
* No bit masks.
* No pointers. 

Registers and the rest of the hardware detail come in the programming course after this one; until then the register work lives in the `.c` files, commented for anyone curious.

---

## Drivers

| Peripheral     | Header                                                         | Status    |
| -------------- | -------------------------------------------------------------- | --------- |
| Digital output | [include/driver/digital_out.h](./include/driver/digital_out.h) | Available |

**[docs/drivers.md](./docs/drivers.md) describes every function in this header**, together with
the hardware behind it.

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

## Getting started in Microchip Studio

This is the shortest route to a blinking LED: nothing to install but Microchip Studio itself, and
no command line at all. The `make` targets further down are an alternative, not a prerequisite.

### 1. Get the code

Either download it as a ZIP, which needs no Git:

1. Open the repository on GitHub and press `Code > Download ZIP`.
2. Right-click the downloaded file and choose `Extract All`.
3. Extract somewhere short and simple, such as `C:\avr`. Do not work inside the ZIP file itself;
   Windows will happily show its contents, but nothing can be built from in there.

GitHub names the extracted folder `libavr32db28-p0-main`, which is the same thing as
`libavr32db28-p0` and can be renamed if the `-main` bothers you.

Or clone it, if Git is installed:

```bash
git clone https://github.com/YRGO-electrical-engineering/libavr32db28-p0.git
```

Cloning is worth the trouble if you expect to pick up later fixes, since `git pull` then replaces
downloading the ZIP again. Either way is fine for building in Microchip Studio.

### 2. Open the project

Double-click `libavr32db28-p0.atsln` in the folder you just unpacked or cloned. Microchip Studio
opens with the whole project in the `Solution Explorer` on the right: `main.c` at the top, the
drivers under `include/` and `source/`, and the tests under `test/`.

Opening `main.c` instead of the `.atsln` file also shows the code, but Studio then has no project
around it and the build commands stay greyed out. Always start from the `.atsln`.

### 3. Build

Press `F7`, or `Build > Build Solution`. The `Output` window should end with `Build succeeded`,
and report a program size of well under a kilobyte.

If it instead complains about an unknown device, the AVR-Dx device family pack is missing. Install
it under `Tools > Device Pack Manager`, search for `AVR-Dx_DFP`, then build again. The project is
set up for pack version 1.10.114; another version works too, it just means correcting the two pack
paths under `Project > Properties > Toolchain`.

### 4. Program the board

With an Atmel-ICE, or a Curiosity board with a programmer built in:

1. Connect the board and open `Tools > Device Programming`.
2. Pick your tool, set `Device` to `AVR32DB28` and `Interface` to `UPDI`, then press `Apply`.
3. Press `Read` under `Device signature` to confirm the board is answering.
4. Go to `Memories`, check that the `Flash` box points at `Debug\libavr32db28-p0.hex`, and press
   `Program`.

Selecting the same tool under `Project > Properties > Tool` lets you skip all of that afterwards
and simply press `Ctrl+Alt+F5` to build and program in one go.

With a plain serial UPDI cable, i.e. a USB-to-serial adapter wired to the UPDI pin, Microchip
Studio is of no use: it only talks to Microchip's own programmers. Use `make flash` instead, as
described under [Building and testing](#building-and-testing). It picks up the `Debug/` output
Studio just produced, so nothing has to be built twice.

### 5. What you should see

`main.c` blinks the LED on `PA0`, marked `D13` on the board, five times a second: 100 ms on and
100 ms off. Once that works, the file is yours to change; everything the drivers offer is listed
under [Drivers](#drivers) above.

The `test/` folder is in the project so it can be read, but it is not built by Studio. Those tests
run on the computer rather than on the board, and are covered under
[Building and testing](#building-and-testing). Ignore them entirely if you like; nothing on the
board depends on them.

---

## Building and testing

```bash
make build         # Cross-compile to build/main.elf and a flashable build/main.hex.
make flash         # Build, then flash the board over UPDI.
make test          # Run the unit tests on the host, so no board is needed.
make format        # Format all C/C++ code with clang-format.
make format-check  # Check formatting without modifying files.
```

`make flash` expects a serial UPDI adapter, i.e. a USB-to-serial cable wired to the UPDI pin. The
port defaults to `COM3` on Windows and `/dev/ttyUSB0` elsewhere, so pass another one if the board
turns up somewhere else:

```bash
make flash PORT=COM4
```

That is the same as running avrdude directly:

```bash
avrdude -c serialupdi -p avr32db28 -P COM3 -b 115200 -U flash:w:build/main.hex:i
```

---

## Toolchain

Building from the terminal needs `avr-gcc`, `avrdude` and `clang-format` on `PATH`.
**[docs/toolchain.md](./docs/toolchain.md) installs them step by step**, on Windows in Git Bash
as well as on Linux, and lists what each `make` target actually uses. The short version, on Linux
or WSL:

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

The build looks in `dfp/` first, then in a local Microchip Studio installation, which it finds by
itself if Studio is installed. Set `DFP_DIR` to point at a pack somewhere else.

---

## Layout

```text
main.c                            Application entry point
Makefile                          Targets for building, testing and formatting
libavr32db28-p0.atsln             Microchip Studio solution, opening the project below
libavr32db28-p0.cproj             Microchip Studio project, building the firmware
docs/drivers.md                   Driver reference, i.e. every function described in full
docs/toolchain.md                 Installing avr-gcc, avrdude and clang-format for the terminal build

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
ci/flash.sh                       Flashes the firmware to the board over UPDI
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
