/**
 * @brief Tests for the mocked hardware platform.
 */
#include <cstdint>

#include "arch/avr/hw_platform.h"
#include "yrgo/test/test.h"

namespace
{
constexpr std::uint8_t zero{0U};

/** Durations used to check that delays are recorded in the order they were requested. */
constexpr std::uint16_t firstDelayMs{5U};
constexpr std::uint16_t secondDelayMs{7U};

/** A delay far longer than the test suite could afford to actually sleep for. */
constexpr std::uint16_t longDelayMs{60000U};
} // namespace

/**
 * @brief Test that the mocked registers start out cleared.
 */
TEST(HwPlatform, ResetClearsRegisters)
{
    PORTA.DIR      = PIN7_bm;
    PORTC.OUTSET   = PIN0_bm;
    PORTD.IN       = PIN1_bm;
    PORTF.PIN0CTRL = PORT_PULLUPEN_bm;

    testHwPlatformReset();

    EXPECT_EQ(PORTA.DIR, zero);
    EXPECT_EQ(PORTC.OUTSET, zero);
    EXPECT_EQ(PORTD.IN, zero);
    EXPECT_EQ(PORTF.PIN0CTRL, zero);
}

/**
 * @brief Test that the port registers behave as independent storage.
 *
 *        The mock does not propagate OUTSET into OUT the way silicon does, which is why the
 *        driver tests assert on the exact register a driver wrote.
 */
TEST(HwPlatform, PortRegistersAreIndependent)
{
    testHwPlatformReset();

    PORTA.OUTSET = PIN7_bm;
    PORTA.OUTCLR = PIN0_bm;

    EXPECT_EQ(PORTA.OUTSET, PIN7_bm);
    EXPECT_EQ(PORTA.OUTCLR, PIN0_bm);
    EXPECT_EQ(PORTA.OUT, zero);
    EXPECT_EQ(PORTA.IN, zero);
}

/**
 * @brief Test that the ports are distinct objects.
 */
TEST(HwPlatform, PortsAreDistinct)
{
    testHwPlatformReset();

    PORTC.PIN0CTRL = PORT_PULLUPEN_bm;

    EXPECT_EQ(PORTC.PIN0CTRL, PORT_PULLUPEN_bm);
    EXPECT_EQ(PORTA.PIN0CTRL, zero);
    EXPECT_EQ(PORTD.PIN0CTRL, zero);
    EXPECT_EQ(PORTF.PIN0CTRL, zero);
}

/**
 * @brief Test that delays are recorded in the order they were requested.
 */
TEST(HwPlatform, DelaysAreRecorded)
{
    testHwPlatformReset();

    delay_ms(firstDelayMs);
    delay_ms(secondDelayMs);

    EXPECT_EQ(testDelayCount(), 2U);
    EXPECT_EQ(testDelayAt(0U), firstDelayMs);
    EXPECT_EQ(testDelayAt(1U), secondDelayMs);
}

/**
 * @brief Test that a delay returns immediately instead of sleeping.
 *
 *        A minute of real sleep would stall the suite, so the test completing at all is the
 *        assertion; the duration still has to be recorded faithfully.
 */
TEST(HwPlatform, DelaysDoNotSleep)
{
    testHwPlatformReset();

    delay_ms(longDelayMs);

    EXPECT_EQ(testDelayCount(), 1U);
    EXPECT_EQ(testDelayAt(0U), longDelayMs);
}

/**
 * @brief Test that resetting the platform discards the recorded delays.
 */
TEST(HwPlatform, ResetClearsRecordedDelays)
{
    testHwPlatformReset();
    delay_ms(firstDelayMs);
    testHwPlatformReset();

    EXPECT_EQ(testDelayCount(), zero);
    EXPECT_EQ(testDelayAt(0U), zero);
}

/**
 * @brief Test that reading a delay that was never recorded is harmless.
 */
TEST(HwPlatform, ReadingAnUnrecordedDelayReturnsZero)
{
    testHwPlatformReset();
    delay_ms(firstDelayMs);

    EXPECT_EQ(testDelayAt(1U), zero);
    EXPECT_EQ(testDelayAt(1000U), zero);
}
