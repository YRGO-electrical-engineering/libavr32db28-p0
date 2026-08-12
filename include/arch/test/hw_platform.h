/**
 * @brief Mocked hardware platform for AVR32DB28.
 *
 *        The AVR Dx peripherals are accessed as struct members (PORTC.OUTSET, PORTF.IN), so the
 *        mock simply declares the same struct types and provides real instances in RAM. Driver
 *        code therefore compiles unchanged against the mock.
 *
 *        The mock is plain storage: writing PORTC.OUTSET does not propagate into PORTC.OUT the
 *        way silicon does. Tests assert on the register the driver actually wrote, for example
 *        EXPECT_EQ(PORTC.OUTSET, PIN0_bm).
 *
 *        Register layouts follow ioavr32db28.h, reduced to the registers the drivers use. The
 *        mock is not address-mapped, so reserved padding is omitted and leaving out a register
 *        no driver touches is harmless. Add peripherals, registers and bit masks here as new
 *        drivers need them.
 */
#ifdef TESTSUITE

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/** I/O port. */
typedef struct
{
    volatile uint8_t DIR;      /* Data direction. */
    volatile uint8_t OUT;      /* Output value. */
    volatile uint8_t OUTSET;   /* Output value set. */
    volatile uint8_t OUTCLR;   /* Output value clear. */
    volatile uint8_t OUTTGL;   /* Output value toggle. */
    volatile uint8_t IN;       /* Input value. */
    volatile uint8_t PIN0CTRL; /* Pin 0 control. */
    volatile uint8_t PIN1CTRL; /* Pin 1 control. */
    volatile uint8_t PIN2CTRL; /* Pin 2 control. */
    volatile uint8_t PIN3CTRL; /* Pin 3 control. */
} PORT_t;

/** Mocked peripheral instances. Only the ports the drivers use are provided. */
extern PORT_t PORTA;
extern PORT_t PORTC;
extern PORT_t PORTD;
extern PORT_t PORTF;

/* Pin bit masks. */
#define PIN0_bm 0x01
#define PIN1_bm 0x02
#define PIN2_bm 0x04
#define PIN3_bm 0x08
#define PIN4_bm 0x10
#define PIN5_bm 0x20
#define PIN6_bm 0x40
#define PIN7_bm 0x80

/* PORT bit masks. */
#define PORT_PULLUPEN_bm 0x08

/**
 * @brief Reset every mocked register to zero and discard the recorded delays.
 *
 *        Call this at the start of each test case, so that state does not leak between tests.
 */
void testHwPlatformReset(void);

/**
 * @brief Get the number of delay_ms calls made since the last reset.
 *
 *        The mocked delay_ms records the duration it was asked for rather than sleeping, so a
 *        test can check how long a driver intended to wait without waiting itself.
 *
 * @return Number of calls.
 */
uint16_t testDelayCount(void);

/**
 * @brief Get the duration passed to one of the recorded delay_ms calls.
 *
 * @param[in] index Zero based index of the call.
 *
 * @return Duration in milliseconds, or zero if no such call was recorded.
 */
uint16_t testDelayAt(uint16_t index);

#ifdef __cplusplus
}
#endif

#endif // TESTSUITE
