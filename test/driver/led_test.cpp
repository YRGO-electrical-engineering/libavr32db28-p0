/**
 * @file Tests for the AVR32DB28 LED driver.
 */
#include <cstdint>

#include "arch/avr/hw_platform.h"
#include "yrgo/test/test.h"

extern "C"
{
#include "driver/led.h"
} // extern "C"

namespace
{
constexpr std::uint8_t zero{0U};

/** Bit masks of the pins driving each LED. LED1 - LED3 sit on PC0 - PC2, see led.h. */
constexpr std::uint8_t Led1Mask{PIN0_bm};
constexpr std::uint8_t Led2Mask{PIN1_bm};
constexpr std::uint8_t Led3Mask{PIN2_bm};

/** Bit mask of all three LEDs. */
constexpr std::uint8_t AllLedsMask{Led1Mask | Led2Mask | Led3Mask};

/** Length of one PWM period in milliseconds, mirroring PWM_PERIOD_MS in led.c. */
constexpr std::uint16_t PwmPeriodMs{10U};

/** Number of delays one PWM period consists of, i.e. the on time and the off time. */
constexpr std::uint16_t PwmDelayCount{2U};

/** Number of delays a rejected PWM call is allowed to make. */
constexpr std::uint16_t NoDelays{0U};

/**
 * @brief A duty cycle, together with the on time in milliseconds it is expected to produce.
 */
struct PwmCase
{
    std::uint8_t percent; ///< Duty cycle passed to led_pwm.
    std::uint16_t onTime; ///< Expected on time in milliseconds.
};

/** Duty cycles landing exactly on a brightness step, and the on times they produce. */
constexpr PwmCase PwmSteps[]{{0U, 0U}, {10U, 1U}, {50U, 5U}, {90U, 9U}, {100U, 10U}};

/** Duty cycles landing between two steps, and the on times they round to. */
constexpr PwmCase PwmRounded[]{{4U, 0U}, {5U, 1U}, {14U, 1U}, {15U, 2U}, {94U, 9U}, {95U, 10U}};

/**
 * @brief Test that initializing the LEDs configures every one of them as an output.
 *
 *        All three direction bits must be set at once. Setting only one of them, e.g. by
 *        shifting a constant instead of the loop variable, is caught here.
 */
TEST(Led, SetupConfiguresAllLedsAsOutputs)
{
    testHwPlatformReset();
    led_setup();
    EXPECT_EQ(PORTC.DIR, AllLedsMask);
}

/**
 * @brief Test that the LEDs are left off after initialization.
 */
TEST(Led, SetupLeavesTheLedsOff)
{
    testHwPlatformReset();
    led_setup();

    EXPECT_EQ(PORTC.OUT, zero);
    EXPECT_EQ(PORTC.OUTSET, zero);
    EXPECT_EQ(PORTC.OUTCLR, zero);
    EXPECT_EQ(PORTC.OUTTGL, zero);
}

/**
 * @brief Test that initializing the LEDs leaves the other I/O ports untouched.
 */
TEST(Led, SetupLeavesOtherPortsUntouched)
{
    testHwPlatformReset();
    led_setup();

    EXPECT_EQ(PORTA.DIR, zero);
    EXPECT_EQ(PORTD.DIR, zero);
    EXPECT_EQ(PORTF.DIR, zero);
}

/**
 * @brief Test that turning a LED on sets the corresponding bit in OUTSET.
 */
TEST(Led, WriteHighTurnsLedOn)
{
    testHwPlatformReset();
    led_setup();
    led_write(LED1, true);

    EXPECT_EQ(PORTC.OUTSET, Led1Mask);
    EXPECT_EQ(PORTC.OUTCLR, zero);
}

/**
 * @brief Test that turning a LED off sets the corresponding bit in OUTCLR.
 */
TEST(Led, WriteLowTurnsLedOff)
{
    testHwPlatformReset();
    led_setup();
    led_write(LED1, false);

    EXPECT_EQ(PORTC.OUTCLR, Led1Mask);
    EXPECT_EQ(PORTC.OUTSET, zero);
}

/**
 * @brief Test that each LED drives its own pin, i.e. LED1 - LED3 map to PC0 - PC2.
 *
 *        The mock stores whatever the driver writes rather than merging it into OUT, so each
 *        write replaces the previous value of OUTSET instead of adding to it.
 */
TEST(Led, EachLedMapsToItsOwnPin)
{
    testHwPlatformReset();
    led_setup();

    led_write(LED1, true);
    EXPECT_EQ(PORTC.OUTSET, Led1Mask);

    led_write(LED2, true);
    EXPECT_EQ(PORTC.OUTSET, Led2Mask);

    led_write(LED3, true);
    EXPECT_EQ(PORTC.OUTSET, Led3Mask);
}

/**
 * @brief Test that toggling a LED sets the corresponding bit in OUTTGL.
 *
 *        The hardware inverts the output itself, so the driver must not read the current state
 *        and write it back: OUTSET and OUTCLR have to stay untouched.
 */
TEST(Led, ToggleInvertsLed)
{
    testHwPlatformReset();
    led_setup();
    led_toggle(LED2);

    EXPECT_EQ(PORTC.OUTTGL, Led2Mask);
    EXPECT_EQ(PORTC.OUTSET, zero);
    EXPECT_EQ(PORTC.OUTCLR, zero);
}

/**
 * @brief Test that driving a LED leaves the other I/O ports untouched.
 */
TEST(Led, WriteAndToggleLeaveOtherPortsUntouched)
{
    testHwPlatformReset();
    led_setup();
    led_write(LED3, true);
    led_toggle(LED3);

    EXPECT_EQ(PORTA.OUTSET, zero);
    EXPECT_EQ(PORTA.OUTCLR, zero);
    EXPECT_EQ(PORTA.OUTTGL, zero);
    EXPECT_EQ(PORTD.OUTSET, zero);
    EXPECT_EQ(PORTD.OUTCLR, zero);
    EXPECT_EQ(PORTD.OUTTGL, zero);
    EXPECT_EQ(PORTF.OUTSET, zero);
    EXPECT_EQ(PORTF.OUTCLR, zero);
    EXPECT_EQ(PORTF.OUTTGL, zero);
}

/**
 * @brief Test that reading a LED returns the state held in the OUT register.
 *
 *        OUT is seeded directly rather than through led_write, since the mock does not
 *        propagate OUTSET into OUT the way silicon does.
 */
TEST(Led, ReadReturnsLedState)
{
    testHwPlatformReset();
    led_setup();
    PORTC.OUT = Led1Mask;

    EXPECT_TRUE(led_read(LED1));
    EXPECT_FALSE(led_read(LED2));
}

/**
 * @brief Test that reading a LED is unaffected by the states of the other LEDs.
 */
TEST(Led, ReadIgnoresOtherLeds)
{
    testHwPlatformReset();
    led_setup();
    PORTC.OUT = static_cast<std::uint8_t>(~Led2Mask);

    EXPECT_FALSE(led_read(LED2));
    EXPECT_TRUE(led_read(LED1));
    EXPECT_TRUE(led_read(LED3));
}

/**
 * @brief Test that reading a LED doesn't disturb any register.
 */
TEST(Led, ReadDoesNotWriteRegisters)
{
    testHwPlatformReset();
    (void)led_read(LED1);

    EXPECT_EQ(PORTC.DIR, zero);
    EXPECT_EQ(PORTC.OUT, zero);
    EXPECT_EQ(PORTC.OUTSET, zero);
    EXPECT_EQ(PORTC.OUTCLR, zero);
    EXPECT_EQ(PORTC.OUTTGL, zero);
}

/**
 * @brief Test that every LED can be driven, not just the ones used in the tests above.
 */
TEST(Led, EveryLedCanBeDriven)
{
    for (std::uint8_t led{LED1}; led <= LED3; ++led)
    {
        const auto mask = static_cast<std::uint8_t>(1U << led);

        testHwPlatformReset();
        led_setup();

        led_write(led, true);
        EXPECT_EQ(PORTC.OUTSET, mask);

        led_write(led, false);
        EXPECT_EQ(PORTC.OUTCLR, mask);

        led_toggle(led);
        EXPECT_EQ(PORTC.OUTTGL, mask);
    }
}

/**
 * @brief Test a full LED sequence: initialize, turn on, turn off and toggle.
 */
TEST(Led, LedSequence)
{
    testHwPlatformReset();
    led_setup();
    EXPECT_EQ(PORTC.DIR, AllLedsMask);

    led_write(LED2, true);
    EXPECT_EQ(PORTC.OUTSET, Led2Mask);

    led_write(LED2, false);
    EXPECT_EQ(PORTC.OUTCLR, Led2Mask);

    led_toggle(LED2);
    EXPECT_EQ(PORTC.OUTTGL, Led2Mask);
}

/**
 * @brief Test that one PWM period turns the LED on and then off again.
 *
 *        The mock keeps OUTSET and OUTCLR apart, so both halves of the period stay visible
 *        after the call has returned.
 */
TEST(Led, PwmTurnsTheLedOnAndThenOff)
{
    constexpr std::uint8_t halfBrightness{50U};

    testHwPlatformReset();
    led_setup();
    led_pwm(LED1, halfBrightness);

    EXPECT_EQ(PORTC.OUTSET, Led1Mask);
    EXPECT_EQ(PORTC.OUTCLR, Led1Mask);
}

/**
 * @brief Test that the duty cycle decides how the period is split between on and off.
 */
TEST(Led, PwmSplitsThePeriodByDutyCycle)
{
    for (const auto& testCase : PwmSteps)
    {
        testHwPlatformReset();
        led_setup();
        led_pwm(LED1, testCase.percent);

        EXPECT_EQ(testDelayCount(), PwmDelayCount);
        EXPECT_EQ(testDelayAt(0U), testCase.onTime);
        EXPECT_EQ(testDelayAt(1U), static_cast<std::uint16_t>(PwmPeriodMs - testCase.onTime));
    }
}

/**
 * @brief Test that a duty cycle between two steps is rounded to the nearest one.
 *
 *        The period is ten milliseconds long and the platform delays in whole milliseconds, so
 *        the LED has ten brightness steps regardless of how finely the duty cycle is given.
 */
TEST(Led, PwmRoundsToTheNearestStep)
{
    for (const auto& testCase : PwmRounded)
    {
        testHwPlatformReset();
        led_setup();
        led_pwm(LED1, testCase.percent);

        EXPECT_EQ(testDelayAt(0U), testCase.onTime);
    }
}

/**
 * @brief Test that a PWM period always lasts the same length of time.
 *
 *        The brightness comes from how the period is divided, so a duty cycle must never
 *        stretch or shorten the period itself.
 */
TEST(Led, PwmAlwaysRunsOneFullPeriod)
{
    for (const auto& testCase : PwmSteps)
    {
        testHwPlatformReset();
        led_setup();
        led_pwm(LED1, testCase.percent);

        const auto total = static_cast<std::uint16_t>(testDelayAt(0U) + testDelayAt(1U));
        EXPECT_EQ(total, PwmPeriodMs);
    }
}

/**
 * @brief Test that each LED can be dimmed, and that PWM drives the requested one only.
 */
TEST(Led, PwmDrivesTheRequestedLed)
{
    constexpr std::uint8_t halfBrightness{50U};

    for (std::uint8_t led{LED1}; led <= LED3; ++led)
    {
        const auto mask = static_cast<std::uint8_t>(1U << led);

        testHwPlatformReset();
        led_setup();
        led_pwm(led, halfBrightness);

        EXPECT_EQ(PORTC.OUTSET, mask);
        EXPECT_EQ(PORTC.OUTCLR, mask);
    }
}

/**
 * @brief Test that a duty cycle above 100 percent is ignored.
 *
 *        Nothing is driven and no time is spent, so a bad duty cycle costs the caller nothing.
 */
TEST(Led, PwmIgnoresDutyCycleAboveHundred)
{
    constexpr std::uint8_t justAboveMax{101U};
    constexpr std::uint8_t largest{255U};

    testHwPlatformReset();
    led_setup();
    led_pwm(LED1, justAboveMax);
    led_pwm(LED1, largest);

    EXPECT_EQ(testDelayCount(), NoDelays);
    EXPECT_EQ(PORTC.OUTSET, zero);
    EXPECT_EQ(PORTC.OUTCLR, zero);
}
} // namespace
