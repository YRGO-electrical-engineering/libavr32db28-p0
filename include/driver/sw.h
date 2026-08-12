/**
 * @file AVR32DB28 switch driver.
 */
#ifndef SW_H_
#define SW_H_

#include <stdbool.h>
#include <stdint.h>

/** Switch IDs. */
#define SW1 0U // Switch 1.
#define SW2 1U // Switch 2.
#define SW3 2U // Switch 3.

/**
 * @brief Initialize all switches.
 */
void sw_setup(void);

/**
 * @brief Read switch state.
 *
 * @param[in] sw Switch ID.
 *
 * @return True if the switch is pressed, false otherwise.
 */
bool sw_read(uint8_t sw);

#endif /** SW_H_ */
