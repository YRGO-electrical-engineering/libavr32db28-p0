/**
 * @file Tests for the AVR32DB28 digital output driver.
 */
#include <cstdint>

#include "arch/avr/hw_platform.h"
#include "yrgo/test/test.h"

extern "C"
{
#include "driver/digital_out.h"
} // extern "C"

namespace
{
constexpr std::uint8_t zero{0U};

/** Number of pins the driver offers (PC0 - PA7). */
constexpr std::uint8_t PinCount{21U};

/** The first pin ID the driver does not know about, and one far beyond it. */
constexpr std::uint8_t FirstUnknownPin{PinCount};
constexpr std::uint8_t FarUnknownPin{255U};

/**
 * @brief Pin structure for testing.
 */
struct TestPin
{
    /** I/O port the pin sits on. */
    PORT_t* port;

    /** Enable the pin. */
    void (*on)(void);

    /** Disable the pin. */
    void (*off)(void);

    /** Pin ID passed to the driver. */
    const std::uint8_t pin;

    /** Bit mask of the pin within that port. */
    const std::uint8_t mask;
};

/** Every pin the driver offers, in the order the pin IDs run. */
constexpr TestPin Pins[]{
    {&PORTC, digital_out_PC0_on, digital_out_PC0_off, PC0, PIN0_bm},
    {&PORTC, digital_out_PC1_on, digital_out_PC1_off, PC1, PIN1_bm},
    {&PORTC, digital_out_PC2_on, digital_out_PC2_off, PC2, PIN2_bm},
    {&PORTC, digital_out_PC3_on, digital_out_PC3_off, PC3, PIN3_bm},
    {&PORTD, digital_out_PD1_on, digital_out_PD1_off, PD1, PIN1_bm},
    {&PORTD, digital_out_PD2_on, digital_out_PD2_off, PD2, PIN2_bm},
    {&PORTD, digital_out_PD3_on, digital_out_PD3_off, PD3, PIN3_bm},
    {&PORTD, digital_out_PD4_on, digital_out_PD4_off, PD4, PIN4_bm},
    {&PORTD, digital_out_PD5_on, digital_out_PD5_off, PD5, PIN5_bm},
    {&PORTD, digital_out_PD6_on, digital_out_PD6_off, PD6, PIN6_bm},
    {&PORTD, digital_out_PD7_on, digital_out_PD7_off, PD7, PIN7_bm},
    {&PORTF, digital_out_PF0_on, digital_out_PF0_off, PF0, PIN0_bm},
    {&PORTF, digital_out_PF1_on, digital_out_PF1_off, PF1, PIN1_bm},
    {&PORTA, digital_out_PA0_on, digital_out_PA0_off, PA0, PIN0_bm},
    {&PORTA, digital_out_PA1_on, digital_out_PA1_off, PA1, PIN1_bm},
    {&PORTA, digital_out_PA2_on, digital_out_PA2_off, PA2, PIN2_bm},
    {&PORTA, digital_out_PA3_on, digital_out_PA3_off, PA3, PIN3_bm},
    {&PORTA, digital_out_PA4_on, digital_out_PA4_off, PA4, PIN4_bm},
    {&PORTA, digital_out_PA5_on, digital_out_PA5_off, PA5, PIN5_bm},
    {&PORTA, digital_out_PA6_on, digital_out_PA6_off, PA6, PIN6_bm},
    {&PORTA, digital_out_PA7_on, digital_out_PA7_off, PA7, PIN7_bm},
};

/**
 * @brief Port structure for testing.
 */
struct PortTest
{
    /** I/O port. */
    PORT_t* port;

    /** Expected contents of DIR once the driver has been initialized. */
    std::uint8_t dir;
};

/** Every port the driver touches, and the pins it owns on each. */
constexpr PortTest Ports[]{
    {&PORTA, PIN0_bm | PIN1_bm | PIN2_bm | PIN3_bm | PIN4_bm | PIN5_bm | PIN6_bm | PIN7_bm},
    {&PORTC, PIN0_bm | PIN1_bm | PIN2_bm | PIN3_bm},
    {&PORTD, PIN1_bm | PIN2_bm | PIN3_bm | PIN4_bm | PIN5_bm | PIN6_bm | PIN7_bm},
    {&PORTF, PIN0_bm | PIN1_bm},
};

/**
 * @brief Check that no port other than the given one has been driven.
 *
 * @param[in] driven The port the driver was expected to write to.
 */
void expectOtherPortsUntouched(const PORT_t* driven)
{
    for (const auto& test : Ports)
    {
        if (test.port == driven) { continue; }

        EXPECT_EQ(test.port->OUTSET, zero);
        EXPECT_EQ(test.port->OUTCLR, zero);
        EXPECT_EQ(test.port->OUTTGL, zero);
    }
}

/**
 * @brief Test that the pin IDs run from PC0 to PA7 without gaps.
 */
TEST(DigitalOut, PinIdsAreContiguous)
{
    constexpr std::uint8_t lastPin{static_cast<std::uint8_t>(PinCount - 1U)};
    std::uint8_t expected{0U};

    for (const auto& test : Pins)
    {
        EXPECT_EQ(test.pin, expected);
        ++expected;
    }

    EXPECT_EQ(expected, PinCount);
    EXPECT_EQ(static_cast<std::uint8_t>(PA7), lastPin);
}

/**
 * @brief Test that initializing the driver configures every pin as an output.
 *
 *        Asserting on the whole DIR register rather than one bit at a time also catches a
 *        driver that claims pins it was never given, e.g. PD0.
 */
TEST(DigitalOut, SetupConfiguresEveryPinAsOutput)
{
    testHwPlatformReset();
    digital_out_setup();

    for (const auto& test : Ports)
    {
        EXPECT_EQ(test.port->DIR, test.dir);
    }
}

/**
 * @brief Test that the pins are left off after initialization.
 */
TEST(DigitalOut, SetupLeavesTheOutputsOff)
{
    testHwPlatformReset();
    digital_out_setup();

    for (const auto& test : Ports)
    {
        EXPECT_EQ(test.port->OUT, zero);
        EXPECT_EQ(test.port->OUTSET, zero);
        EXPECT_EQ(test.port->OUTCLR, zero);
        EXPECT_EQ(test.port->OUTTGL, zero);
    }
}

/**
 * @brief Test that turning a pin on sets its own bit, on its own port.
 */
TEST(DigitalOut, WriteHighDrivesTheNamedPin)
{
    for (const auto& test : Pins)
    {
        testHwPlatformReset();
        digital_out_setup();
        digital_out_write(test.pin, true);

        EXPECT_EQ(test.port->OUTSET, test.mask);
        EXPECT_EQ(test.port->OUTCLR, zero);
    }
}

/**
 * @brief Test that turning a pin off clears its own bit, on its own port.
 */
TEST(DigitalOut, WriteLowDrivesTheNamedPin)
{
    for (const auto& test : Pins)
    {
        testHwPlatformReset();
        digital_out_setup();
        digital_out_write(test.pin, false);

        EXPECT_EQ(test.port->OUTCLR, test.mask);
        EXPECT_EQ(test.port->OUTSET, zero);
    }
}

/**
 * @brief Test that driving a pin leaves the other ports alone.
 */
TEST(DigitalOut, WriteTouchesOnlyItsOwnPort)
{
    for (const auto& test : Pins)
    {
        testHwPlatformReset();
        digital_out_setup();

        digital_out_write(test.pin, true);
        expectOtherPortsUntouched(test.port);
        digital_out_write(test.pin, false);
        expectOtherPortsUntouched(test.port);
    }
}

/**
 * @brief Test that PD1 - PD7 sit on bit 1 - 7 and that bit 0 of PORTD is never driven.
 */
TEST(DigitalOut, PortDPinsSkipBitZero)
{
    testHwPlatformReset();
    digital_out_setup();
    digital_out_write(PD1, true);
    EXPECT_EQ(PORTD.OUTSET, PIN1_bm);

    testHwPlatformReset();
    digital_out_setup();
    digital_out_write(PD7, true);
    EXPECT_EQ(PORTD.OUTSET, PIN7_bm);

    // No pin at all may drive bit 0 of PORTD.
    for (const auto& test : Pins)
    {
        testHwPlatformReset();
        digital_out_setup();

        digital_out_write(test.pin, true);
        EXPECT_EQ(PORTD.OUTSET & PIN0_bm, zero);
        digital_out_write(test.pin, false);
        EXPECT_EQ(PORTD.OUTCLR & PIN0_bm, zero);
    }
}

/**
 * @brief Test that reading a pin returns the state held in the OUT register.
 */
TEST(DigitalOut, ReadReturnsThePinState)
{
    for (const auto& test : Pins)
    {
        testHwPlatformReset();
        digital_out_setup();

        EXPECT_FALSE(digital_out_read(test.pin));
        test.port->OUT = test.mask;
        EXPECT_TRUE(digital_out_read(test.pin));
    }
}

/**
 * @brief Test that reading a pin ignores every other pin on the same port.
 *
 *        Seeding OUT with all bits but this one catches a mask covering more than the pin it
 *        names, which a read of the pin on its own would not.
 */
TEST(DigitalOut, ReadIgnoresTheOtherPinsOnTheSamePort)
{
    for (const auto& test : Pins)
    {
        testHwPlatformReset();
        digital_out_setup();
        test.port->OUT = static_cast<std::uint8_t>(~test.mask);

        EXPECT_FALSE(digital_out_read(test.pin));
    }
}

/**
 * @brief Test that reading a pin doesn't disturb any register.
 */
TEST(DigitalOut, ReadDoesNotWriteRegisters)
{
    testHwPlatformReset();

    for (const auto& test : Pins)
    {
        (void)digital_out_read(test.pin);
    }

    for (const auto& test : Ports)
    {
        EXPECT_EQ(test.port->DIR, zero);
        EXPECT_EQ(test.port->OUT, zero);
        EXPECT_EQ(test.port->OUTSET, zero);
        EXPECT_EQ(test.port->OUTCLR, zero);
        EXPECT_EQ(test.port->OUTTGL, zero);
    }
}

/**
 * @brief Test that writing to a pin that doesn't exist is ignored.
 */
TEST(DigitalOut, WriteIgnoresAnUnknownPin)
{
    testHwPlatformReset();
    digital_out_setup();

    digital_out_write(FirstUnknownPin, true);
    digital_out_write(FarUnknownPin, true);
    digital_out_write(FirstUnknownPin, false);
    digital_out_write(FarUnknownPin, false);

    for (const auto& test : Ports)
    {
        EXPECT_EQ(test.port->DIR, test.dir);
        EXPECT_EQ(test.port->OUTSET, zero);
        EXPECT_EQ(test.port->OUTCLR, zero);
        EXPECT_EQ(test.port->OUTTGL, zero);
    }
}

/**
 * @brief Test that a pin that doesn't exist reads as off rather than as on.
 */
TEST(DigitalOut, ReadReturnsFalseForAnUnknownPin)
{
    constexpr std::uint8_t allPinsOn{0xFFU};

    testHwPlatformReset();
    digital_out_setup();

    // Seed every port, so that a stray read cannot land on a register that happens to be clear.
    for (const auto& test : Ports)
    {
        test.port->OUT = allPinsOn;
    }
    EXPECT_FALSE(digital_out_read(FirstUnknownPin));
    EXPECT_FALSE(digital_out_read(FarUnknownPin));
}

/**
 * @brief Test that each pin's own on and off function drives the pin its name says.
 *
 *        These are what the students call, and there are 42 of them, so a copy-paste slip
 *        naming the wrong pin or the wrong state is easy to make and invisible in review.
 */
TEST(DigitalOut, NamedFunctionsDriveTheirOwnPin)
{
    for (const auto& test : Pins)
    {
        testHwPlatformReset();
        digital_out_setup();

        test.on();
        EXPECT_EQ(test.port->OUTSET, test.mask);
        EXPECT_EQ(test.port->OUTCLR, zero);
        expectOtherPortsUntouched(test.port);

        testHwPlatformReset();
        digital_out_setup();

        test.off();
        EXPECT_EQ(test.port->OUTCLR, test.mask);
        EXPECT_EQ(test.port->OUTSET, zero);
        expectOtherPortsUntouched(test.port);
    }
}

/**
 * @brief Test a full sequence: initialize, turn a pin on, read it back and turn it off.
 */
TEST(DigitalOut, DigitalOutSequence)
{
    testHwPlatformReset();
    digital_out_setup();
    EXPECT_EQ(PORTA.DIR, Ports[0U].dir);

    digital_out_PA3_on();
    EXPECT_EQ(PORTA.OUTSET, PIN3_bm);

    PORTA.OUT = PIN3_bm;
    EXPECT_TRUE(digital_out_read(PA3));

    digital_out_PA3_off();
    EXPECT_EQ(PORTA.OUTCLR, PIN3_bm);
}
} // namespace
